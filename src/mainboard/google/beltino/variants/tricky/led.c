/* SPDX-License-Identifier: GPL-2.0-only */

#include <superio/ite/common/ite.h>
#include <superio/ite/common/ite_gpio.h>
#include <superio/ite/it8772f/it8772f.h>
#include "../../onboard.h"

void set_power_led(int state)
{
	/* Configure GPIO22 as power LED */
	ite_reg_write(IT8772F_GPIO_DEV, ITE_GPIO_REG_SELECT(1), 0xf7);
	ite_gpio_setup(IT8772F_GPIO_DEV, 22, ITE_GPIO_OUTPUT,
		state == LED_BLINK ? ITE_GPIO_ALT_FN_MODE : ITE_GPIO_SIMPLE_IO_MODE,
		(state != LED_OFF ? ITE_GPIO_POL_INVERT : 0) |
		(state == LED_BLINK ? ITE_GPIO_PULLUP_ENABLE : 0));
	ite_gpio_setup_led(IT8772F_GPIO_DEV, 22, ITE_GPIO_LED_1,
			   ITE_LED_FREQ_1HZ, ITE_LED_CONTROL_DEFAULT);
}
