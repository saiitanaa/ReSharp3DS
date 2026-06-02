#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include <nanoCLR_Types.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Application.h>

#ifndef SUCCEEDED
#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#endif

extern "C" {
    HRESULT nanoCLR_Initialize(const void* settings);
    void nanoCLR_Cleanup();
}

static CLR_RT_Assembly* g_mscorlibAssembly = NULL;
static CLR_RT_Assembly* g_appAssembly = NULL;

static HRESULT Native_PrintProbe(CLR_RT_StackFrame& stack)
{
    unsigned int assmIdx = 0;
    unsigned int methodIdx = 0;

    CLR_RetrieveCurrentMethod(assmIdx, methodIdx);

    printf("[C# NATIVE] called! assm=%u method=%u\n", assmIdx, methodIdx);
    printf("Hello World depuis C# sur 3DS!\n");

    return S_OK;
}

static const CLR_RT_MethodHandler g_appNativeMethods[] =
{
    Native_PrintProbe, // index 0
    Native_PrintProbe, // index 1
    NULL,              // index 2 = Program.Main, surtout PAS native
    Native_PrintProbe, // index 3
    Native_PrintProbe, // index 4
    Native_PrintProbe, // index 5
    Native_PrintProbe, // index 6
    Native_PrintProbe, // index 7
    Native_PrintProbe, // index 8
    Native_PrintProbe, // index 9
    Native_PrintProbe  // index 10
};

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

int main()
{
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);

    printf("=== CLR NATIVE TEST ===\n\n");

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

    if (!SUCCEEDED(hr))
    {
        printf("[FATAL] PrepareForExecution failed\n");
        WaitExit();
        nanoCLR_Cleanup();
        gfxExit();
        return 0;
    }

    // Patch APRES PrepareForExecution.
    // Le runtime appelle assm->m_nativeCode[MethodIndex].
    g_appAssembly->m_nativeCode = g_appNativeMethods;

    printf("[PATCH] app.m_nativeCode AFTER prepare = %p\n",
        g_appAssembly->m_nativeCode);

    printf("[CLR] Before Execute\n");

    static wchar_t args[] = L"";
    hr = g_CLR_RT_ExecutionEngine.Execute(args, 1000);

    printf("[CLR] After Execute\n");
    printf("[RET] 0x%08X\n", (unsigned)hr);

    WaitExit();

    nanoCLR_Cleanup();
    gfxExit();

    return 0;
}