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

// configSUPPORT_STATIC_ALLOCATION=1 olduğu icin FreeRTOS-Kernel, kendi Idle ve
// Timer task'lari icin statik bellegi uygulamadan istiyor (eski ST wrapper'i
// bunlari kendi icinde sagliyordu, yeni ARM upstream wrapper'i saglamiyor).
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleTaskStackBuffer[configMINIMAL_STACK_SIZE];

extern "C" void vApplicationGetIdleTaskMemory(StaticTask_t** ppxIdleTaskTCBBuffer,
    StackType_t** ppxIdleTaskStackBuffer, uint32_t* pulIdleTaskStackSize)
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = xIdleTaskStackBuffer;
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

static StaticTask_t xTimerTaskTCBBuffer;
static StackType_t xTimerTaskStackBuffer[configTIMER_TASK_STACK_DEPTH];

extern "C" void vApplicationGetTimerTaskMemory(StaticTask_t** ppxTimerTaskTCBBuffer,
    StackType_t** ppxTimerTaskStackBuffer, uint32_t* pulTimerTaskStackSize)
{
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCBBuffer;
  *ppxTimerTaskStackBuffer = xTimerTaskStackBuffer;
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
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
