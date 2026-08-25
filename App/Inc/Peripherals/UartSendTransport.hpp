#pragma once
#include "UartPeripheral.hpp"
#include "cmsis_os2.h"
#include <cstddef>
#include <span>

class UartSendTransport : public UartPeripheral {
    public:
        UartSendTransport(UartPeripheral::HalHandle* huart, osSemaphoreId_t txDoneSem)
            : UartPeripheral(huart), txDoneSem_(txDoneSem) {
        }

        void send(std::span<const uint8_t> data)
        {
            transmitDma(data);
            osSemaphoreAcquire(txDoneSem_, osWaitForever);
        }

    private:
        void onTxComplete() override
        {
            osSemaphoreRelease(txDoneSem_);
        }

        osSemaphoreId_t txDoneSem_;
};
