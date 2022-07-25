/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <gpio.h>
#include <security/tpm/tis.h>

#include "gpio.h"

void fill_lb_gpios(struct lb_gpios *gpios)
{
	/* TODO: add Chrome specific gpios */
}

int tis_plat_irq_status(void)
{
	return gpio_eint_poll(GPIO_GSC_AP_INT_ODL);
}
