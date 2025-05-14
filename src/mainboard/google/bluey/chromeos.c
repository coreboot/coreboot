/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <bootmode.h>
#include <drivers/tpm/cr50.h>
#include "board.h"

void setup_chromeos_gpios(void)
{
	if (CONFIG(EC_GOOGLE_CHROMEEC))
		gpio_input_pullup(GPIO_AP_EC_INT);

	if (CONFIG(TPM_GOOGLE_TI50))
		gpio_input_irq(GPIO_GSC_AP_INT, IRQ_TYPE_RISING_EDGE, GPIO_PULL_UP);

	if (CONFIG(MAINBOARD_HAS_FINGERPRINT)) {
		gpio_output(GPIO_FP_RST_L, 0);
		if (CONFIG(MAINBOARD_HAS_FINGERPRINT_VIA_SPI)) {
			gpio_output(GPIO_FPMCU_BOOT0, 0);
			gpio_output(GPIO_EN_FP_RAILS, 0);
			gpio_input_irq(GPIO_FPMCU_INT, IRQ_TYPE_LEVEL, GPIO_PULL_UP);
		}
	}

	gpio_output(GPIO_SNDW_AMP_0_ENABLE, 0);
	gpio_output(GPIO_SNDW_AMP_1_ENABLE, 0);
}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	/* TODO: Add required GPIO after referring to the schematics */
	const struct lb_gpio chromeos_gpios[] = {
#if CONFIG(EC_GOOGLE_CHROMEEC)
		{GPIO_AP_EC_INT.addr, ACTIVE_LOW, gpio_get(GPIO_AP_EC_INT),
			"EC interrupt"},
#endif
#if CONFIG(TPM_GOOGLE_TI50)
		{GPIO_GSC_AP_INT.addr, ACTIVE_HIGH, gpio_get(GPIO_GSC_AP_INT),
			"TPM interrupt"},
#endif
		{GPIO_SNDW_AMP_0_ENABLE.addr, ACTIVE_HIGH, gpio_get(GPIO_SNDW_AMP_0_ENABLE),
			"Speaker 0 enable"},
		{GPIO_SNDW_AMP_1_ENABLE.addr, ACTIVE_HIGH, gpio_get(GPIO_SNDW_AMP_1_ENABLE),
			"Speaker 1 enable"},
#if CONFIG(MAINBOARD_HAS_SD_CONTROLLER)
		{GPIO_SD_CD_L.addr, ACTIVE_LOW, gpio_get(GPIO_SD_CD_L),
			"SD card detect"},
#endif
	};

	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int cr50_plat_irq_status(void)
{
	return gpio_irq_status(GPIO_GSC_AP_INT);
}
