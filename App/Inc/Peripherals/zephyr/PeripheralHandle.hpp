#pragma once
#include <zephyr/device.h>

using PeripheralHandle = const struct device;

[[nodiscard]] PeripheralHandle* GetUart1Handle();
[[nodiscard]] PeripheralHandle* GetUart2Handle();
[[nodiscard]] PeripheralHandle* GetUart3Handle();