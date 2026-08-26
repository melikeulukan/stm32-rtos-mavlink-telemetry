#pragma once
#include "UartPeripheral.hpp"
#include <span>

class UartBlockingSendTransport : public UartPeripheral {
    public:
        explicit UartBlockingSendTransport(UartPeripheral::HalHandle* huart)
            : UartPeripheral(huart) {
        }

        void send(std::span<const uint8_t> data)
        {
            transmitBlocking(data);
        }
};