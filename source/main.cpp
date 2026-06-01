#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include <nanoCLR_Types.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Application.h>

#ifdef malloc
#undef malloc
#endif
#ifdef free
#undef free
#endif

#ifndef SUCCEEDED
#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#endif

extern "C" {
    HRESULT nanoCLR_Initialize(const void* settings);
    void nanoCLR_Cleanup();
}

// Fonction utilitaire pour charger et lier une assembly
HRESULT LoadAndLinkAssembly(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("[ERR] Impossible d'ouvrir %s\n", filename);
        return CLR_E_FAIL;
    }

    fseek(file, 0, SEEK_END);
    long pe_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char* pe_buf = (unsigned char*)memalign(4, pe_size);
    if (pe_buf == NULL) {
        printf("[ERR] Allocation mémoire échouée pour %s\n", filename);
        fclose(file);
        return CLR_E_OUT_OF_MEMORY;
    }
    
    fread(pe_buf, 1, pe_size, file);
    fclose(file);

    const CLR_RECORD_ASSEMBLY* header = (const CLR_RECORD_ASSEMBLY*)pe_buf;
    printf("[SD] Chargé %s (%ld octets, Marker: 0x%08X)\n", filename, pe_size, (unsigned int)header->marker);

    CLR_RT_Assembly* assembly = NULL;
    HRESULT hr = CLR_RT_Assembly::CreateInstance(header, assembly);
    if (!SUCCEEDED(hr)) {
        printf("[ERR] CreateInstance %s: 0x%08X\n", filename, (unsigned int)hr);
        free(pe_buf);
        return hr;
    }

    g_CLR_RT_TypeSystem.Link(assembly);
    printf("[CLR] Lié %s\n", filename);
    return S_OK;
}

int main(int argc, char* argv[]) {
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);

    printf("=== RESHARP 3DS - PE LAUNCHER ===\n\n");

    CLR_SETTINGS clrSettings;
    memset(&clrSettings, 0, sizeof(clrSettings));
    
    HRESULT hr = nanoCLR_Initialize(&clrSettings);
    if (!SUCCEEDED(hr)) {
        printf("[ERR] Init moteur: 0x%08X\n", (unsigned int)hr);
        goto wait_and_exit;
    }

    // 1. Charger mscorlib.pe (Impératif)
    hr = LoadAndLinkAssembly("mscorlib.pe");
    if (!SUCCEEDED(hr)) goto wait_and_exit;

    // 2. Charger app.pe
    hr = LoadAndLinkAssembly("app.pe");
    if (!SUCCEEDED(hr)) goto wait_and_exit;

    printf("[CLR] Assemblies chargées, préparation...\n");

    // 3. Résolution système après chargement de TOUTES les assemblies
    hr = g_CLR_RT_TypeSystem.ResolveAll();
    if (!SUCCEEDED(hr)) {
        printf("[ERR] ResolveAll: 0x%08X\n", (unsigned int)hr);
        goto wait_and_exit;
    }

    hr = g_CLR_RT_TypeSystem.PrepareForExecution();
    if (!SUCCEEDED(hr)) {
        printf("[ERR] PrepareForExecution: 0x%08X\n", (unsigned int)hr);
        goto wait_and_exit;
    }

    printf("[CLR] Lancement execution...\n");
    g_CLR_RT_ExecutionEngine.Execute(NULL, -1);

wait_and_exit:
    printf("\nSTART pour quitter.\n");
    while (aptMainLoop()) {
        hidScanInput();
        if (hidKeysDown() & KEY_START) break;
        gfxFlushBuffers();
        gspWaitForVBlank();
    }
    nanoCLR_Cleanup();
    gfxExit();
    return 0;
}