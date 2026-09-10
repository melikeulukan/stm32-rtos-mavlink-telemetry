#include "PeripheralHandle.hpp"
#include "HardwareInit.hpp"
#include "Peripherals/UartPeripheral.hpp"
#include "Peripherals/UartSendTransport.hpp"
#include "Peripherals/UartReceiveTransport.hpp"
#include "Peripherals/UartBlockingSendTransport.hpp"
#include "Tasks/MavlinkTxTask.hpp"
#include "Tasks/MavlinkRxTask.hpp"
#include "Tasks/WatchdogTask.hpp"
#include "Watchdog/Watchdog.hpp"

#include <stdio.h>

int main(void)
{
  printf("boot ok\r\n");
  HardwareInit_Run();

  ActiveOs::InitializeKernel();

  auto txDoneSemHandle = ActiveOs::CreateSemaphore(1, 0);
  printf("sem: %p\r\n", (void*)txDoneSemHandle);

  auto rxQueueHandle = ActiveOs::CreateMessageQueue(8, sizeof(uint16_t));
  printf("msgq: %p\r\n", (void*)rxQueueHandle);

  static UartSendTransport uart1(GetUart1Handle(), txDoneSemHandle);
  static UartReceiveTransport uart2(GetUart2Handle(), rxQueueHandle);
  static UartBlockingSendTransport debugUart(GetUart3Handle());

  printf("transports ok\r\n");

  uart2.startListening();
  printf("listening ok\r\n");

  static MavlinkTxTask<UartSendTransport> mavlinkTxTask(uart1);
  static MavlinkRxTask<UartReceiveTransport, UartBlockingSendTransport> mavlinkRxTask(uart2, debugUart);
  static WatchdogTask watchdogTask{};

  mavlinkTxTask.start();
  printf("tx task started\r\n");
  mavlinkRxTask.start();
  printf("rx task started\r\n");

  Watchdog::Init();
  watchdogTask.start();

  ActiveOs::StartKernel();
  ActiveOs::Delay(ActiveOs::WaitForever);
}
