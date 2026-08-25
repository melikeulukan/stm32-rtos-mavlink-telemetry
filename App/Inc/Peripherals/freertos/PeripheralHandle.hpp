#pragma once
#include "main.h"

struct PeripheralHandle {
    UART_HandleTypeDef* huart;
    DMA_HandleTypeDef* hdma;   // only for rx
};

[[nodiscard]] PeripheralHandle* GetUart1Handle();
[[nodiscard]] PeripheralHandle* GetUart2Handle();
[[nodiscard]] PeripheralHandle* GetUart3Handle();