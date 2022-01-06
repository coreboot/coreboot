/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <bootmode.h>
#include "board.h"
#include <security/tpm/tis.h>

void setup_chromeos_gpios(void)
{
	if (CONFIG(EC_GOOGLE_CHROMEEC)) {
		gpio_input_pullup(GPIO_EC_IN_RW);
		gpio_input_pullup(GPIO_AP_EC_INT);
	}
	if (CONFIG(MAINBOARD_HAS_TPM2))
		gpio_input_irq(GPIO_H1_AP_INT, IRQ_TYPE_RISING_EDGE, GPIO_PULL_UP);

	gpio_input_pullup(GPIO_SD_CD_L);

	if (CONFIG(HEROBRINE_HAS_FINGERPRINT)) {
		gpio_output(GPIO_FPMCU_BOOT0, 0);
		gpio_output(GPIO_FP_RST_L, 0);
		gpio_output(GPIO_EN_FP_RAILS, 0);
	}
}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	const struct lb_gpio chromeos_gpios[] = {
		{GPIO_SD_CD_L.addr, ACTIVE_LOW, gpio_get(GPIO_SD_CD_L),
			"SD card detect"},
#if CONFIG(EC_GOOGLE_CHROMEEC)
		{GPIO_EC_IN_RW.addr, ACTIVE_LOW, gpio_get(GPIO_EC_IN_RW),
			"EC in RW"},
		{GPIO_AP_EC_INT.addr, ACTIVE_LOW, gpio_get(GPIO_AP_EC_INT),
			"EC interrupt"},
#endif
#if CONFIG(MAINBOARD_HAS_TPM2)
		{GPIO_H1_AP_INT.addr, ACTIVE_HIGH, gpio_get(GPIO_H1_AP_INT),
			"TPM interrupt"},
#endif
	};

	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int get_ec_is_trusted(void)
{
	/* EC is trusted if not in RW. This is active low. */
	if (CONFIG(EC_GOOGLE_CHROMEEC))
		return !!gpio_get(GPIO_EC_IN_RW);
	else /* If no EC, always return true */
		return 1;
}

int tis_plat_irq_status(void)
{
	return gpio_irq_status(GPIO_H1_AP_INT);
}
