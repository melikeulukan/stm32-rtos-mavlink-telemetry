#pragma once
#include "UartPeripheral.hpp"
#include "Os/ActiveOs.hpp"
#include <cstddef>
#include <span>

class UartSendTransport : public UartPeripheral {
    public:
        UartSendTransport(UartPeripheral::HalHandle* huart, ActiveOs::SemaphoreHandle txDoneSem)
            : UartPeripheral(huart), txDoneSem_(txDoneSem) {
        }

        void send(std::span<const uint8_t> data)
        {
            transmitDma(data);
            ActiveOs::AcquireSemaphore(txDoneSem_, ActiveOs::WaitForever);
        }

    private:
        void onTxComplete() override
        {
            ActiveOs::ReleaseSemaphore(txDoneSem_);
        }

        ActiveOs::SemaphoreHandle txDoneSem_;
};
