#pragma once
#include "UartPeripheral.hpp"
#include <span>

#if defined(RTOS_BACKEND_ZEPHYR)
#include <zephyr/drivers/uart.h>
#endif

class UartBlockingSendTransport : public UartPeripheral {
    public:
        explicit UartBlockingSendTransport(UartPeripheral::HalHandle* huart)
            : UartPeripheral(huart) {
        }

        void send(std::span<const uint8_t> data)
        {
#if defined(RTOS_BACKEND_FREERTOS)
            HAL_UART_Transmit(huart_->huart, data.data(), static_cast<uint16_t>(data.size()), HAL_MAX_DELAY);
#elif defined(RTOS_BACKEND_ZEPHYR)
            for (auto byte : data)
            {
                uart_poll_out(huart_, byte);
            }
#endif
        }
};