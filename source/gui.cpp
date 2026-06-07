#include "gui.h"

#include <3ds.h>
#include <citro2d.h>

#include <dirent.h>
#include <ctype.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

static const char* ROOT_PATH = "sdmc:/ReSharp3DS";
static const char* MSCORLIB_PATH = "sdmc:/ReSharp3DS/mscorlib.pe";

#define MAX_BROWSER_ITEMS 64
#define MAX_ITEM_NAME 128
#define MAX_ITEM_PATH 256

enum BrowserItemType
{
    ITEM_PARENT = 0,
    ITEM_DIRECTORY = 1,
    ITEM_APP = 2
};

struct BrowserItem
{
    char name[MAX_ITEM_NAME];
    char path[MAX_ITEM_PATH];
    BrowserItemType type;
};

static C3D_RenderTarget* topScreen = nullptr;
static C3D_RenderTarget* bottomScreen = nullptr;
static C2D_TextBuf textBuf = nullptr;

static bool initialized = false;

static bool folderFound = false;
static bool mscorlibFound = false;
static bool canLaunch = false;

static BrowserItem browserItems[MAX_BROWSER_ITEMS];
static int browserItemCount = 0;
static int selectedIndex = 0;
static int listOffset = 0;

static char statusText[128];
static char selectedAppPath[MAX_ITEM_PATH];
static char currentDir[MAX_ITEM_PATH];

// ------------------------------------------------------------
// Forward declarations
// ------------------------------------------------------------

static void draw_text(const char* text, float x, float y, float scale, u32 color);
static void draw_text_centered(const char* text, float screenWidth, float y, float scale, u32 color);

// ------------------------------------------------------------
// Filesystem helpers
// ------------------------------------------------------------

static bool file_exists(const char* path)
{
    struct stat st;
    return stat(path, &st) == 0;
}

static bool path_is_directory(const char* path)
{
    struct stat st;

    if (stat(path, &st) != 0)
    {
        return false;
    }

    return S_ISDIR(st.st_mode);
}

static bool str_equals_ignore_case(const char* a, const char* b)
{
    if (!a || !b)
    {
        return false;
    }

    while (*a && *b)
    {
        char ca = (char)tolower((unsigned char)*a);
        char cb = (char)tolower((unsigned char)*b);

        if (ca != cb)
        {
            return false;
        }

        a++;
        b++;
    }

    return *a == '\0' && *b == '\0';
}

static bool ends_with_pe(const char* name)
{
    if (!name)
    {
        return false;
    }

    int len = strlen(name);

    if (len < 4)
    {
        return false;
    }

    const char* ext = name + len - 3;

    return str_equals_ignore_case(ext, ".pe");
}

static bool is_mscorlib_file(const char* name)
{
    return str_equals_ignore_case(name, "mscorlib.pe");
}

static bool is_root_directory(void)
{
    return strcmp(currentDir, ROOT_PATH) == 0;
}

static void reset_browser_selection(void)
{
    selectedIndex = 0;
    listOffset = 0;
    selectedAppPath[0] = '\0';
}

static void make_parent_path(const char* path, char* out, size_t outSize)
{
    if (!out || outSize == 0)
    {
        return;
    }

    out[0] = '\0';

    if (!path || path[0] == '\0')
    {
        snprintf(out, outSize, "%s", ROOT_PATH);
        return;
    }

    snprintf(out, outSize, "%s", path);

    int len = strlen(out);

    while (len > 0 && (out[len - 1] == '/' || out[len - 1] == '\\'))
    {
        out[len - 1] = '\0';
        len--;
    }

    int lastSlash = -1;

    for (int i = 0; out[i] != '\0'; i++)
    {
        if (out[i] == '/' || out[i] == '\\')
        {
            lastSlash = i;
        }
    }

    if (lastSlash >= 0)
    {
        out[lastSlash] = '\0';
    }

    if (strlen(out) < strlen(ROOT_PATH))
    {
        snprintf(out, outSize, "%s", ROOT_PATH);
    }

    if (strncmp(out, ROOT_PATH, strlen(ROOT_PATH)) != 0)
    {
        snprintf(out, outSize, "%s", ROOT_PATH);
    }

    if (out[0] == '\0')
    {
        snprintf(out, outSize, "%s", ROOT_PATH);
    }
}

