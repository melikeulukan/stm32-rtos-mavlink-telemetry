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
void UartPeripheral::transmitDma(const uint8_t* data, std::size_t len)
{
    uart_tx(huart_, data, len, SYS_FOREVER_US);
}

// RX
void UartPeripheral::receiveDma(uint8_t* buffer, std::size_t len)
{
    // timeout (us): HAL'in "ReceiveToIdle" davranisina karsilik gelen bosluk suresi.
    // 10000 (10ms) baslangic degeri - protokole gore ayarlanabilir.
    uart_rx_enable(huart_, buffer, len, 0);
}

void UartPeripheral::stopDma()
{
    uart_tx_abort(huart_);
    uart_rx_disable(huart_);
}