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


static HRESULT Native3DS_Print(CLR_RT_StackFrame& stack)
{
    unsigned int assmIdx = 0;
    unsigned int methodIdx = 0;

    CLR_RetrieveCurrentMethod(assmIdx, methodIdx);

    printf("[NATIVE] Native3DS.Print called\n");
    printf("[NATIVE] assm=%u method=%u\n", assmIdx, methodIdx);
    printf("Hello from C# to C++ on 3DS!\n");

    return S_OK;
}

static void InitNativeTable()
{
    for (int i = 0; i < 1024; i++)
    {
        g_appNativeMethods[i] = NULL;
    }

    CLR_IDX entryMethod = g_CLR_RT_TypeSystem.m_entryPoint.Method();

    printf("[PATCH] entryMethod=%u\n", (unsigned)entryMethod);

    for (int i = 0; i < 32; i++)
    {
        if ((CLR_IDX)i == entryMethod)
        {
            printf("[PATCH] skip Program.Main method=%d\n", i);
            continue;
        }

        g_appNativeMethods[i] = Native3DS_Print;
        printf("[PATCH] Native3DS_Print installed at method=%d\n", i);
    }
}

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

    printf("=== ReSharp3DS Native Print Test ===\n\n");

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

    InitNativeTable();

    g_appAssembly->m_nativeCode = g_appNativeMethods;

    printf("[PATCH] native table installed\n");
    printf("[CLR] Execute\n");

    static wchar_t args[] = L"";
    hr = g_CLR_RT_ExecutionEngine.Execute(args, 1000);

    printf("[CLR] returned 0x%08X\n", (unsigned)hr);

    WaitExit();

    nanoCLR_Cleanup();
    gfxExit();

    return 0;
}