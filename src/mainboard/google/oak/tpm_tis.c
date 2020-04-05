/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <gpio.h>
#include <security/tpm/tis.h>

#include "gpio.h"

int tis_plat_irq_status(void)
{
	return gpio_eint_poll(CR50_IRQ);
}