static void set_current_directory(const char* path)
{
    if (!path || path[0] == '\0')
    {
        snprintf(currentDir, sizeof(currentDir), "%s", ROOT_PATH);
        return;
    }

    if (strncmp(path, ROOT_PATH, strlen(ROOT_PATH)) != 0)
    {
        snprintf(currentDir, sizeof(currentDir), "%s", ROOT_PATH);
        return;
    }

    snprintf(currentDir, sizeof(currentDir), "%s", path);
}

static void add_browser_item(const char* name, const char* path, BrowserItemType type)
{
    if (browserItemCount >= MAX_BROWSER_ITEMS)
    {
        return;
    }

    snprintf(browserItems[browserItemCount].name, MAX_ITEM_NAME, "%s", name);
    snprintf(browserItems[browserItemCount].path, MAX_ITEM_PATH, "%s", path);
    browserItems[browserItemCount].type = type;

    browserItemCount++;
}

static void scan_current_directory(void)
{
    browserItemCount = 0;
    reset_browser_selection();

    if (!is_root_directory())
    {
        char parentPath[MAX_ITEM_PATH];
        make_parent_path(currentDir, parentPath, sizeof(parentPath));
        add_browser_item("..", parentPath, ITEM_PARENT);
    }

    DIR* dir = opendir(currentDir);

    if (!dir)
    {
        snprintf(statusText, sizeof(statusText), "Cannot open folder");
        return;
    }

    struct dirent* entry;

    while ((entry = readdir(dir)) != NULL)
    {
        if (browserItemCount >= MAX_BROWSER_ITEMS)
        {
            break;
        }

        const char* name = entry->d_name;

        if (!name || name[0] == '\0')
        {
            continue;
        }

        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
        {
            continue;
        }

        char fullPath[MAX_ITEM_PATH];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", currentDir, name);

        if (path_is_directory(fullPath))
        {
            add_browser_item(name, fullPath, ITEM_DIRECTORY);
            continue;
        }

        if (!ends_with_pe(name))
        {
            continue;
        }

        if (is_mscorlib_file(name))
        {
            continue;
        }

        add_browser_item(name, fullPath, ITEM_APP);
    }

    closedir(dir);

    if (browserItemCount > 0)
    {
        if (browserItems[0].type == ITEM_APP)
        {
            snprintf(selectedAppPath, MAX_ITEM_PATH, "%s", browserItems[0].path);
        }
        else
        {
            selectedAppPath[0] = '\0';
        }
    }
}

static bool selected_item_is_app(void)
{
    if (selectedIndex < 0 || selectedIndex >= browserItemCount)
    {
        return false;
    }

    return browserItems[selectedIndex].type == ITEM_APP;
}

static bool selected_item_is_directory_like(void)
{
    if (selectedIndex < 0 || selectedIndex >= browserItemCount)
    {
        return false;
    }

    return browserItems[selectedIndex].type == ITEM_DIRECTORY ||
        browserItems[selectedIndex].type == ITEM_PARENT;
}

static void update_selected_app_path(void)
{
    selectedAppPath[0] = '\0';

    if (selected_item_is_app())
    {
        snprintf(selectedAppPath, MAX_ITEM_PATH, "%s", browserItems[selectedIndex].path);
    }
}

static void open_selected_directory(void)
{
    if (!selected_item_is_directory_like())
    {
        return;
    }

    set_current_directory(browserItems[selectedIndex].path);
    scan_current_directory();

}

static void check_filesystem(void)
{
    int result = mkdir(ROOT_PATH, 0777);

    if (result == 0)
    {
        folderFound = true;
        mscorlibFound = false;
        canLaunch = false;
        browserItemCount = 0;
        selectedAppPath[0] = '\0';

        set_current_directory(ROOT_PATH);

        snprintf(
            statusText,
            sizeof(statusText),
            "Folder created - Add .pe files and mscorlib.pe"
        );

        return;
    }

    if (errno == EEXIST)
    {
        folderFound = true;
        mscorlibFound = file_exists(MSCORLIB_PATH);

        if (currentDir[0] == '\0')
        {
            set_current_directory(ROOT_PATH);
        }

        scan_current_directory();

        canLaunch = mscorlibFound && selected_item_is_app();

        if (!mscorlibFound)
        {
            snprintf(statusText, sizeof(statusText), "Missing mscorlib.pe!");
        }
        else if (browserItemCount <= 0)
        {
            snprintf(statusText, sizeof(statusText), "No apps or folders found");
        }

        return;
    }

    folderFound = false;
    mscorlibFound = false;
    canLaunch = false;
    browserItemCount = 0;
    selectedAppPath[0] = '\0';

    snprintf(
        statusText,
        sizeof(statusText),
        "Error creating folder. errno=%d",
        errno
    );
}

