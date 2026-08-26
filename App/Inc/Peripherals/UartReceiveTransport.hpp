#pragma once
#include "UartPeripheral.hpp"
#include "Os/ActiveOs.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <span>

template<std::size_t N>
using Buffer = std::array<std::uint8_t, N>;

class UartReceiveTransport : public UartPeripheral {
    public:
        UartReceiveTransport(UartPeripheral::HalHandle* huart, ActiveOs::QueueHandle rxQueue)
            : UartPeripheral(huart), rxQueue_(rxQueue) {
        }

        // Ilk dinlemeyi baslatir, rxQueue olusturulduktan sonra, bir kere cagrilir.
        void startListening()
        {
            receiveDma(dmaBuffer_);
        }

        // Bir sonraki paket gelene kadar bloklar.
        const uint8_t* receive(uint16_t& outLen)
        {
            ActiveOs::QueueGet(rxQueue_, &outLen, ActiveOs::WaitForever);
            return packet_.data();
        }


    private:
        void onRxEvent(uint16_t size) override
        {
            stopDma();

            if (size >= packet_.size())
            {
                size = static_cast<uint16_t>(packet_.size() - 1);
            }
            memcpy(packet_.data(), dmaBuffer_.data(), size);
            packet_[size] = '\0';

            ActiveOs::QueuePut(rxQueue_, &size);

            receiveDma(dmaBuffer_);
        }

        ActiveOs::QueueHandle rxQueue_;
        Buffer<128> dmaBuffer_{};
        Buffer<128> packet_{};
};
