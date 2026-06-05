#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <iostream>
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
// Native API app.pe
// ------------------------------------------------------------

static HRESULT Native_Clear(CLR_RT_StackFrame& stack)
{
    consoleClear();
    return S_OK;
}

static HRESULT Native_WriteInt(CLR_RT_StackFrame& stack)
{
    CLR_RT_HeapBlock& arg = stack.Arg0();

    int value = arg.NumericByRef().s4;

    printf("%d", value);

    return S_OK;
}

static HRESULT Native_WriteLineInt(CLR_RT_StackFrame& stack)
{
    CLR_RT_HeapBlock& arg = stack.Arg0();

    int value = arg.NumericByRef().s4;

    printf("%d\n", value);

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

static HRESULT Native_Yield(CLR_RT_StackFrame& stack)
{
    gspWaitForVBlank();
    return S_OK;
}

// ------------------------------------------------------------
// Graphics
// ------------------------------------------------------------



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

static void InitAppNativeTable()
{
    for (int i = 0; i < 1024; i++)
    {
        g_appNativeMethods[i] = NULL;
    }

    CLR_IDX entryMethod = g_CLR_RT_TypeSystem.m_entryPoint.Method();

    printf("[PATCH] app entryMethod=%u\n", (unsigned)entryMethod);

    //Console
    InstallAppNativeMethod(0, Native_Clear, "Native3DS.Clear");
    InstallAppNativeMethod(1, Native_Write, "Native3DS.Write");
    InstallAppNativeMethod(2, Native_WriteLine, "Native3DS.WriteLine");
    InstallAppNativeMethod(3, Native_WriteInt, "Native3DS.WriteInt");
    InstallAppNativeMethod(4, Native_WriteLineInt, "Native3DS.WriteLineInt");
    //Inputs
    InstallAppNativeMethod(5, Native_IsStartPressed, "Native3DS.IsStartPressed");
    InstallAppNativeMethod(6, Native_IsSelectPressed, "Native3DS.IsSelectPressed");
    InstallAppNativeMethod(7, Native_IsAPressed, "Native3DS.IsAPressed");
    InstallAppNativeMethod(8, Native_IsBPressed, "Native3DS.IsBPressed");
    InstallAppNativeMethod(9, Native_IsXPressed, "Native3DS.IsXPressed");
    InstallAppNativeMethod(10, Native_IsYPressed, "Native3DS.IsYPressed");
    InstallAppNativeMethod(11, Native_IsLPressed, "Native3DS.IsLPressed");
    InstallAppNativeMethod(12, Native_IsRPressed, "Native3DS.IsRPressed");
    InstallAppNativeMethod(13, Native_IsUpPressed, "Native3DS.IsUpPressed");
    InstallAppNativeMethod(14, Native_IsDownPressed, "Native3DS.IsDownPressed");
    InstallAppNativeMethod(15, Native_IsLeftPressed, "Native3DS.IsLeftPressed");
    InstallAppNativeMethod(16, Native_IsRightPressed, "Native3DS.IsRightPressed");

    InstallAppNativeMethod(17, Native_Yield, "Native3DS.Yield");

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
            break;

        gspWaitForVBlank();
    }
}

// ------------------------------------------------------------
// main
// ------------------------------------------------------------

int main()
{
    gfxInitDefault();

    run_gui();

    bool start_runtime = false;
    while (aptMainLoop())
    {
        hidScanInput();
        u32 kDown = hidKeysDown();

        if (kDown & KEY_A) {
            start_runtime = true;
            break;
        }

        if (kDown & KEY_START) {
            break;
        }

        run_gui();

        gspWaitForVBlank();
    }

    if (!start_runtime) {
        gfxExit();
        return 0;
    }

    consoleInit(GFX_BOTTOM, NULL);
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
        gspWaitForVBlank();
    }

    printf("[CLR] loop ended\n");

    WaitExit();

    nanoCLR_Cleanup();
    gfxExit();

    return 0;
}