#define NANOCLR_NO_ASSERT
#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#pragma pack(push, 1)
#include <nanoCLR_Types.h>
#include <nanoCLR_Runtime.h>
#pragma pack(pop)

#undef malloc
#undef free

// --- 1. Définitions Platform ---
extern "C" {
    void* platform_malloc(size_t size) { return malloc(size); }
    void platform_free(void* ptr) { if (ptr) free(ptr); }

    void platform_sleep(uint32_t milliseconds) {
        svcSleepThread((uint64_t)milliseconds * 1000000);
    }
    
    uint64_t platform_get_ticks() { return osGetTime(); }
    void nanoCLR_Cleanup() {}

    void HAL_Assert(const char *Func, int Line, const char *File) {
        printf("\n\x1b[31m[!!! PANIC DU MOTEUR CLR !!!]\x1b[0m\n");
        printf("Fichier : %s\nLigne   : %d\nFonction: %s\n", File, Line, Func);
        while (aptMainLoop()) { hidScanInput(); if (hidKeysDown() & KEY_START) break; }
        exit(0);
    }
    
    void HAL_AssertEx() { HAL_Assert("Unknown", 0, "Unknown"); }
    uint64_t HAL_Time_CurrentTime() { return osGetTime() * 1000; }
    uint64_t HAL_Time_SysTicksToTime(uint64_t ticks) { return ticks; }
    void Watchdog_Reset() {}
    void RtosYield() {}
    void HAL_Continuations_Trigger() {}
}

// --- 2. Debug Printf ---
int CLR_Debug::Printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    int result = vprintf(format, args);
    va_end(args);
    return result;
}

// --- 3. Gestion HAL ---
HAL_DblLinkedList<void*> g_HAL_Completion_List;
HAL_SYSTEM_CONFIG HalSystemConfig;
static unsigned char g_ClrHeapBuffer[4 * 1024 * 1024] __attribute__((aligned(32)));

extern "C" {
    void HAL_Continuations_Initialize() {
        void** listPtr = (void**)&g_HAL_Completion_List;
        listPtr[0] = (void*)&listPtr[1];
        listPtr[1] = (void*)&listPtr[0];
    }
}

// --- 4. Stubs et Handlers ---
// Déclarations uniquement (pas d'implémentation ici pour éviter la double définition)
struct CLR_HW_Hardware {
    static HRESULT CreateInstance();
    static void PrepareForGC();
    static void ProcessActivity();
};

struct Library_corlib_native_System_Exception {
    static HRESULT CreateInstance(CLR_RT_HeapBlock& ref, CLR_RT_TypeDef_Index const& cls, int arg, CLR_RT_StackFrame* frame);
    static HRESULT CreateInstance(CLR_RT_HeapBlock& ref, int arg, CLR_RT_StackFrame* frame);
    static HRESULT SetStackTrace(CLR_RT_HeapBlock& ref, CLR_RT_StackFrame* frame);
};

HRESULT CLR_Messaging::CreateInstance() { return S_OK; }
HRESULT CLR_DBG_Debugger::CreateInstance() { return S_OK; }

void CLR_RT_HeapBlock_EndPoint::HandlerMethod_Initialize() {}
void CLR_RT_HeapBlock_NativeEventDispatcher::HandlerMethod_Initialize() {}
void CLR_RT_HeapBlock_EndPoint::HandlerMethod_RecoverFromGC() {}
void CLR_RT_HeapBlock_NativeEventDispatcher::HandlerMethod_RecoverFromGC() {}
void CLR_RT_HeapBlock_WeakReference::Relocate() {}
void CLR_RT_HeapBlock_WeakReference::RecoverObjects(CLR_RT_DblLinkedList& list) {}

namespace CLR_RT_DUMP {
    void METHOD(const CLR_RT_MethodDef_Index& method) {}
    void POST_PROCESS_EXCEPTION(CLR_RT_HeapBlock& exception) {}
}

bool CLR_SafeSprintf(char*& buf, size_t& len, const char* format, ...) {
    va_list args; va_start(args, format);
    int ret = vsnprintf(buf, len, format, args);
    va_end(args);
    if (ret >= 0) { buf += ret; len -= ret; return true; }
    return false;
}

// --- 5. Initialisation Moteur ---
extern "C" {
    void HeapLocation_C(unsigned char **baseAddress, unsigned int *sizeInBytes) {
        *baseAddress = g_ClrHeapBuffer;
        *sizeInBytes = sizeof(g_ClrHeapBuffer);
    }

    HRESULT nanoCLR_Initialize(const void* settings) {
        printf("[DEBUG] RAM Base: %p\n", (void*)g_ClrHeapBuffer);
        HalSystemConfig.RAM1.Base = (unsigned int)g_ClrHeapBuffer;
        HalSystemConfig.RAM1.Size = sizeof(g_ClrHeapBuffer);
        HAL_Continuations_Initialize();
        
        HRESULT hr = CLR_RT_ExecutionEngine::CreateInstance();
        if (hr != S_OK) return hr;
        return g_CLR_RT_ExecutionEngine.ExecutionEngine_Initialize();
    }
}

// --- 6. Implémentations (Séparées des structures pour éviter les redéfinitions) ---
HRESULT CLR_HW_Hardware::CreateInstance() { return S_OK; }
void CLR_HW_Hardware::PrepareForGC() {}
void CLR_HW_Hardware::ProcessActivity() {}

HRESULT Library_corlib_native_System_Exception::CreateInstance(CLR_RT_HeapBlock& ref, CLR_RT_TypeDef_Index const& cls, int arg, CLR_RT_StackFrame* frame) { return S_OK; }
HRESULT Library_corlib_native_System_Exception::CreateInstance(CLR_RT_HeapBlock& ref, int arg, CLR_RT_StackFrame* frame) { return S_OK; }
HRESULT Library_corlib_native_System_Exception::SetStackTrace(CLR_RT_HeapBlock& ref, CLR_RT_StackFrame* frame) { return S_OK; }