// ------------------------------------------------------------
// Drawing helpers
// ------------------------------------------------------------

static void draw_text(const char* text, float x, float y, float scale, u32 color)
{
    C2D_Text c2dText;

    C2D_TextParse(&c2dText, textBuf, text);
    C2D_TextOptimize(&c2dText);

    C2D_DrawText(
        &c2dText,
        C2D_WithColor,
        x,
        y,
        0.0f,
        scale,
        scale,
        color
    );
}

static void draw_text_centered(const char* text, float screenWidth, float y, float scale, u32 color)
{
    C2D_Text c2dText;
    float width = 0.0f;
    float height = 0.0f;

    C2D_TextParse(&c2dText, textBuf, text);
    C2D_TextOptimize(&c2dText);
    C2D_TextGetDimensions(&c2dText, scale, scale, &width, &height);

    C2D_DrawText(
        &c2dText,
        C2D_WithColor,
        (screenWidth - width) / 2.0f,
        y,
        0.0f,
        scale,
        scale,
        color
    );
}

static void draw_status_line(const char* label, bool ok, float y)
{
    u32 labelColor = C2D_Color32(190, 190, 205, 255);
    u32 okColor = ok
        ? C2D_Color32(90, 230, 130, 255)
        : C2D_Color32(240, 90, 90, 255);

    draw_text(label, 30, y, 0.45f, labelColor);
    draw_text(ok ? "OK" : "MISSING", 310, y, 0.45f, okColor);
}

static void make_short_path(const char* path, char* out, size_t outSize)
{
    if (!out || outSize == 0)
    {
        return;
    }

    if (!path)
    {
        out[0] = '\0';
        return;
    }

    const char* prefix = "sdmc:/ReSharp3DS";

    if (strncmp(path, prefix, strlen(prefix)) == 0)
    {
        const char* tail = path + strlen(prefix);

        if (tail[0] == '\0')
        {
            snprintf(out, outSize, "/");
        }
        else
        {
            snprintf(out, outSize, "%s", tail);
        }

        return;
    }

    snprintf(out, outSize, "%s", path);
}

static void draw_browser_list(void)
{
    u32 white = C2D_Color32(255, 255, 255, 255);
    u32 muted = C2D_Color32(175, 175, 195, 255);
    u32 selectedBg = C2D_Color32(92, 54, 190, 255);
    u32 rowBg = C2D_Color32(28, 28, 40, 255);
    u32 disabled = C2D_Color32(120, 120, 135, 255);
    u32 green = C2D_Color32(90, 230, 130, 255);
    u32 blue = C2D_Color32(95, 210, 255, 255);
    u32 yellow = C2D_Color32(245, 190, 90, 255);


    char shortPath[128];
    make_short_path(currentDir, shortPath, sizeof(shortPath));

    draw_text(shortPath, 18, 31, 0.34f, muted);

    if (browserItemCount <= 0)
    {
        draw_text_centered("No folder or .pe found", 320, 92, 0.52f, disabled);
        draw_text_centered("Put apps in sdmc:/ReSharp3DS", 320, 120, 0.42f, muted);
        return;
    }

    int visibleRows = 5;
    int rowHeight = 28;
    int startY = 55;

    if (selectedIndex < listOffset)
    {
        listOffset = selectedIndex;
    }

    if (selectedIndex >= listOffset + visibleRows)
    {
        listOffset = selectedIndex - visibleRows + 1;
    }

    for (int i = 0; i < visibleRows; i++)
    {
        int index = listOffset + i;

        if (index >= browserItemCount)
        {
            break;
        }

        int y = startY + i * rowHeight;
        bool selected = index == selectedIndex;

        C2D_DrawRectSolid(
            16,
            y,
            0,
            288,
            23,
            selected ? selectedBg : rowBg
        );

        char line[180];

        const char* tag = "     ";
        u32 textColor = muted;

        if (browserItems[index].type == ITEM_PARENT)
        {
            tag = "[..] ";
            textColor = yellow;
        }
        else if (browserItems[index].type == ITEM_DIRECTORY)
        {
            textColor = blue;
        }
        else if (browserItems[index].type == ITEM_APP)
        {
            textColor = green;
        }

        if (selected)
        {
            snprintf(line, sizeof(line), "> %s", browserItems[index].name);
        }
        else
        {
            snprintf(line, sizeof(line), "  %s", browserItems[index].name);
        }

        draw_text(line, 24, y + 4, 0.42f, selected ? white : textColor);
    }

    draw_text("X: refresh", 20, 220, 0.38f, muted);
}

