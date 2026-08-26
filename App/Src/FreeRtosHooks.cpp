#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

#include <cstring>
#include <cstdint>

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
