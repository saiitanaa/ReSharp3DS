#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

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
    printf("[API] Clear called from C#\n");
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

static HRESULT Native_IsStartPressed(CLR_RT_StackFrame& stack)
{
    hidScanInput();

    bool pressed = (hidKeysHeld() & KEY_START) != 0;

    if (pressed)
    {
        printf("[API] START detected from C#\n");
    }

    stack.SetResult_I4(pressed ? 1 : 0);

    return S_OK;
}

static HRESULT Native_Yield(CLR_RT_StackFrame& stack)
{
    gfxFlushBuffers();
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

static void InitAppNativeTable()
{
    for (int i = 0; i < 1024; i++)
    {
        g_appNativeMethods[i] = NULL;
    }

    CLR_IDX entryMethod = g_CLR_RT_TypeSystem.m_entryPoint.Method();

    printf("[PATCH] app entryMethod=%u\n", (unsigned)entryMethod);

    InstallAppNativeMethod(0, Native_Clear, "Native3DS.Clear");
    InstallAppNativeMethod(1, Native_Write, "Native3DS.Write");
    InstallAppNativeMethod(2, Native_WriteLine, "Native3DS.WriteLine");
    InstallAppNativeMethod(3, Native_IsStartPressed, "Native3DS.IsStartPressed");
    InstallAppNativeMethod(4, Native_Yield, "Native3DS.Yield");

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

        gfxFlushBuffers();
        gspWaitForVBlank();
    }
}

// ------------------------------------------------------------
// main
// ------------------------------------------------------------

int main()
{
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);

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

    g_mscorlibAssembly = LoadAssembly("sdmc:/mscorlib.pe");
    if (!g_mscorlibAssembly)
    {
        printf("[FATAL] mscorlib load failed\n");
        WaitExit();
        nanoCLR_Cleanup();
        gfxExit();
        return 0;
    }

    g_appAssembly = LoadAssembly("sdmc:/app.pe");
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

    printf("[CLR] Execute loop\n");

    while (aptMainLoop())
    {
        hr = g_CLR_RT_ExecutionEngine.Execute(args, 1000);

        if (!SUCCEEDED(hr))
        {
            printf("[CLR] ERROR 0x%08X\n", (unsigned)hr);
            break;
        }

        hidScanInput();

        // SELECT quitte le host C++.
        // START reste disponible pour le programme C#.
        if (hidKeysDown() & KEY_SELECT)
        {
            printf("[HOST] SELECT pressed, exiting host\n");
            break;
        }

        gfxFlushBuffers();
        gspWaitForVBlank();
    }

    printf("[CLR] loop ended\n");

    WaitExit();

    nanoCLR_Cleanup();
    gfxExit();

    return 0;
}