/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <boardid.h>
#include <gpio.h>
#include <security/tpm/tis.h>

int tis_plat_irq_status(void)
{
	gpio_t irq_gpio = GPIO_85;
	uint32_t board_ver = board_id();

	if (CONFIG(BOARD_GOOGLE_GUYBRUSH) || (CONFIG(BOARD_GOOGLE_NIPPERKIN) && board_ver == 1))
		irq_gpio = GPIO_3;

	return gpio_interrupt_status(irq_gpio);
}
