#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "gui.h"

#include <nanoCLR_Types.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Application.h>
#include <nanoCLR_Interop.h>

#ifndef SUCCEEDED
#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#endif

extern "C" {
    HRESULT nanoCLR_Initialize(const void* settings);
    void nanoCLR_Cleanup();
}

static CLR_RT_Assembly* g_mscorlibAssembly = NULL;
static CLR_RT_Assembly* g_appAssembly = NULL;

static CLR_RT_MethodHandler g_appNativeMethods[1024];
static CLR_RT_MethodHandler g_mscorlibNativeMethods[1024];

static bool g_graphicsUsed = false;
static bool g_consoleModeScreenShown = false;

// ------------------------------------------------------------
// mscorlib temporary patch
// ------------------------------------------------------------

static HRESULT Native_Nop(CLR_RT_StackFrame& stack)
{
    return S_OK;
}

static void InitMscorlibNativeTable()
{
    for (int i = 0; i < 1024; i++)
    {
        g_mscorlibNativeMethods[i] = NULL;
    }

    g_mscorlibNativeMethods[3] = Native_Nop;
    g_mscorlibAssembly->m_nativeCode = g_mscorlibNativeMethods;

    printf("[PATCH] mscorlib method=3 patched as NOP\n");
}

// ------------------------------------------------------------
// Console API
// ------------------------------------------------------------

static HRESULT Native_Clear(CLR_RT_StackFrame& stack)
{
    consoleClear();
    return S_OK;
}

static HRESULT Native_Write(CLR_RT_StackFrame& stack)
{
    CLR_RT_HeapBlock& arg = stack.Arg0();

    const char* text = arg.RecoverString();

    if (text == NULL)
    {
        text = "";
    }

    printf("%s", text);

    return S_OK;
}

static HRESULT Native_WriteLine(CLR_RT_StackFrame& stack)
{
    CLR_RT_HeapBlock& arg = stack.Arg0();

    const char* text = arg.RecoverString();

    if (text == NULL)
    {
        text = "";
    }

    printf("%s\n", text);

    return S_OK;
}

static HRESULT Native_WriteInt(CLR_RT_StackFrame& stack)
{
    int value = stack.Arg0().NumericByRef().s4;

    printf("%d", value);

    return S_OK;
}

static HRESULT Native_WriteLineInt(CLR_RT_StackFrame& stack)
{
    int value = stack.Arg0().NumericByRef().s4;

    printf("%d\n", value);

    return S_OK;
}

// ------------------------------------------------------------
// Input API
// ------------------------------------------------------------

static HRESULT Native_IsKeyPressed(CLR_RT_StackFrame& stack, u32 key)
{
    hidScanInput();

    u32 keys = hidKeysHeld();

    stack.SetResult_I4((keys & key) ? 1 : 0);

    return S_OK;
}

static HRESULT Native_IsStartPressed(CLR_RT_StackFrame& stack)
{
    return Native_IsKeyPressed(stack, KEY_START);
}

static HRESULT Native_IsSelectPressed(CLR_RT_StackFrame& stack)
{
    return Native_IsKeyPressed(stack, KEY_SELECT);
}

static HRESULT Native_IsAPressed(CLR_RT_StackFrame& stack)
{
    return Native_IsKeyPressed(stack, KEY_A);
}

static HRESULT Native_IsBPressed(CLR_RT_StackFrame& stack)
{
    return Native_IsKeyPressed(stack, KEY_B);
}

static HRESULT Native_IsXPressed(CLR_RT_StackFrame& stack)
{
    return Native_IsKeyPressed(stack, KEY_X);
}

static HRESULT Native_IsYPressed(CLR_RT_StackFrame& stack)
{
    return Native_IsKeyPressed(stack, KEY_Y);
}

static HRESULT Native_IsLPressed(CLR_RT_StackFrame& stack)
{
    return Native_IsKeyPressed(stack, KEY_L);
}

static HRESULT Native_IsRPressed(CLR_RT_StackFrame& stack)
{
    return Native_IsKeyPressed(stack, KEY_R);
}

static HRESULT Native_IsUpPressed(CLR_RT_StackFrame& stack)
{
    return Native_IsKeyPressed(stack, KEY_DUP);
}

static HRESULT Native_IsDownPressed(CLR_RT_StackFrame& stack)
{
    return Native_IsKeyPressed(stack, KEY_DDOWN);
}

