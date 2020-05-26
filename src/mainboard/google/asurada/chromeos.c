/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <gpio.h>
#include <security/tpm/tis.h>

#include "gpio.h"

void setup_chromeos_gpios(void)
{
	gpio_input(GPIO_WP);
	gpio_input_pullup(GPIO_EC_AP_INT);
	gpio_input_pullup(GPIO_EC_IN_RW);
	gpio_input_pullup(GPIO_H1_AP_INT);
	gpio_input_pullup(GPIO_SD_CD);
	gpio_output(GPIO_RESET, 0);
}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{GPIO_EC_IN_RW.id, ACTIVE_LOW, -1, "EC in RW"},
		{GPIO_EC_AP_INT.id, ACTIVE_LOW, -1, "EC interrupt"},
		{GPIO_H1_AP_INT.id, ACTIVE_HIGH, -1, "TPM interrupt"},
		{GPIO_SD_CD.id, ACTIVE_HIGH, -1, "SD card detect"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int get_write_protect_state(void)
{
	return !gpio_get(GPIO_WP);
}

int tis_plat_irq_status(void)
{
	return gpio_eint_poll(GPIO_H1_AP_INT);
}
