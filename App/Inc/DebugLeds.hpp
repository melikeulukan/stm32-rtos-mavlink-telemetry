#pragma once

#if defined(RTOS_BACKEND_FREERTOS)

#include "main.h"

inline void IndicateTxActivity()
{
	HAL_GPIO_TogglePin(GPIOB, LD1_Pin);
}

inline void IndicateRxActivity()
{
	HAL_GPIO_TogglePin(GPIOB, LD2_Pin);
}

#elif defined(RTOS_BACKEND_ZEPHYR)

#include <zephyr/drivers/gpio.h>

inline void IndicateTxActivity()
{
	static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
	static bool configured = false;
	if (!configured) {
		gpio_pin_configure_dt(&led0, GPIO_OUTPUT_INACTIVE);
		configured = true;
	}
	gpio_pin_toggle_dt(&led0);
}

inline void IndicateRxActivity()
{
	static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
	static bool configured = false;
	if (!configured) {
		gpio_pin_configure_dt(&led1, GPIO_OUTPUT_INACTIVE);
		configured = true;
	}
	gpio_pin_toggle_dt(&led1);
}

#endif
