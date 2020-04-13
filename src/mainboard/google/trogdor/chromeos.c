/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <bootmode.h>
#include "board.h"
#include <security/tpm/tis.h>

int get_write_protect_state(void)
{
	return !gpio_get(GPIO_WP_STATE);
}

void setup_chromeos_gpios(void)
{
	gpio_input_pullup(GPIO_EC_IN_RW);
	gpio_input_pullup(GPIO_AP_EC_INT);
	gpio_output(GPIO_AP_SUSPEND, 1);
	gpio_input(GPIO_WP_STATE);
	gpio_input_pullup(GPIO_SD_CD_L);
	gpio_input_irq(GPIO_H1_AP_INT, IRQ_TYPE_RISING_EDGE, GPIO_PULL_UP);
	gpio_output(GPIO_AMP_ENABLE, 0);
}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{GPIO_EC_IN_RW.addr, ACTIVE_LOW, gpio_get(GPIO_EC_IN_RW),
			"EC in RW"},
		{GPIO_AP_EC_INT.addr, ACTIVE_LOW, gpio_get(GPIO_AP_EC_INT),
			"EC interrupt"},
		{GPIO_H1_AP_INT.addr, ACTIVE_LOW, gpio_get(GPIO_H1_AP_INT),
			"TPM interrupt"},
		{GPIO_SD_CD_L.addr, ACTIVE_LOW, gpio_get(GPIO_SD_CD_L),
			"SD card detect"},
		{GPIO_AMP_ENABLE.addr, ACTIVE_HIGH, gpio_get(GPIO_AMP_ENABLE),
			"speaker enable"},
	};

	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int tis_plat_irq_status(void)
{
	return gpio_irq_status(GPIO_H1_AP_INT);
}
