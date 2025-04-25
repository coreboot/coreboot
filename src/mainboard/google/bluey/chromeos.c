/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <bootmode.h>
#include "board.h"
#include <drivers/tpm/cr50.h>

void setup_chromeos_gpios(void)
{
	gpio_input_pullup(GPIO_AP_EC_INT);

	gpio_input_irq(GPIO_GSC_AP_INT, IRQ_TYPE_RISING_EDGE, GPIO_PULL_UP);
}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	/* TODO: Add required GPIO after referring to the schematics */
	const struct lb_gpio chromeos_gpios[] = {
		{GPIO_AP_EC_INT.addr, ACTIVE_LOW, gpio_get(GPIO_AP_EC_INT),
			"EC interrupt"},
		{GPIO_GSC_AP_INT.addr, ACTIVE_HIGH, gpio_get(GPIO_GSC_AP_INT),
			"TPM interrupt"},
	};

	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int cr50_plat_irq_status(void)
{
	return gpio_irq_status(GPIO_GSC_AP_INT);
}