static HRESULT Native_IsLeftPressed(CLR_RT_StackFrame& stack)
{
    return Native_IsKeyPressed(stack, KEY_DLEFT);
}

static HRESULT Native_IsRightPressed(CLR_RT_StackFrame& stack)
{
    return Native_IsKeyPressed(stack, KEY_DRIGHT);
}

// ------------------------------------------------------------
// Graphics API
// Bottom screen: 320x240
// Top screen: debug console
// ------------------------------------------------------------

static void DecodeColor(int color, u8& r, u8& g, u8& b)
{
    r = (color >> 16) & 0xFF;
    g = (color >> 8) & 0xFF;
    b = color & 0xFF;
}

static u8* GetBottomFramebuffer()
{
    u16 fbWidth = 0;
    u16 fbHeight = 0;

    return gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, &fbWidth, &fbHeight);
}

static void PutPixelBottomRaw(u8* fb, int x, int y, int color)
{
    if (fb == NULL)
    {
        return;
    }

    if (x < 0 || x >= 320 || y < 0 || y >= 240)
    {
        return;
    }

    u8 r, g, b;
    DecodeColor(color, r, g, b);

    int pos = ((239 - y) + x * 240) * 3;

    fb[pos + 0] = b;
    fb[pos + 1] = g;
    fb[pos + 2] = r;
}

static void PutPixelBottom(int x, int y, int color)
{
    u8* fb = GetBottomFramebuffer();

    PutPixelBottomRaw(fb, x, y, color);
}

static void FillRectBottom(int x, int y, int width, int height, int color)
{
    if (width <= 0 || height <= 0)
    {
        return;
    }

    u8* fb = GetBottomFramebuffer();

    if (fb == NULL)
    {
        return;
    }

    int x0 = x;
    int y0 = y;
    int x1 = x + width;
    int y1 = y + height;

    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > 320) x1 = 320;
    if (y1 > 240) y1 = 240;

    for (int px = x0; px < x1; px++)
    {
        for (int py = y0; py < y1; py++)
        {
            PutPixelBottomRaw(fb, px, py, color);
        }
    }
}

static void DrawRectBottom(int x, int y, int width, int height, int color)
{
    if (width <= 0 || height <= 0)
    {
        return;
    }

    for (int px = x; px < x + width; px++)
    {
        PutPixelBottom(px, y, color);
        PutPixelBottom(px, y + height - 1, color);
    }

    for (int py = y; py < y + height; py++)
    {
        PutPixelBottom(x, py, color);
        PutPixelBottom(x + width - 1, py, color);
    }
}

// ------------------------------------------------------------
// Small 5x7 bitmap font
// ------------------------------------------------------------

