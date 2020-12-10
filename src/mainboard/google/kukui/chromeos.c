/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <gpio.h>
#include <security/tpm/tis.h>

#include "gpio.h"

void setup_chromeos_gpios(void)
{
	gpio_input(GPIO_WP);
	gpio_input_pullup(EC_IN_RW);
	gpio_input_pullup(EC_IRQ);
	gpio_input_pullup(CR50_IRQ);
	gpio_output(GPIO_RESET, 0);
	gpio_output(GPIO_EN_SPK_AMP, 0);
}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{EC_IN_RW.id, ACTIVE_HIGH, -1, "EC in RW"},
		{EC_IRQ.id, ACTIVE_LOW, -1, "EC interrupt"},
		{CR50_IRQ.id, ACTIVE_HIGH, -1, "TPM interrupt"},
		{GPIO_EN_SPK_AMP.id, ACTIVE_HIGH, -1, CONFIG_SPEAKER_GPIO_NAME},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int get_write_protect_state(void)
{
	return !gpio_get(GPIO_WP);
}

int tis_plat_irq_status(void)
{
	return gpio_eint_poll(CR50_IRQ);
}
