/* SPDX-License-Identifier: GPL-2.0-only */

#include <superio/ite/common/ite.h>
#include <superio/ite/common/ite_gpio.h>

#include "common_defines.h"
#include "led.h"

void set_power_led(int state)
{
	// Board has a dual color LED: GPIO22 and GPIO23
	ite_reg_write(GPIO_DEV, ITE_GPIO_REG_SELECT(1), 0xf3 | LED_BOTH);
	ite_gpio_setup(GPIO_DEV, 22, ITE_GPIO_OUTPUT, ITE_GPIO_ALT_FN_MODE,
		       state & LED_WHITE ? ITE_GPIO_POL_INVERT : ITE_GPIO_CONTROL_DEFAULT);
	ite_gpio_setup(GPIO_DEV, 23, ITE_GPIO_OUTPUT, ITE_GPIO_ALT_FN_MODE,
		       state & LED_YELLOW ? ITE_GPIO_POL_INVERT : ITE_GPIO_CONTROL_DEFAULT);
}
