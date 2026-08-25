#pragma once

// Clock config + GPIO/DMA/RTC/USART bring-up. HAL_Init() sonrasi, RTOS baslamadan once bir kere cagrilir.
void HardwareInit_Run();

#if defined(RTOS_BACKEND_FREERTOS)
#include "main.h"

extern RTC_HandleTypeDef hrtc;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart2_rx;
#endif