static const u8* GetGlyph5x7(char c)
{
    static const u8 SPACE[7] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    static const u8 A[7] = { 0x0E, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11 };
    static const u8 B[7] = { 0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E };
    static const u8 C[7] = { 0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E };
    static const u8 D[7] = { 0x1E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1E };
    static const u8 E[7] = { 0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F };
    static const u8 F[7] = { 0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10 };
    static const u8 G[7] = { 0x0E, 0x11, 0x10, 0x13, 0x11, 0x11, 0x0E };
    static const u8 H[7] = { 0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11 };
    static const u8 I[7] = { 0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x1F };
    static const u8 J[7] = { 0x01, 0x01, 0x01, 0x01, 0x11, 0x11, 0x0E };
    static const u8 K[7] = { 0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11 };
    static const u8 L[7] = { 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F };
    static const u8 M[7] = { 0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11 };
    static const u8 N[7] = { 0x11, 0x19, 0x15, 0x13, 0x11, 0x11, 0x11 };
    static const u8 O[7] = { 0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E };
    static const u8 P[7] = { 0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10 };
    static const u8 Q[7] = { 0x0E, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0D };
    static const u8 R[7] = { 0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11 };
    static const u8 S[7] = { 0x0F, 0x10, 0x10, 0x0E, 0x01, 0x01, 0x1E };
    static const u8 T[7] = { 0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04 };
    static const u8 U[7] = { 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E };
    static const u8 V[7] = { 0x11, 0x11, 0x11, 0x11, 0x0A, 0x0A, 0x04 };
    static const u8 W[7] = { 0x11, 0x11, 0x11, 0x15, 0x15, 0x15, 0x0A };
    static const u8 X[7] = { 0x11, 0x0A, 0x04, 0x04, 0x04, 0x0A, 0x11 };
    static const u8 Y[7] = { 0x11, 0x0A, 0x04, 0x04, 0x04, 0x04, 0x04 };
    static const u8 Z[7] = { 0x1F, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1F };

    static const u8 N0[7] = { 0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E };
    static const u8 N1[7] = { 0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E };
    static const u8 N2[7] = { 0x0E, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1F };
    static const u8 N3[7] = { 0x1E, 0x01, 0x01, 0x0E, 0x01, 0x01, 0x1E };
    static const u8 N4[7] = { 0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02 };
    static const u8 N5[7] = { 0x1F, 0x10, 0x10, 0x1E, 0x01, 0x01, 0x1E };
    static const u8 N6[7] = { 0x0E, 0x10, 0x10, 0x1E, 0x11, 0x11, 0x0E };
    static const u8 N7[7] = { 0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08 };
    static const u8 N8[7] = { 0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E };
    static const u8 N9[7] = { 0x0E, 0x11, 0x11, 0x0F, 0x01, 0x01, 0x0E };

    static const u8 EXCL[7] = { 0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x04 };
    static const u8 QMARK[7] = { 0x0E, 0x11, 0x01, 0x02, 0x04, 0x00, 0x04 };
    static const u8 DOT[7] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C };
    static const u8 COMMA[7] = { 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x08 };
    static const u8 COLON[7] = { 0x00, 0x0C, 0x0C, 0x00, 0x0C, 0x0C, 0x00 };
    static const u8 DASH[7] = { 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00 };
    static const u8 UNDERSCORE[7] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F };
    static const u8 GT[7] = { 0x10, 0x08, 0x04, 0x02, 0x04, 0x08, 0x10 };
    static const u8 LT[7] = { 0x01, 0x02, 0x04, 0x08, 0x04, 0x02, 0x01 };

    if (c >= 'a' && c <= 'z')
    {
        c = c - 32;
    }

    switch (c)
    {
    case 'A': return A;
    case 'B': return B;
    case 'C': return C;
    case 'D': return D;
    case 'E': return E;
    case 'F': return F;
    case 'G': return G;
    case 'H': return H;
    case 'I': return I;
    case 'J': return J;
    case 'K': return K;
    case 'L': return L;
    case 'M': return M;
    case 'N': return N;
    case 'O': return O;
    case 'P': return P;
    case 'Q': return Q;
    case 'R': return R;
    case 'S': return S;
    case 'T': return T;
    case 'U': return U;
    case 'V': return V;
    case 'W': return W;
    case 'X': return X;
    case 'Y': return Y;
    case 'Z': return Z;

    case '0': return N0;
    case '1': return N1;
    case '2': return N2;
    case '3': return N3;
    case '4': return N4;
    case '5': return N5;
    case '6': return N6;
    case '7': return N7;
    case '8': return N8;
    case '9': return N9;

    case '!': return EXCL;
    case '?': return QMARK;
    case '.': return DOT;
    case ',': return COMMA;
    case ':': return COLON;
    case '-': return DASH;
    case '_': return UNDERSCORE;
    case '>': return GT;
    case '<': return LT;

    default: return SPACE;
    }
}

static void DrawGlyph5x7(int x, int y, const u8* glyph, int color)
{
    int scale = 2;

    for (int row = 0; row < 7; row++)
    {
        for (int col = 0; col < 5; col++)
        {
            int bit = 1 << (4 - col);

            if (glyph[row] & bit)
            {
                FillRectBottom(
                    x + col * scale,
                    y + row * scale,
                    scale,
                    scale,
                    color
                );
            }
        }
    }
}

static void DrawTextBottom(int x, int y, const char* text, int color)
{
    if (text == NULL)
    {
        return;
    }

    int cursorX = x;
    int cursorY = y;

    for (int i = 0; text[i] != '\0'; i++)
    {
        char c = text[i];

        if (c == '\n')
        {
            cursorX = x;
            cursorY += 16;
            continue;
        }

        const u8* glyph = GetGlyph5x7(c);

        DrawGlyph5x7(cursorX, cursorY, glyph, color);

        cursorX += 12;
    }
}

