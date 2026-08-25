#pragma once
#include "UartPeripheral.hpp"

#if defined(RTOS_BACKEND_ZEPHYR)
#include <zephyr/drivers/uart.h>
#endif

class UartBlockingSendTransport : public UartPeripheral {
    public:
        explicit UartBlockingSendTransport(UartPeripheral::HalHandle* huart)
            : UartPeripheral(huart) {
        }

        void send(const uint8_t* data, std::size_t len)
        {
#if defined(RTOS_BACKEND_FREERTOS)
            HAL_UART_Transmit(huart_->huart, data, static_cast<uint16_t>(len), HAL_MAX_DELAY);
#elif defined(RTOS_BACKEND_ZEPHYR)
            for (std::size_t i = 0; i < len; ++i)
            {
                uart_poll_out(huart_, data[i]);
            }
#endif
        }
};