#pragma once
#include "main.h"

struct PeripheralHandle {
    UART_HandleTypeDef* huart;
    DMA_HandleTypeDef* hdma;   // only for rx
};

PeripheralHandle* GetUart1Handle();
PeripheralHandle* GetUart2Handle();
PeripheralHandle* GetUart3Handle();