static void ClearBottomBuffers(int color)
{
    // Clear draw buffer 1
    FillRectBottom(0, 0, 320, 240, color);
    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();

    // Clear draw buffer 2
    FillRectBottom(0, 0, 320, 240, color);
    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();
}

static void ShowRuntimeLoadingScreen()
{
    // Important: clear both buffers so the old GUI frame cannot come back.
    ClearBottomBuffers(0x000000);

    DrawTextBottom(50, 90, "RESHARP3DS", 0xFFFFFF);
    DrawTextBottom(65, 115, "LOADING...", 0x808080);

    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();

    DrawTextBottom(50, 90, "Loading...", 0xFFFFFF);

    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();
}

static void ShowConsoleModeScreen()
{
    ClearBottomBuffers(0x000000);

    DrawTextBottom(42, 90, "CONSOLE MODE", 0xFFFFFF);
    DrawTextBottom(30, 118, "TOP SCREEN OUTPUT", 0x808080);

    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();

    DrawTextBottom(42, 90, "CONSOLE MODE", 0xFFFFFF);
    DrawTextBottom(30, 118, "TOP SCREEN OUTPUT", 0x808080);

    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();
}

// ------------------------------------------------------------
// Graphics native calls
// ------------------------------------------------------------

static void MarkGraphicsUsed()
{
    g_graphicsUsed = true;
    g_consoleModeScreenShown = true;
}

static HRESULT Native_GraphicsClear(CLR_RT_StackFrame& stack)
{
    MarkGraphicsUsed();

    int color = stack.Arg0().NumericByRef().s4;

    FillRectBottom(0, 0, 320, 240, color);

    return S_OK;
}

static HRESULT Native_GraphicsDrawPixel(CLR_RT_StackFrame& stack)
{
    MarkGraphicsUsed();

    int x = stack.Arg0().NumericByRef().s4;
    int y = stack.Arg1().NumericByRef().s4;
    int color = stack.Arg2().NumericByRef().s4;

    PutPixelBottom(x, y, color);

    return S_OK;
}

static HRESULT Native_GraphicsFillRect(CLR_RT_StackFrame& stack)
{
    MarkGraphicsUsed();

    int x = stack.Arg0().NumericByRef().s4;
    int y = stack.Arg1().NumericByRef().s4;
    int width = stack.Arg2().NumericByRef().s4;
    int height = stack.Arg3().NumericByRef().s4;
    int color = stack.Arg4().NumericByRef().s4;

    FillRectBottom(x, y, width, height, color);

    return S_OK;
}

static HRESULT Native_GraphicsDrawRect(CLR_RT_StackFrame& stack)
{
    MarkGraphicsUsed();

    int x = stack.Arg0().NumericByRef().s4;
    int y = stack.Arg1().NumericByRef().s4;
    int width = stack.Arg2().NumericByRef().s4;
    int height = stack.Arg3().NumericByRef().s4;
    int color = stack.Arg4().NumericByRef().s4;

    DrawRectBottom(x, y, width, height, color);

    return S_OK;
}

static HRESULT Native_GraphicsDrawText(CLR_RT_StackFrame& stack)
{
    MarkGraphicsUsed();

    int x = stack.Arg0().NumericByRef().s4;
    int y = stack.Arg1().NumericByRef().s4;

    CLR_RT_HeapBlock& textArg = stack.Arg2();
    const char* text = textArg.RecoverString();

    int color = stack.Arg3().NumericByRef().s4;

    DrawTextBottom(x, y, text, color);

    return S_OK;
}

static HRESULT Native_GraphicsPresent(CLR_RT_StackFrame& stack)
{
    MarkGraphicsUsed();

    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();

    return S_OK;
}

// ------------------------------------------------------------
// Runtime API
// ------------------------------------------------------------

static HRESULT Native_Yield(CLR_RT_StackFrame& stack)
{
    gspWaitForVBlank();
    return S_OK;
}

// ------------------------------------------------------------
// Native table app.pe
// ------------------------------------------------------------

static void InstallAppNativeMethod(CLR_UINT32 index, CLR_RT_MethodHandler handler, const char* name)
{
    CLR_IDX entryMethod = g_CLR_RT_TypeSystem.m_entryPoint.Method();

    if ((CLR_IDX)index == entryMethod)
    {
        printf("[PATCH] ERROR: %s conflicts with Program.Main at method=%u\n",
            name,
            (unsigned)index);
        return;
    }

    g_appNativeMethods[index] = handler;

    printf("[PATCH] %s installed at method=%u\n",
        name,
        (unsigned)index);
}

