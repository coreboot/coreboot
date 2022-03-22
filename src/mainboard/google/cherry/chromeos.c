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
	gpio_input_pullup(GPIO_SD_CD);
	gpio_input_pullup(GPIO_EC_IN_RW);
	gpio_input_pullup(GPIO_GSC_AP_INT);
	gpio_output(GPIO_EN_SPK, 0);
	gpio_output(GPIO_RESET, 0);
	gpio_output(GPIO_XHCI_DONE, 0);
	if (CONFIG(CHERRY_USE_RT1019))
		gpio_output(GPIO_BEEP_ON, 0);
	else if (CONFIG(CHERRY_USE_RT1011))
		gpio_output(GPIO_RST_RT1011, 0);

}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{GPIO_EC_AP_INT.id, ACTIVE_LOW, -1, "EC interrupt"},
		{GPIO_SD_CD.id, ACTIVE_LOW, -1, "SD card detect"},
		{GPIO_EC_IN_RW.id, ACTIVE_LOW, -1, "EC in RW"},
		/*
		 * The GPIO_GSC_AP_INT itself is active low, but the payloads will
		 * create the IRQ using its eint driver, which is active high.
		 */
		{GPIO_GSC_AP_INT.id, ACTIVE_HIGH, -1, "TPM interrupt"},
	};

	struct lb_gpio rt1019_gpios[] = {
		{GPIO_BEEP_ON.id, ACTIVE_HIGH, -1, "beep enable"},
		{GPIO_EN_SPK.id, ACTIVE_HIGH, -1, "speaker enable"},
	};

	struct lb_gpio rt1011_gpios[] = {
		{GPIO_RST_RT1011.id, ACTIVE_HIGH, -1, "rt1011 reset"},
		{GPIO_EN_SPK.id, ACTIVE_HIGH, -1, "speaker enable"},
	};

	struct lb_gpio spk_gpios[] = {
		{GPIO_EN_SPK.id, ACTIVE_HIGH, -1, "speaker enable"},
	};

	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));

	if (CONFIG(CHERRY_USE_RT1019))
		lb_add_gpios(gpios, rt1019_gpios, ARRAY_SIZE(rt1019_gpios));
	else if (CONFIG(CHERRY_USE_RT1011))
		lb_add_gpios(gpios, rt1011_gpios, ARRAY_SIZE(rt1011_gpios));
	else if (CONFIG(CHERRY_USE_MAX98390))
		lb_add_gpios(gpios, spk_gpios, ARRAY_SIZE(spk_gpios));
}

int tis_plat_irq_status(void)
{
	return gpio_eint_poll(GPIO_GSC_AP_INT);
}

int get_ec_is_trusted(void)
{
	/* EC is trusted if not in RW. This is active low. */
	return !!gpio_get(GPIO_EC_IN_RW);
}
