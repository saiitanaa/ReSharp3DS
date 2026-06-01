//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef NANOHAL_V2_H
#define NANOHAL_V2_H

#ifndef VIRTUAL_DEVICE

// need to include stdlib.h **BEFORE** redefining malloc/free/realloc otherwise bad things happen
#include <stdlib.h>

// defines to prevent use of malloc, free and realloc
// the platform implementations: platform_malloc(), platform_free() should be used instead.
// realloc should never be used.
// as these the preferred calls to use as they ensure thread safety and RTOS integration.
#define malloc YOU_SHALL_NOT_USE_malloc
#define free   YOU_SHALL_NOT_USE_free

#endif

#include <nanoCLR_Headers.h>

#include <nanoHAL_Capabilites.h>
#include <nanoHAL_Network.h>
#include <nanoHAL_Power.h>
#include <nanoHAL_ConfigurationManager.h>
#include "nanoHAL_Boot.h"
#include <nanoVersion.h>

#if defined(VIRTUAL_DEVICE)
#include <crtdbg.h>
#endif

typedef uint32_t GPIO_PIN;

typedef enum SYSTEM_STATE
{
    SYSTEM_STATE_ISR,
    SYSTEM_STATE_NO_CONTINUATIONS,
    SYSTEM_STATE_TOTAL_STATES
} SYSTEM_STATE_type;

//////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Microsoft.SPOT.Hardware.SleepLevel !!! //
//////////////////////////////////////////////////////////////////
typedef enum SLEEP_LEVEL
{
    SLEEP_LEVEL__AWAKE = 0x00,
    SLEEP_LEVEL__SELECTIVE_OFF = 0x10,
    SLEEP_LEVEL__SLEEP = 0x20,
    SLEEP_LEVEL__DEEP_SLEEP = 0x30,
    SLEEP_LEVEL__OFF = 0x40,
} SLEEP_LEVEL_type;

// These events match emulator events in Framework\Tools\Emulator\Events.cs

#define SYSTEM_EVENT_FLAG_COM_IN       0x00000001
#define SYSTEM_EVENT_FLAG_COM_OUT      0x00000002
#define SYSTEM_EVENT_FLAG_STORAGE_IO   0x00000004
#define SYSTEM_EVENT_FLAG_SYSTEM_TIMER 0x00000010
#define SYSTEM_EVENT_FLAG_USB_IN       0x00000020
#define SYSTEM_EVENT_FLAG_USB_OUT      0x00000040
#define SYSTEM_EVENT_FLAG_GENERIC_PORT 0x00000100
#define SYSTEM_EVENT_FLAG_NETWORK      0x00000800
#define SYSTEM_EVENT_FLAG_SOCKET         0x00004000
#define SYSTEM_EVENT_FLAG_ONEWIRE_MASTER 0x00008000
#define SYSTEM_EVENT_FLAG_RADIO          0x00010000
#define SYSTEM_EVENT_FLAG_BLUETOOTH      0x00020000

#define SYSTEM_EVENT_FLAG_WIFI_STATION       0x01000000
#define SYSTEM_EVENT_FLAG_SPI_MASTER         0x02000000
#define SYSTEM_EVENT_FLAG_I2C_MASTER         0x04000000
#define SYSTEM_EVENT_HW_INTERRUPT            0x08000000
#define SYSTEM_EVENT_FLAG_DEBUGGER_ACTIVITY  0x20000000
#define SYSTEM_EVENT_FLAG_MESSAGING_ACTIVITY 0x40000000
#define SYSTEM_EVENT_FLAG_ALL 0xFFFFFFFF

////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH nanoFramework.Runtime.Events.EventCategory (in managed code) !!! //
////////////////////////////////////////////////////////////////////////////////////////////

#define EVENT_UNKNOWN               0
#define EVENT_CUSTOM                10
#define EVENT_GPIO                  20
#define EVENT_SERIAL                30
#define EVENT_NETWORK               40
#define EVENT_WIFI                  50
#define EVENT_CAN                   60
#define EVENT_STORAGE               70
#define EVENT_RADIO                 80
#define EVENT_HIGH_RESOLUTION_TIMER 90
#define EVENT_BLUETOOTH             100
#define EVENT_USB                   110
#define EVENT_TOUCH                 120
#define EVENT_GESTURE               130

#define PAL_EVENT_TOUCH 0x1
#define PAL_EVENT_KEY   0x2
#define PAL_EVENT_MOUSE 0x4

typedef int COM_HANDLE;

typedef struct HAL_DRIVER_CONFIG_HEADER
{
    unsigned int Enable;
} HAL_DRIVER_CONFIG_HEADER;

typedef struct HAL_SYSTEM_MEMORY_CONFIG
{
    unsigned int Base;
    unsigned int Size;
} HAL_SYSTEM_MEMORY_CONFIG;

