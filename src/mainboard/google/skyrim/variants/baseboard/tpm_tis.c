/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <security/tpm/tis.h>
#include <soc/gpio.h>

int tis_plat_irq_status(void)
{
	return gpio_interrupt_status(GPIO_18);
}
