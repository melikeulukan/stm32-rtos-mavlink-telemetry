#pragma once
#include "main.h"

struct PeripheralHandle {
    UART_HandleTypeDef* huart;
    DMA_HandleTypeDef* hdma;   // only for rx
};