typedef struct HAL_SYSTEM_CONFIG
{
    HAL_DRIVER_CONFIG_HEADER Header;
    COM_HANDLE DebuggerPort;
    COM_HANDLE DebugTextPort;
    unsigned int USART_DefaultBaudRate;
    COM_HANDLE stdio;
    HAL_SYSTEM_MEMORY_CONFIG RAM1;
    HAL_SYSTEM_MEMORY_CONFIG FLASH1;
} HAL_SYSTEM_CONFIG;

extern HAL_SYSTEM_CONFIG HalSystemConfig;

#ifdef __cplusplus
extern "C"
{
#endif

    void nanoHAL_Initialize_C();
    void nanoHAL_Uninitialize_C(bool isPoweringDown);
    void HeapLocation_C(unsigned char **baseAddress, unsigned int *sizeInBytes);
    void Target_ExternalMemoryInit();

    void CPU_Reset();
    void CPU_Sleep(SLEEP_LEVEL_type level, uint64_t wakeEvents);
    void CPU_SetPowerMode(PowerLevel_type powerLevel);
    void CPU_SetPowerModePlatform(PowerLevel_type powerLevel);
    void CPU_SetPowerModeTarget(PowerLevel_type powerLevel);
    bool DebuggerIsConnected();

    void *platform_malloc(size_t size);
    void platform_free(void *ptr);

    void SystemState_SetNoLock(SYSTEM_STATE_type state);
    void SystemState_ClearNoLock(SYSTEM_STATE_type state);
    bool SystemState_QueryNoLock(SYSTEM_STATE_type state);

    void HAL_Assert(const char *Func, int Line, const char *File);
    void HAL_AssertEx(); 

#ifdef __cplusplus
}
#endif

void SystemState_Set(SYSTEM_STATE_type newState);
void SystemState_Clear(SYSTEM_STATE_type state);
bool SystemState_Query(SYSTEM_STATE_type state);

#define NATIVE_INTERRUPT_START                                                                                          \
    SystemState_SetNoLock(SYSTEM_STATE_ISR);                                                                            \
    SystemState_SetNoLock(SYSTEM_STATE_NO_CONTINUATIONS);
#define NATIVE_INTERRUPT_END                                                                                            \
    SystemState_ClearNoLock(SYSTEM_STATE_NO_CONTINUATIONS);                                                             \
    SystemState_ClearNoLock(SYSTEM_STATE_ISR);

#define HAL_COMPLETION_IDLE_VALUE 0x0000FFFFFFFFFFFFull

#if defined(VIRTUAL_DEVICE)
#define OS_DELAY(milliSecs) ;
#else
#define OS_DELAY(milliSecs) PLATFORM_DELAY(milliSecs)
#endif

// Neutralisation complète des macros d'assertion défectueuses pour la 3DS
#define ASSERT(expr)        ((void)0)
#define _ASSERTE(expr)      ((void)0)
#define _SIDE_ASSERTE(expr) (void)(expr)

#ifdef STATIC_ASSERT_SUPPORTED
#define CT_ASSERT_STRING(x) #x
#define CT_ASSERT_UNIQUE_NAME(e, name)                                                                                  \
    static_assert((e), CT_ASSERT_STRING(name) "@" __FILE__ CT_ASSERT_STRING(__LINE__));
#define CT_ASSERT(e) static_assert((e), __FILE__ CT_ASSERT_STRING(__LINE__));
#else
#define CT_ASSERT_UNIQUE_NAME(e, name) typedef char __CT_ASSERT__##name[(e) ? 1 : -1];
#define CT_ASSERT(e)                   CT_ASSERT_UNIQUE_NAME(e, nanoclr)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#if !defined(BUILD_RTM)
    void debug_printf(const char *format, ...);
#else
__inline void debug_printf(const char *format, ...)
{
    (void)format;
}
#endif

#ifdef __cplusplus
}
#endif

#define NANOCLR_LOCKED_ACCESS_DECLARATION(type, name) type copy##name
#define NANOCLR_LOCKED_ACCESS_EXECUTE(name, expr)                                                                       \
    {                                                                                                                   \
        GLOBAL_LOCK();                                                                                                  \
        copy##name = expr;                                                                                              \
        GLOBAL_UNLOCK();                                                                                                \
    }
#define NANOCLR_LOCKED_ACCESS_GET(name) copy##name

#ifdef __cplusplus
extern "C"
{
#endif
    void HARD_Breakpoint();
#ifdef __cplusplus
}
#endif

#include <nanoHAL_Watchdog.h>
#include <nanoHAL_Rtos.h>
#include <nanoHAL_Windows_Storage.h>

#endif // NANOHAL_V2_H