#pragma once
#include <cstdint>
#include <atomic>

#if defined(RTOS_BACKEND_FREERTOS)
#include "stm32f3xx_hal.h"
#elif defined(RTOS_BACKEND_ZEPHYR)
#include <zephyr/drivers/watchdog.h>
#include <zephyr/device.h>
#endif

class Watchdog {
    public:
        enum class TaskId : uint8_t { MavlinkTx = 0, MavlinkRx = 1, Count };

        static void Init()
        {
#if defined(RTOS_BACKEND_FREERTOS)
            Handle().Instance = IWDG;
            Handle().Init.Prescaler = IWDG_PRESCALER_32;
            Handle().Init.Reload = 3749; // ~3s @ LSI/32 (LSI ~40kHz)
            Handle().Init.Window = IWDG_WINDOW_DISABLE; // pencere ozelligi kapali, her an beslenebilir
            HAL_IWDG_Init(&Handle());
#elif defined(RTOS_BACKEND_ZEPHYR)
            Device() = DEVICE_DT_GET(DT_NODELABEL(iwdg));
            wdt_timeout_cfg cfg{};
            cfg.window.max = 3000; // ms
            cfg.flags = WDT_FLAG_RESET_SOC;
            ChannelId() = wdt_install_timeout(Device(), &cfg);
            wdt_setup(Device(), 0);
#endif
        }

        static void KickTask(TaskId id)
        {
            Mask().fetch_or(1u << static_cast<uint8_t>(id), std::memory_order_relaxed);
        }

        static void Poll()
        {
            if (Mask().exchange(0, std::memory_order_relaxed) == kAllTasksMask)
            {
                FeedHardware();
            }
        }

    private:
        static constexpr uint8_t kAllTasksMask =
            static_cast<uint8_t>((1u << static_cast<uint8_t>(TaskId::Count)) - 1);

        static std::atomic<uint8_t>& Mask()
        {
            static std::atomic<uint8_t> mask{0};
            return mask;
        }

        static void FeedHardware()
        {
#if defined(RTOS_BACKEND_FREERTOS)
            HAL_IWDG_Refresh(&Handle());
#elif defined(RTOS_BACKEND_ZEPHYR)
            wdt_feed(Device(), ChannelId());
#endif
        }

#if defined(RTOS_BACKEND_FREERTOS)
        static IWDG_HandleTypeDef& Handle()
        {
            static IWDG_HandleTypeDef hiwdg{};
            return hiwdg;
        }
#elif defined(RTOS_BACKEND_ZEPHYR)
        static const struct device*& Device()
        {
            static const struct device* dev = nullptr;
            return dev;
        }

        static int& ChannelId()
        {
            static int id = -1;
            return id;
        }
#endif
};
