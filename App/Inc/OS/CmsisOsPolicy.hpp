#pragma once

#include "cmsis_os2.h"
#include <cstdint>
#include <cstddef>
#include <array>

#if defined(RTOS_BACKEND_ZEPHYR)
#include <zephyr/kernel.h>
#elif defined(RTOS_BACKEND_FREERTOS)
#include "FreeRTOS.h"
#include "task.h"
#endif

struct CmsisOsPolicy {
    using ThreadHandle = osThreadId_t;
    using Priority = osPriority_t;

    template<std::size_t StackBytes>
    [[nodiscard]] static ThreadHandle CreateThread(const char* name, Priority prio,
        void(*entry)(void*), void* arg)
    {
            osThreadAttr_t attr = {};
            attr.name=name;
            attr.priority=prio;

#if defined(RTOS_BACKEND_ZEPHYR)
            static K_THREAD_STACK_DEFINE(stack, StackBytes);
            attr.stack_mem = stack;
            attr.stack_size = K_THREAD_STACK_SIZEOF(stack);
#elif defined(RTOS_BACKEND_FREERTOS)
            static StaticTask_t controlBlock;
            alignas(8) static std::array<uint8_t, StackBytes> stack;
            attr.cb_mem=&controlBlock;
            attr.cb_size= sizeof(controlBlock);
            attr.stack_mem=stack.data();
            attr.stack_size=StackBytes;
#endif
        return osThreadNew(entry, arg, &attr);

    }

    static void TerminateCurrentThread()
    {
        osThreadTerminate(osThreadGetId());
    }

    static void InitializeKernel()
    {
#if defined(RTOS_BACKEND_FREERTOS)
        osKernelInitialize();
#endif
    }

    static void StartKernel()
    {
#if defined(RTOS_BACKEND_FREERTOS)
        osKernelStart();
#endif
    }
};