#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>
#include <nanoCLR_Types.h>
#include <nanoCLR_Runtime.h>

#ifdef malloc
#undef malloc
#endif
#ifdef free
#undef free
#endif

extern "C" {
    void* platform_malloc(size_t size) {
        return malloc(size);
    }

    void platform_free(void* ptr) {
        if (ptr) free(ptr);
    }
}

int CLR_Debug::Printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    int result = vprintf(format, args);
    va_end(args);
    return result;
}

extern "C" {
    void platform_sleep(uint32_t milliseconds) {
        uint64_t nanoseconds = (uint64_t)milliseconds * 1000000;
        svcSleepThread(nanoseconds);
    }
    
    uint64_t platform_get_ticks() {
        return osGetTime();
    }

    void nanoCLR_Cleanup() {
    }

    void HAL_Assert(const char *Func, int Line, const char *File) {
        printf("\n\x1b[31m[!!! PANIC DU MOTEUR CLR !!!]\x1b[0m\n");
        printf("Fichier : %s\n", File);
        printf("Ligne   : %d\n", Line);
        printf("Fonction: %s\n", Func);
        printf("\nAppuyez sur START pour quitter.\n");
        
        while (aptMainLoop()) {
            hidScanInput();
            if (hidKeysDown() & KEY_START) break;
        }
        exit(0);
    }
    
    void HAL_AssertEx() {
        HAL_Assert("Unknown", 0, "Unknown");
    }
    
    uint64_t HAL_Time_CurrentTime() { 
        return osGetTime() * 1000; 
    }
    
    uint64_t HAL_Time_SysTicksToTime(uint64_t ticks) { 
        return ticks; 
    }
    
    void Watchdog_Reset() {}
    void RtosYield() {}
    void HAL_Continuations_Trigger() {}
}

HAL_DblLinkedList<void*> g_HAL_Completion_List;

extern "C" {
    void HAL_Continuations_Initialize() {
        void** listPtr = (void**)&g_HAL_Completion_List;
        listPtr[0] = (void*)&listPtr[1];
        listPtr[1] = (void*)&listPtr[0];
    }
}

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

HRESULT CLR_HW_Hardware::CreateInstance() { return S_OK; }
void CLR_HW_Hardware::PrepareForGC() {}
void CLR_HW_Hardware::ProcessActivity() {}

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
    va_list args;
    va_start(args, format);
    int ret = vsnprintf(buf, len, format, args);
    va_end(args);
    if (ret >= 0) {
        buf += ret;
        len -= ret;
        return true;
    }
    return false;
}

HRESULT Library_corlib_native_System_Exception::CreateInstance(CLR_RT_HeapBlock& ref, CLR_RT_TypeDef_Index const& cls, int arg, CLR_RT_StackFrame* frame) { return S_OK; }
HRESULT Library_corlib_native_System_Exception::CreateInstance(CLR_RT_HeapBlock& ref, int arg, CLR_RT_StackFrame* frame) { return S_OK; }
HRESULT Library_corlib_native_System_Exception::SetStackTrace(CLR_RT_HeapBlock& ref, CLR_RT_StackFrame* frame) { return S_OK; } 

HAL_SYSTEM_CONFIG HalSystemConfig;

static unsigned char* g_ClrHeapBuffer = NULL;

extern "C" {
    void HeapLocation_C(unsigned char **baseAddress, unsigned int *sizeInBytes) {
        if (g_ClrHeapBuffer == NULL) {
            g_ClrHeapBuffer = (unsigned char*)memalign(8, 8 * 1024 * 1024);        }
        *baseAddress = g_ClrHeapBuffer;
        *sizeInBytes = 4 * 1024 * 1024;
    }

    void nanoHAL_Initialize_C() {
    }
}

extern "C" {
    HRESULT nanoCLR_Initialize(const void* settings) {
        const CLR_RECORD_VERSION g_CLR_Version = { 1, 0, 0, 0 };
        if (g_ClrHeapBuffer == NULL) {
            g_ClrHeapBuffer = (unsigned char*)memalign(8, 8 * 1024 * 1024);
        }
        if (g_ClrHeapBuffer == NULL) return CLR_E_OUT_OF_MEMORY;

        printf("[DEBUG] RAM: %p\n", g_ClrHeapBuffer);

        HalSystemConfig.RAM1.Base = (unsigned int)g_ClrHeapBuffer;
        HalSystemConfig.RAM1.Size = 8 * 1024 * 1024;

        HAL_Continuations_Initialize();

        memset(&g_CLR_RT_ExecutionEngine, 0, sizeof(g_CLR_RT_ExecutionEngine));

        printf("[DEBUG] Appel CreateInstance...\n");
        HRESULT hr = CLR_RT_ExecutionEngine::CreateInstance();
        if (hr != S_OK) {
            printf("[ERR] CreateInstance a echoue avec code: 0x%08X\n", (unsigned int)hr);
            return hr;
        }
        printf("[DEBUG] CreateInstance OK.\n");

        printf("[DEBUG] Appel ExecutionEngine_Initialize...\n");
        hr = g_CLR_RT_ExecutionEngine.ExecutionEngine_Initialize();
        if (hr != S_OK) {
            printf("[ERR] ExecutionEngine_Initialize a echoue avec code: 0x%08X\n", (unsigned int)hr);
            return hr;
        }
        printf("[DEBUG] ExecutionEngine_Initialize OK.\n");
        
        return S_OK;
    }
}