/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <gpio.h>
#include <security/tpm/tis.h>

#include "gpio.h"
#include "panel.h"

void setup_chromeos_gpios(void)
{
	/* Set up open-drain pins */
	gpio_input(GPIO_EC_AP_INT_ODL);
	gpio_input(GPIO_GSC_AP_INT_ODL);
	gpio_input(GPIO_AP_WP_ODL);

	/* Set up GPOs */
	gpio_output(GPIO_AP_EC_WARM_RST_REQ, 0);
	gpio_output(GPIO_EN_SPKR, 0);
	gpio_output(GPIO_XHCI_INIT_DONE, 0);
}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{GPIO_EC_AP_INT_ODL.id, ACTIVE_LOW, -1, "EC interrupt"},
		{GPIO_GSC_AP_INT_ODL.id, ACTIVE_HIGH, -1, "TPM interrupt"},
		{GPIO_EN_SPKR.id, ACTIVE_HIGH, -1, "speaker enable"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));

	fill_lp_backlight_gpios(gpios);
}

int tis_plat_irq_status(void)
{
	return gpio_eint_poll(GPIO_GSC_AP_INT_ODL);
}
