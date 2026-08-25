#pragma once
#include "PeripheralHandle.hpp"
#include <cstddef>
#include <cstdint>
#include <array>
#include <span>

class UartPeripheral {
    public:
        using HalHandle = PeripheralHandle;

        explicit UartPeripheral(HalHandle* huart);
        virtual ~UartPeripheral() = default;

        UartPeripheral(const UartPeripheral&) = delete;
        UartPeripheral& operator=(const UartPeripheral&) = delete;  

        virtual void onTxComplete() {}
        virtual void onRxEvent(uint16_t size) {}
        virtual void onError(std::uint32_t errorCode) {}

        HalHandle* handle() const { return huart_; }

        static UartPeripheral* find(HalHandle* huart);

    protected:
        HalHandle* huart_;

        void transmitDma(std::span<const uint8_t> data);

        void receiveDma(std::span<uint8_t> buffer);

        void stopDma();

    private:
        static constexpr std::size_t MAX_UARTS = 3;
        static inline std::array<UartPeripheral*, MAX_UARTS> table_{};
};