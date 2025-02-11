/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <boot/coreboot_tables.h>
#include <drivers/tpm/cr50.h>
#include <ec/google/chromeec/ec.h>
#include <gpio.h>

#include "gpio.h"

void setup_chromeos_gpios(void)
{
	gpio_input(GPIO_EC_AP_INT_ODL);
	gpio_input(GPIO_GSC_AP_INT_ODL);
	gpio_output(GPIO_AP_EC_WARM_RST_REQ, 0);
	gpio_output(GPIO_XHCI_INIT_DONE, 0);
}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{GPIO_XHCI_INIT_DONE.id, ACTIVE_HIGH, -1, "XHCI init done"},
		{GPIO_EC_AP_INT_ODL.id, ACTIVE_LOW, -1, "EC interrupt"},
		{GPIO_GSC_AP_INT_ODL.id, ACTIVE_HIGH, -1, "TPM interrupt"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int cr50_plat_irq_status(void)
{
	return gpio_eint_poll(GPIO_GSC_AP_INT_ODL);
}
