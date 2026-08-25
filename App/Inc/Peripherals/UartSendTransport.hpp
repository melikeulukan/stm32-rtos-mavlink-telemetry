#pragma once
#include "UartPeripheral.hpp"
#include "cmsis_os2.h"
#include <cstddef>

class UartSendTransport : public UartPeripheral {
    public:
        UartSendTransport(UartPeripheral::HalHandle* huart, osSemaphoreId_t txDoneSem)
            : UartPeripheral(huart), txDoneSem_(txDoneSem) {
        }

        void send(const uint8_t* data, std::size_t len)
        {
            transmitDma(data, len);
            osSemaphoreAcquire(txDoneSem_, osWaitForever);
        }

    private:
        void onTxComplete() override
        {
            osSemaphoreRelease(txDoneSem_);
        }

        osSemaphoreId_t txDoneSem_;
};
