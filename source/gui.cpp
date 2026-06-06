#include "gui.h"

#include <3ds.h>
#include <citro2d.h>

#include <dirent.h>
#include <ctype.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

static const char* DIR_PATH = "sdmc:/ReSharp3DS";
static const char* MSCORLIB_PATH = "sdmc:/ReSharp3DS/mscorlib.pe";

#define MAX_PE_FILES 32
#define MAX_PE_NAME 128
#define MAX_PE_PATH 256

struct PeFile
{
    char name[MAX_PE_NAME];
    char path[MAX_PE_PATH];
};

static C3D_RenderTarget* topScreen = nullptr;
static C3D_RenderTarget* bottomScreen = nullptr;
static C2D_TextBuf textBuf = nullptr;

static bool initialized = false;

static bool folderFound = false;
static bool mscorlibFound = false;
static bool canLaunch = false;

static PeFile peFiles[MAX_PE_FILES];
static int peFileCount = 0;
static int selectedPeIndex = 0;
static int listOffset = 0;

static char statusText[128];
static char selectedAppPath[MAX_PE_PATH];

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

static void scan_pe_files(void)
{
    peFileCount = 0;
    selectedPeIndex = 0;
    listOffset = 0;
    selectedAppPath[0] = '\0';

    DIR* dir = opendir(DIR_PATH);

    if (!dir)
    {
        return;
    }

    struct dirent* entry;

    while ((entry = readdir(dir)) != NULL)
    {
        if (peFileCount >= MAX_PE_FILES)
        {
            break;
        }

        const char* name = entry->d_name;

        if (!ends_with_pe(name))
        {
            continue;
        }

        if (is_mscorlib_file(name))
        {
            continue;
        }

        snprintf(peFiles[peFileCount].name, MAX_PE_NAME, "%s", name);
        snprintf(peFiles[peFileCount].path, MAX_PE_PATH, "%s/%s", DIR_PATH, name);

        peFileCount++;
    }

    closedir(dir);

    if (peFileCount > 0)
    {
        snprintf(selectedAppPath, MAX_PE_PATH, "%s", peFiles[0].path);
    }
}

static void check_filesystem(void)
{
    int result = mkdir(DIR_PATH, 0777);

    if (result == 0)
    {
        folderFound = true;
        mscorlibFound = false;
        canLaunch = false;
        peFileCount = 0;
        selectedAppPath[0] = '\0';

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

        scan_pe_files();

        canLaunch = mscorlibFound && peFileCount > 0;

        if (canLaunch)
        {
            snprintf(statusText, sizeof(statusText), "Select a .pe and press A");
        }
        else if (!mscorlibFound)
        {
            snprintf(statusText, sizeof(statusText), "Missing mscorlib.pe!");
        }
        else
        {
            snprintf(statusText, sizeof(statusText), "No app .pe found!");
        }

        return;
    }

    folderFound = false;
    mscorlibFound = false;
    canLaunch = false;
    peFileCount = 0;
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

static void draw_pe_file_list(void)
{
    u32 white = C2D_Color32(255, 255, 255, 255);
    u32 muted = C2D_Color32(175, 175, 195, 255);
    u32 selectedBg = C2D_Color32(92, 54, 190, 255);
    u32 rowBg = C2D_Color32(28, 28, 40, 255);
    u32 disabled = C2D_Color32(120, 120, 135, 255);
    u32 green = C2D_Color32(90, 230, 130, 255);

    draw_text("Available .pe apps", 18, 14, 0.52f, white);

    if (peFileCount <= 0)
    {
        draw_text_centered("No .pe file found", 320, 92, 0.52f, disabled);
        draw_text_centered("Put apps in sdmc:/ReSharp3DS", 320, 120, 0.42f, muted);
        return;
    }

    int visibleRows = 5;
    int rowHeight = 28;
    int startY = 42;

    if (selectedPeIndex < listOffset)
    {
        listOffset = selectedPeIndex;
    }

    if (selectedPeIndex >= listOffset + visibleRows)
    {
        listOffset = selectedPeIndex - visibleRows + 1;
    }

    for (int i = 0; i < visibleRows; i++)
    {
        int index = listOffset + i;

        if (index >= peFileCount)
        {
            break;
        }

        int y = startY + i * rowHeight;
        bool selected = index == selectedPeIndex;

        C2D_DrawRectSolid(
            16,
            y,
            0,
            288,
            23,
            selected ? selectedBg : rowBg
        );

        char line[160];

        if (selected)
        {
            snprintf(line, sizeof(line), "> %s", peFiles[index].name);
        }
        else
        {
            snprintf(line, sizeof(line), "  %s", peFiles[index].name);
        }

        draw_text(line, 24, y + 4, 0.45f, selected ? white : muted);
    }

    draw_text("UP/DOWN: select", 20, 196, 0.40f, muted);
    draw_text("A: launch", 20, 214, 0.40f, green);
    draw_text("X: refresh", 180, 214, 0.40f, muted);
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
    draw_text("v1.7.3-beta.6", 300, 12, 0.45f, C2D_Color32(230, 220, 255, 255));

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
    draw_status_line("App checker", peFileCount > 0, 166);

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

    draw_pe_file_list();

    C3D_FrameEnd(0);
}

// ------------------------------------------------------------
// Public GUI API
// ------------------------------------------------------------

void gui_move_selection(int direction)
{
    if (peFileCount <= 0)
    {
        return;
    }

    selectedPeIndex += direction;

    if (selectedPeIndex < 0)
    {
        selectedPeIndex = peFileCount - 1;
    }

    if (selectedPeIndex >= peFileCount)
    {
        selectedPeIndex = 0;
    }

    snprintf(
        selectedAppPath,
        MAX_PE_PATH,
        "%s",
        peFiles[selectedPeIndex].path
    );
}

void gui_refresh_files(void)
{
    check_filesystem();
}

bool gui_can_launch(void)
{
    init_graphics_once();

    canLaunch = mscorlibFound && peFileCount > 0;

    return canLaunch;
}

const char* gui_get_selected_app_path(void)
{
    if (peFileCount <= 0)
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