static HRESULT Native_GraphicsProbe(CLR_RT_StackFrame& stack)
{
    unsigned int assmIdx = 0;
    unsigned int methodIdx = 0;

    CLR_RetrieveCurrentMethod(assmIdx, methodIdx);

    printf("[PROBE HIT] assm=%u method=%u\n", assmIdx, methodIdx);

    FillRectBottom(0, 0, 320, 240, 0xFF0000);

    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();

    return S_OK;
}

static void InitAppNativeTable()
{
    for (int i = 0; i < 1024; i++)
    {
        g_appNativeMethods[i] = NULL;
    }

    CLR_IDX entryMethod = g_CLR_RT_TypeSystem.m_entryPoint.Method();

    printf("[PATCH] app entryMethod=%u\n", (unsigned)entryMethod);

    // ------------------------------------------------------------
    // Console
    // Current SDK indexes:
    // 0 = Console.Clear native
    // 1 = Console.Write string native
    // 2 = Console.WriteLine string native
    // 3 = Console.Write int native
    // 4 = Console.WriteLine int native
    // ------------------------------------------------------------

    InstallAppNativeMethod(0, Native_Clear, "Console.Clear");
    InstallAppNativeMethod(1, Native_Write, "Console.Write");
    InstallAppNativeMethod(2, Native_WriteLine, "Console.WriteLine");
    InstallAppNativeMethod(3, Native_WriteInt, "Console.WriteInt");
    InstallAppNativeMethod(4, Native_WriteLineInt, "Console.WriteLineInt");

    // ------------------------------------------------------------
    // Graphics
    // Real indexes found with probe:
    // 5  = Graphics.Clear
    // 6  = Graphics.DrawPixel
    // 7  = Graphics.FillRect
    // 8  = Graphics.DrawRect
    // 9  = Graphics.DrawText
    // 10 = Graphics.Present
    // ------------------------------------------------------------

    InstallAppNativeMethod(5, Native_GraphicsClear, "Graphics.Clear");
    InstallAppNativeMethod(6, Native_GraphicsDrawPixel, "Graphics.DrawPixel");
    InstallAppNativeMethod(7, Native_GraphicsFillRect, "Graphics.FillRect");
    InstallAppNativeMethod(8, Native_GraphicsDrawRect, "Graphics.DrawRect");
    InstallAppNativeMethod(9, Native_GraphicsDrawText, "Graphics.DrawText");
    InstallAppNativeMethod(10, Native_GraphicsPresent, "Graphics.Present");

    // ------------------------------------------------------------
    // Runtime
    // ------------------------------------------------------------

    InstallAppNativeMethod(11, Native_Yield, "Runtime.Yield");

    g_appAssembly->m_nativeCode = g_appNativeMethods;

    printf("[PATCH] app native API table installed\n");
}

// ------------------------------------------------------------
// Load PE
// ------------------------------------------------------------

