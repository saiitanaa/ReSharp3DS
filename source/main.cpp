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

int main(int argc, char* argv[]) {
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);

    printf("========================================\n");
    printf("      RESHARP 3DS - PE LAUNCHER         \n");
    printf("========================================\n\n");

    CLR_SETTINGS clrSettings;
    memset(&clrSettings, 0, sizeof(clrSettings));
    
    printf("[CLR] Initialisation du moteur...\n");
    HRESULT hr = nanoCLR_Initialize(&clrSettings);
    if (!SUCCEEDED(hr)) {
        printf("[ERR] Init moteur: 0x%08X\n", (unsigned int)hr);
        goto wait_and_exit;
    }
    printf("[CLR] Moteur OK.\n");

    {
        const char* pe_path = "sdmc:/app.pe";
        FILE* file = fopen(pe_path, "rb");
        if (file == NULL) {
            pe_path = "app.pe";
            file = fopen(pe_path, "rb");
        }
        if (file == NULL) {
            printf("[ERR] 'app.pe' introuvable.\n");
            goto wait_and_exit;
        }

        fseek(file, 0, SEEK_END);
        long pe_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        printf("[SD] Fichier: %ld octets\n", pe_size);

        unsigned char* pe_buf = (unsigned char*)memalign(4, pe_size);
        if (pe_buf == NULL) {
            printf("[ERR] Allocation echouee.\n");
            fclose(file);
            goto wait_and_exit;
        }
        fread(pe_buf, 1, pe_size, file);
        fclose(file);

        const CLR_RECORD_ASSEMBLY* header = (const CLR_RECORD_ASSEMBLY*)pe_buf;
        printf("[CLR] Marker PE: 0x%08X\n", (unsigned int)header->marker);

        CLR_RT_Assembly* assembly = NULL;
        hr = CLR_RT_Assembly::CreateInstance(header, assembly);
        if (!SUCCEEDED(hr)) {
            printf("[ERR] CreateInstance: 0x%08X\n", (unsigned int)hr);
            free(pe_buf);
            goto wait_and_exit;
        }
        printf("[CLR] Assembly instanciee.\n");

        assembly->Resolve_TypeDef();
        assembly->Resolve_MethodDef();

        bool fOutput = true;
        if (!assembly->Resolve_AssemblyRef(fOutput)) {
            printf("[ERR] Resolve_AssemblyRef echouee\n");
            free(pe_buf);
            goto wait_and_exit;
        }

        hr = assembly->Resolve_TypeRef();
        if (!SUCCEEDED(hr)) {
            printf("[ERR] Resolve_TypeRef: 0x%08X\n", (unsigned int)hr);
            free(pe_buf);
            goto wait_and_exit;
        }

        hr = assembly->Resolve_MethodRef();
        if (!SUCCEEDED(hr)) {
            printf("[ERR] Resolve_MethodRef: 0x%08X\n", (unsigned int)hr);
            free(pe_buf);
            goto wait_and_exit;
        }

        hr = assembly->Resolve_ComputeHashes();
        if (!SUCCEEDED(hr)) {
            printf("[ERR] Resolve_ComputeHashes: 0x%08X\n", (unsigned int)hr);
            free(pe_buf);
            goto wait_and_exit;
        }
        printf("[CLR] Resolution OK.\n");

        g_CLR_RT_TypeSystem.Link(assembly);
        printf("[CLR] Link OK.\n");

        hr = g_CLR_RT_TypeSystem.ResolveAll();
        if (!SUCCEEDED(hr)) {
            printf("[ERR] ResolveAll: 0x%08X\n", (unsigned int)hr);
            free(pe_buf);
            goto wait_and_exit;
        }
        printf("[CLR] ResolveAll OK.\n");

        hr = g_CLR_RT_TypeSystem.PrepareForExecution();
        if (!SUCCEEDED(hr)) {
            printf("[ERR] PrepareForExecution: 0x%08X\n", (unsigned int)hr);
            free(pe_buf);
            goto wait_and_exit;
        }
        printf("[CLR] PrepareForExecution OK.\n");

        printf("[CLR] Lancement de l'ExecutionEngine...\n\n");
        
        HRESULT hr_exec = g_CLR_RT_ExecutionEngine.Execute(NULL, -1);
        
        if (SUCCEEDED(hr_exec)) {
            printf("[CLR] Execution terminée avec succès !\n");
        } else {
            printf("[ERR] Erreur lors de l'execution: 0x%08X\n", (unsigned int)hr_exec);
        }

        printf("\n[CLR] Execution terminee.\n");
        free(pe_buf);
    }

wait_and_exit:
    printf("\nSTART pour quitter.\n");
    while (aptMainLoop()) {
        hidScanInput();
        if (hidKeysDown() & KEY_START) break;
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    nanoCLR_Cleanup();
    gfxExit();
    return 0;
}