#include "Peripherals/UartPeripheral.hpp"
#include <zephyr/drivers/uart.h>

static void zephyr_uart_callback(const struct device *dev, struct uart_event *evt, void *user_data)
{
    UartPeripheral* peripheral = static_cast<UartPeripheral*>(user_data);

    switch (evt->type) {
        case UART_TX_DONE:
            peripheral->onTxComplete();
            break;
        case UART_RX_RDY:
            peripheral->onRxEvent(evt->data.rx.len);
            break;
        case UART_RX_STOPPED:
            peripheral->onError(evt->data.rx_stop.reason);
            break;
        default:
            // UART_TX_ABORTED, UART_RX_BUF_REQUEST, UART_RX_BUF_RELEASED, UART_RX_DISABLED
            break;
    }
}

UartPeripheral::UartPeripheral(HalHandle* huart) : huart_(huart)
{
    uart_callback_set(huart_, zephyr_uart_callback, this);
}

// TX
void UartPeripheral::transmitDma(std::span<const uint8_t> data)
{
    uart_tx(huart_, data.data(), data.size(), SYS_FOREVER_US);
}

// RX
void UartPeripheral::receiveDma(std::span<uint8_t> buffer)
{
    // timeout (us): HAL'in "ReceiveToIdle" davranisina karsilik gelen bosluk suresi.
    // 0: idle algilanir algilanmaz aninda bildirir.
    uart_rx_enable(huart_, buffer.data(), buffer.size(), 0);
}

void UartPeripheral::stopDma()
{
    uart_tx_abort(huart_);
    uart_rx_disable(huart_);
}

void UartPeripheral::transmitBlocking(std::span<const uint8_t> data)
{
    for (auto byte : data)
    {
        uart_poll_out(huart_, byte);
    }
}