#include "Peripherals/UartPeripheral.hpp"
#include "PeripheralHandle.hpp"

UartPeripheral::UartPeripheral(HalHandle* huart) : huart_(huart)
{
    for(auto& entry : table_)
    {
        if(entry == nullptr)
        {
            entry = this;
            break;
        }
    }
}

UartPeripheral* UartPeripheral::find(HalHandle* huart)
{
    for(auto* entry : table_)
    {
        if(entry != nullptr && entry->handle() == huart)
        {
            return entry;
        }
    }
    return nullptr;
}

namespace {
UartPeripheral* findByRawHandle(UART_HandleTypeDef* rawHuart)
{
    PeripheralHandle* candidates[] = { GetUart1Handle(), GetUart2Handle(), GetUart3Handle() };
    for (auto* candidate : candidates)
    {
        if (candidate->huart == rawHuart)
        {
            return UartPeripheral::find(candidate);
        }
    }
    return nullptr;
}
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (UartPeripheral* peripheral = findByRawHandle(huart))
    {
        peripheral->onRxEvent(Size);
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (UartPeripheral* peripheral = findByRawHandle(huart))
    {
        peripheral->onTxComplete();
    }
}

void UartPeripheral::transmitDma(const uint8_t* data, std::size_t len)
{
    HAL_UART_Transmit_DMA(huart_->huart, data, static_cast<uint16_t>(len));
}

void UartPeripheral::receiveDma(uint8_t* buffer, std::size_t len)
{
    HAL_UARTEx_ReceiveToIdle_DMA(huart_->huart, buffer, len);
    if (huart_->hdma) __HAL_DMA_DISABLE_IT(huart_->hdma, DMA_IT_HT);
}

void UartPeripheral::stopDma()
{
    HAL_UART_DMAStop(huart_->huart);
}