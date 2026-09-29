/* SPDX-License-Identifier: GPL-2.0-only */

#include "board.h"
#include <boot/coreboot_tables.h>
#include <drivers/tpm/cr50.h>

void setup_chromeos_gpios(void)
{
	if (CONFIG(EC_GOOGLE_CHROMEEC))
		gpio_input_pullup(GPIO_EC_AP_INT);

	if (CONFIG(MAINBOARD_HAS_TPM2))
		gpio_input_irq(GPIO_GSC_AP_INT, IRQ_TYPE_RISING_EDGE, GPIO_PULL_UP);
}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	const struct lb_gpio chromeos_gpios[] = {
#if CONFIG(EC_GOOGLE_CHROMEEC)
		{GPIO_EC_AP_INT.addr, ACTIVE_LOW, gpio_get(GPIO_EC_AP_INT),
			"EC interrupt"},
#endif
#if CONFIG(MAINBOARD_HAS_TPM2)
		{GPIO_GSC_AP_INT.addr, ACTIVE_HIGH, gpio_get(GPIO_GSC_AP_INT),
			"TPM interrupt"},
#endif
	};

	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int cr50_plat_irq_status(void)
{
	return gpio_irq_status(GPIO_GSC_AP_INT);
}
