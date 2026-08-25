#include "cmsis_os2.h"

#include "PeripheralHandle.hpp"
#include "HardwareInit.hpp"
#include "Peripherals/UartPeripheral.hpp"
#include "Peripherals/UartSendTransport.hpp"
#include "Peripherals/UartReceiveTransport.hpp"
#include "Peripherals/UartBlockingSendTransport.hpp"
#include "Tasks/MavlinkTxTask.hpp"
#include "Tasks/MavlinkRxTask.hpp"

#include <string.h>
#include <stdio.h>

int main(void)
{
  printf("boot ok\r\n");
  HardwareInit_Run();
  ActiveOs::InitializeKernel();

  auto txDoneSemHandle = osSemaphoreNew(1, 0, nullptr);
  printf("sem: %p\r\n", (void*)txDoneSemHandle);

  auto rxQueueHandle = osMessageQueueNew(8, sizeof(uint16_t), nullptr);
  printf("msgq: %p\r\n", (void*)rxQueueHandle);

  static UartSendTransport uart1(GetUart1Handle(), txDoneSemHandle);
  static UartReceiveTransport uart2(GetUart2Handle(), rxQueueHandle);
  static UartBlockingSendTransport debugUart(GetUart3Handle());
  printf("transports ok\r\n");

  uart2.startListening();
  printf("listening ok\r\n");

  static MavlinkTxTask<UartSendTransport> mavlinkTxTask(uart1);
  static MavlinkRxTask<UartReceiveTransport, UartBlockingSendTransport> mavlinkRxTask(uart2, debugUart);

  mavlinkTxTask.start();
  printf("tx task started\r\n");
  mavlinkRxTask.start();
  printf("rx task started\r\n");

  ActiveOs::StartKernel();
  osDelay(osWaitForever);
}

#if defined(RTOS_BACKEND_FREERTOS)

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
  if (strcmp(pcTaskName, "MavlinkTxTask") == 0)
  {
    HAL_GPIO_WritePin(GPIOB, LD1_Pin, GPIO_PIN_SET);   // yeşil = TxTask taştı
  }
  else if (strcmp(pcTaskName, "RxTask") == 0 || strcmp(pcTaskName, "MavlinkRxTask") == 0)
  {
    HAL_GPIO_WritePin(GPIOB, LD2_Pin, GPIO_PIN_SET);   // mavi = RxTask taştı
  }
  else
  {
    HAL_GPIO_WritePin(GPIOB, LD3_Pin, GPIO_PIN_SET);   // kırmızı = defaultTask ya da başka
  }
  while(1) { }
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM6)
  {
    HAL_IncTick();
  }
}

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

#endif // RTOS_BACKEND_FREERTOS
