/* SPDX-License-Identifier: GPL-2.0-only */

#include <drivers/tpm/cr50.h>
#include <gpio.h>

#include "gpio.h"

int cr50_plat_irq_status(void)
{
	return gpio_eint_poll(CR50_IRQ);
}
