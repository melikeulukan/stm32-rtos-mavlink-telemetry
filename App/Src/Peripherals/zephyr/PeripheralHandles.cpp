#include "Peripherals/PeripheralHandles.hpp"
#include <zephyr/device.h>
#include <zephyr/devicetree.h>

PeripheralHandle* GetUart1Handle() { return DEVICE_DT_GET(DT_NODELABEL(usart1)); }
PeripheralHandle* GetUart2Handle() { return DEVICE_DT_GET(DT_NODELABEL(usart2)); }
PeripheralHandle* GetUart3Handle() { return DEVICE_DT_GET(DT_NODELABEL(usart3)); }