// ------------------------------------------------------------
// Init / render
// ------------------------------------------------------------

static void init_graphics_once(void)
{
    if (initialized)
    {
        return;
    }

    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    topScreen = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    bottomScreen = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    textBuf = C2D_TextBufNew(4096);

    set_current_directory(ROOT_PATH);
    check_filesystem();

    initialized = true;
}

void run_gui(void)
{
    init_graphics_once();

    C2D_TextBufClear(textBuf);

    u32 bgTop = C2D_Color32(14, 14, 22, 255);
    u32 bgBottom = C2D_Color32(10, 10, 16, 255);
    u32 headerColor = C2D_Color32(92, 54, 190, 255);
    u32 cardColor = C2D_Color32(28, 28, 40, 255);
    u32 white = C2D_Color32(255, 255, 255, 255);
    u32 cyan = C2D_Color32(95, 210, 255, 255);

    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

    // Top screen
    C2D_TargetClear(topScreen, bgTop);
    C2D_SceneBegin(topScreen);

    C2D_DrawRectSolid(0, 0, 0, 400, 42, headerColor);

    draw_text("ReSharp3DS Runtime", 18, 9, 0.62f, white);
    draw_text("v1.8.3-beta.7", 300, 12, 0.45f, C2D_Color32(230, 220, 255, 255));

    draw_text_centered(
        "Support project : github.com/saysaa/ReSharp3DS",
        400,
        51,
        0.45f,
        cyan
    );

    C2D_DrawRectSolid(18, 78, 0, 364, 112, cardColor);
    C2D_DrawRectSolid(18, 78, 0, 4, 112, headerColor);

    draw_text("Integrity Check", 32, 90, 0.55f, white);

    draw_status_line("Folder - :sdmc/ReSharp3DS", folderFound, 120);
    draw_status_line("mscorlib.pe", mscorlibFound, 143);
    draw_status_line("Explorer", browserItemCount > 0, 166);

    draw_text_centered(
        statusText,
        400,
        207,
        0.43f,
        canLaunch
        ? C2D_Color32(100, 240, 140, 255)
        : C2D_Color32(245, 190, 90, 255)
    );

    // Bottom screen
    C2D_TargetClear(bottomScreen, bgBottom);
    C2D_SceneBegin(bottomScreen);

    draw_browser_list();

    C3D_FrameEnd(0);
}

// ------------------------------------------------------------
// Public GUI API
// ------------------------------------------------------------

void gui_move_selection(int direction)
{
    if (browserItemCount <= 0)
    {
        return;
    }

    selectedIndex += direction;

    if (selectedIndex < 0)
    {
        selectedIndex = browserItemCount - 1;
    }

    if (selectedIndex >= browserItemCount)
    {
        selectedIndex = 0;
    }

    update_selected_app_path();

    canLaunch = mscorlibFound && selected_item_is_app();
}

void gui_refresh_files(void)
{
    check_filesystem();
}

bool gui_can_launch(void)
{
    init_graphics_once();

    if (!mscorlibFound)
    {
        canLaunch = false;
        snprintf(statusText, sizeof(statusText), "Missing mscorlib.pe!");
        return false;
    }

    if (browserItemCount <= 0)
    {
        canLaunch = false;
        snprintf(statusText, sizeof(statusText), "No apps or folders found");
        return false;
    }

    if (selected_item_is_directory_like())
    {
        open_selected_directory();
        canLaunch = false;
        return false;
    }

    if (selected_item_is_app())
    {
        update_selected_app_path();
        canLaunch = true;
        return true;
    }

    canLaunch = false;
    return false;
}

const char* gui_get_selected_app_path(void)
{
    if (!selected_item_is_app())
    {
        return NULL;
    }

    return selectedAppPath;
}

void gui_shutdown(void)
{
    if (!initialized)
    {
        return;
    }

    if (textBuf)
    {
        C2D_TextBufDelete(textBuf);
        textBuf = nullptr;
    }

    C2D_Fini();
    C3D_Fini();

    topScreen = nullptr;
    bottomScreen = nullptr;

    initialized = false;
}
