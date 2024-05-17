/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <gpio.h>

#include "gpio.h"

void setup_chromeos_gpios(void)
{
	gpio_output(GPIO_AP_FP_FW_UP_STRAP, 0);
	gpio_output(GPIO_EN_PWR_FP, 0);
	gpio_output(GPIO_FP_RST_1V8_S3_L, 0);
	gpio_output(GPIO_XHCI_INIT_DONE, 0);
}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{ GPIO_XHCI_INIT_DONE.id, ACTIVE_HIGH, -1, "XHCI init done" },
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}
