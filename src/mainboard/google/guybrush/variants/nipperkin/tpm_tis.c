/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <boardid.h>
#include <gpio.h>
#include <drivers/tpm/cr50.h>

gpio_t cr50_override_gpio(gpio_t irq)
{
	if (board_id() == 1)
		return GPIO_3;
	else
		return irq;
}
