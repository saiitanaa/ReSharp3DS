#include "gui.h"

#include <3ds.h>
#include <citro2d.h>

#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

static const char* DIR_PATH = "sdmc:/ReSharp3DS";
static const char* APP_PATH = "sdmc:/ReSharp3DS/app.pe";
static const char* MSCORLIB_PATH = "sdmc:/ReSharp3DS/mscorlib.pe";

static C3D_RenderTarget* topScreen = nullptr;
static C3D_RenderTarget* bottomScreen = nullptr;
static C2D_TextBuf textBuf;

static bool initialized = false;

static bool folderFound = false;
static bool appFound = false;
static bool mscorlibFound = false;
static bool canLaunch = false;

static char statusText[128];

struct Button {
    float x;
    float y;
    float w;
    float h;
    const char* label;
};

static Button launchButton = { 35, 55, 250, 58, "Press A to start: app.pe" };
static Button exitButton = { 35, 140, 250, 42, "Press Start to exit" };

static bool file_exists(const char* path)
{
    struct stat st;
    return stat(path, &st) == 0;
}

static void check_filesystem(void)
{
    int result = mkdir(DIR_PATH, 0777);

    if (result == 0)
    {
        folderFound = true;
        appFound = false;
        mscorlibFound = false;
        canLaunch = false;

        snprintf(
            statusText,
            sizeof(statusText),
            "Folder created. Add app.pe and mscorlib.pe"
        );

        return;
    }

    if (errno == EEXIST)
    {
        folderFound = true;
        appFound = file_exists(APP_PATH);
        mscorlibFound = file_exists(MSCORLIB_PATH);
        canLaunch = appFound && mscorlibFound;

        if (canLaunch)
        {
            snprintf(statusText, sizeof(statusText), "Ready to launch.");
        }
        else
        {
            snprintf(statusText, sizeof(statusText), "Unable to launch the runtime!");
        }

        return;
    }

    folderFound = false;
    appFound = false;
    mscorlibFound = false;
    canLaunch = false;

    snprintf(
        statusText,
        sizeof(statusText),
        "Error creating folder. errno=%d",
        errno
    );
}

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

static void draw_text_in_button(const char* text, const Button& button, float scale, u32 color)
{
    C2D_Text c2dText;
    float width = 0.0f;
    float height = 0.0f;

    C2D_TextParse(&c2dText, textBuf, text);
    C2D_TextOptimize(&c2dText);
    C2D_TextGetDimensions(&c2dText, scale, scale, &width, &height);

    float x = button.x + (button.w - width) / 2.0f;
    float y = button.y + (button.h - height) / 2.0f - 2.0f;

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

static void draw_status_line(const char* label, bool ok, float y)
{
    u32 labelColor = C2D_Color32(190, 190, 205, 255);
    u32 okColor = ok
        ? C2D_Color32(90, 230, 130, 255)
        : C2D_Color32(240, 90, 90, 255);

    draw_text(label, 30, y, 0.45f, labelColor);
    draw_text(ok ? "OK" : "MISSING", 310, y, 0.45f, okColor);
}

static void draw_button(const Button& button, bool enabled, u32 color)
{
    u32 shadowColor = C2D_Color32(0, 0, 0, 90);
    u32 buttonColor = enabled ? color : C2D_Color32(70, 70, 80, 255);
    u32 borderColor = enabled
        ? C2D_Color32(255, 255, 255, 45)
        : C2D_Color32(255, 255, 255, 20);

    C2D_DrawRectSolid(
        button.x + 4,
        button.y + 4,
        0,
        button.w,
        button.h,
        shadowColor
    );

    C2D_DrawRectSolid(
        button.x,
        button.y,
        0,
        button.w,
        button.h,
        buttonColor
    );

    C2D_DrawRectSolid(
        button.x,
        button.y,
        0,
        button.w,
        2,
        borderColor
    );

    draw_text_in_button(
        button.label,
        button,
        0.58f,
        enabled
        ? C2D_Color32(255, 255, 255, 255)
        : C2D_Color32(170, 170, 180, 255)
    );
}

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
    u32 muted = C2D_Color32(175, 175, 195, 255);
    u32 cyan = C2D_Color32(95, 210, 255, 255);
    u32 green = C2D_Color32(60, 190, 105, 255);
    u32 red = C2D_Color32(190, 65, 75, 255);

    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

    // =========================
    // TOP SCREEN 400x240
    // =========================
    C2D_TargetClear(topScreen, bgTop);
    C2D_SceneBegin(topScreen);

    C2D_DrawRectSolid(0, 0, 0, 400, 42, headerColor);

    draw_text("ReSharp3DS Runtime", 18, 9, 0.62f, white);
    draw_text("v1.6.3-beta.5", 300, 12, 0.45f, C2D_Color32(230, 220, 255, 255));

    draw_text_centered(
        "Support project : github.com/saysaa/ReSharp3DS",
        400,
        51,
        0.45f,
        cyan
    );

    C2D_DrawRectSolid(18, 78, 0, 364, 112, cardColor);
    C2D_DrawRectSolid(18, 78, 0, 4, 112, headerColor);

    draw_text("Necessary files check", 32, 90, 0.55f, white);

    draw_status_line("Folder - :sdmc/ReSharp3DS", folderFound, 120);
    draw_status_line("app.pe", appFound, 143);
    draw_status_line("mscorlib.pe", mscorlibFound, 166);

    draw_text_centered(
        statusText,
        400,
        207,
        0.43f,
        canLaunch
        ? C2D_Color32(100, 240, 140, 255)
        : C2D_Color32(245, 190, 90, 255)
    );

    // =========================
    // BOTTOM SCREEN 320x240
    // =========================
    C2D_TargetClear(bottomScreen, bgBottom);
    C2D_SceneBegin(bottomScreen);


    draw_button(launchButton, canLaunch, green);
    draw_button(exitButton, true, red);

    draw_text_centered(
        "---",
        320,
        207,
        0.42f,
        muted
    );

    C3D_FrameEnd(0);
}

bool gui_can_launch(void)
{
    init_graphics_once();
    return canLaunch;
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