static CLR_RT_Assembly* LoadAssembly(const char* path)
{
    printf("[LOAD] %s\n", path);

    FILE* f = fopen(path, "rb");

    if (!f)
    {
        printf("[ERR] fopen %s\n", path);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (size <= 0)
    {
        printf("[ERR] invalid size %s\n", path);
        fclose(f);
        return NULL;
    }

    unsigned char* buf = (unsigned char*)memalign(32, size);

    if (!buf)
    {
        printf("[ERR] alloc %s\n", path);
        fclose(f);
        return NULL;
    }

    size_t read = fread(buf, 1, size, f);
    fclose(f);

    if (read != (size_t)size)
    {
        printf("[ERR] fread %s\n", path);
        return NULL;
    }

    const CLR_RECORD_ASSEMBLY* hdr = (const CLR_RECORD_ASSEMBLY*)buf;

    CLR_RT_Assembly* asmObj = NULL;

    HRESULT hr = CLR_RT_Assembly::CreateInstance(hdr, asmObj);

    if (!SUCCEEDED(hr))
    {
        printf("[ERR] CreateInstance %s = 0x%08X\n", path, (unsigned)hr);
        return NULL;
    }

    g_CLR_RT_TypeSystem.Link(asmObj);

    printf("[OK] linked %s\n", path);
    printf("[ASM] name=%s idx=%u methodCount=%d\n",
        asmObj->m_szName ? asmObj->m_szName : "NULL",
        (unsigned)asmObj->m_idx,
        asmObj->m_pTablesSize[TBL_MethodDef]);

    return asmObj;
}

// ------------------------------------------------------------
// Wait exit
// ------------------------------------------------------------

static void WaitExit()
{
    printf("\nSTART to exit\n");

    while (aptMainLoop())
    {
        hidScanInput();

        if (hidKeysDown() & KEY_START)
        {
            break;
        }

        gspWaitForVBlank();
    }
}

// ------------------------------------------------------------
// main
// ------------------------------------------------------------

int main()
{
    gfxInitDefault();

    bool start_runtime = false;

    while (aptMainLoop())
    {
        run_gui();

        hidScanInput();

        u32 kDown = hidKeysDown();

        if (kDown & KEY_A)
        {
            if (gui_can_launch())
            {
                start_runtime = true;
                break;
            }
        }

        if (kDown & KEY_START)
        {
            break;
        }

        gspWaitForVBlank();
    }

    gui_shutdown();

    if (!start_runtime)
    {
        gfxExit();
        return 0;
    }

    g_graphicsUsed = false;
    g_consoleModeScreenShown = false;

    ShowRuntimeLoadingScreen();

    consoleInit(GFX_TOP, NULL);
    consoleClear();

    printf("=== ReSharp3DS Runtime ===\n\n");

    CLR_SETTINGS settings;
    memset(&settings, 0, sizeof(settings));

    HRESULT hr = nanoCLR_Initialize(&settings);

    if (!SUCCEEDED(hr))
    {
        printf("[ERR INIT] 0x%08X\n", (unsigned)hr);
        WaitExit();
        nanoCLR_Cleanup();
        gfxExit();
        return 0;
    }

    g_mscorlibAssembly = LoadAssembly("sdmc:/ReSharp3DS/mscorlib.pe");

    if (!g_mscorlibAssembly)
    {
        printf("[FATAL] mscorlib load failed\n");
        WaitExit();
        nanoCLR_Cleanup();
        gfxExit();
        return 0;
    }

    g_appAssembly = LoadAssembly("sdmc:/ReSharp3DS/app.pe");

    if (!g_appAssembly)
    {
        printf("[FATAL] app load failed\n");
        WaitExit();
        nanoCLR_Cleanup();
        gfxExit();
        return 0;
    }

    printf("[DEBUG] ResolveAll...\n");
    hr = g_CLR_RT_TypeSystem.ResolveAll();
    printf("[ResolveAll] 0x%08X\n", (unsigned)hr);

    if (!SUCCEEDED(hr))
    {
        printf("[FATAL] ResolveAll failed\n");
        WaitExit();
        nanoCLR_Cleanup();
        gfxExit();
        return 0;
    }

    printf("[DEBUG] PrepareForExecution...\n");
    hr = g_CLR_RT_TypeSystem.PrepareForExecution();
    printf("[Prepare] 0x%08X\n", (unsigned)hr);

    printf("[DEBUG] entryPoint data=0x%08X method=%u assembly=%u\n",
        (unsigned)g_CLR_RT_TypeSystem.m_entryPoint.m_data,
        (unsigned)g_CLR_RT_TypeSystem.m_entryPoint.Method(),
        (unsigned)g_CLR_RT_TypeSystem.m_entryPoint.Assembly());

    if (!SUCCEEDED(hr))
    {
        printf("[FATAL] PrepareForExecution failed\n");
        WaitExit();
        nanoCLR_Cleanup();
        gfxExit();
        return 0;
    }

    InitMscorlibNativeTable();
    InitAppNativeTable();

    static wchar_t args[] = L"";

    printf("[CLR] Execute\n");

    while (aptMainLoop())
    {
        hr = g_CLR_RT_ExecutionEngine.Execute(args, 1000);

        if (!SUCCEEDED(hr))
        {
            printf("[CLR] ERROR 0x%08X\n", (unsigned)hr);
            break;
        }

        if (!g_graphicsUsed && !g_consoleModeScreenShown)
        {
            ShowConsoleModeScreen();
            g_consoleModeScreenShown = true;
        }
    }

    printf("[CLR] loop ended\n");

    WaitExit();

    nanoCLR_Cleanup();
    gfxExit();

    return 0;
}