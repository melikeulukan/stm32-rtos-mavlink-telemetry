#include "PeripheralHandle.hpp"
#include "HardwareInit.hpp"

namespace {
PeripheralHandle uart1Handle{&huart1, &hdma_usart1_tx};
PeripheralHandle uart2Handle{&huart2, &hdma_usart2_rx};
PeripheralHandle uart3Handle{&huart3, nullptr};
}

PeripheralHandle* GetUart1Handle() { return &uart1Handle; }
PeripheralHandle* GetUart2Handle() { return &uart2Handle; }
PeripheralHandle* GetUart3Handle() { return &uart3Handle; }
