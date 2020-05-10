/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>
#include <security/tpm/tis.h>

#include "gpio.h"

int tis_plat_irq_status(void)
{
	return gpio_eint_poll(CR50_IRQ);
}
