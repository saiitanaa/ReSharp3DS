#ifndef TARGET_PLATFORM_H
#define TARGET_PLATFORM_H

#include <cstdint>   // Inclut les types uint32_t, uint64_t, etc.
#include <string.h>  // Pour utiliser le strlen standard

#define NANOCLR_LIGHTWEIGHT          1
#define PLATFORM_ARM                 1

#define HAL_USE_RTC                  0
#define HAL_USE_SDC                  0
#define HAL_USE_BDC                  0

#define VERSION_REVISION             0

#ifndef _MSC_VER
#define __int64 long long
#endif

typedef uint32_t ByteAddress;
typedef int BlockStorageStream;
typedef int HAL_COMPLETION;
typedef int BlockStorageDevice;

#ifdef __cplusplus
extern "C" {
#endif

uint64_t platform_get_ticks();
inline bool DebuggerPort_Initialize(int port) { return true; }

void HeapLocation(unsigned char*& base, unsigned int& size);
inline void NANOCLR_STOP() { while(1) {} }

inline uint32_t Events_MaskedRead(uint32_t eventMask) {
    return 0; 
}

inline uint32_t Events_WaitForEvents(uint32_t powerLevel, uint32_t eventMask, int64_t timeoutTicks) {
    return 0;
}

inline void Events_SetBoolTimer(bool* pBool, uint32_t milliseconds) {
}

#ifdef __cplusplus
}
#endif

inline uint64_t HAL_Time_CurrentSysTicks() {
    return platform_get_ticks();
}

inline size_t hal_strlen_s(const char* str) {
    if (str == nullptr) return 0;
    return strlen(str);
}

inline void hal_strcpy_s(char* dest, size_t destSize, const char* src) {
    if (dest == nullptr || destSize == 0) return;
    if (src == nullptr) {
        dest[0] = '\0';
        return;
    }
    strncpy(dest, src, destSize - 1);
    dest[destSize - 1] = '\0';
}

inline void hal_strncpy_s(char* dest, size_t destSize, const char* src, size_t count) {
    if (dest == nullptr || destSize == 0) return;
    if (src == nullptr || count == 0) {
        dest[0] = '\0';
        return;
    }
    size_t copyLen = (count < destSize - 1) ? count : destSize - 1;
    strncpy(dest, src, copyLen);
    dest[copyLen] = '\0';
}

#endif // TARGET_PLATFORM_H