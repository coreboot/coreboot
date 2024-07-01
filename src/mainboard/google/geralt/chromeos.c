/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <drivers/tpm/cr50.h>
#include <fw_config.h>
#include <gpio.h>

#include "gpio.h"
#include "panel.h"

void setup_chromeos_gpios(void)
{
	/* Set up open-drain pins */
	gpio_input(GPIO_EC_AP_INT_ODL);
	gpio_input(GPIO_EC_AP_HID_INT_ODL);
	gpio_input(GPIO_EC_AP_RSVD0_ODL);
	gpio_input(GPIO_GSC_AP_INT_ODL);
	gpio_input(GPIO_AP_WP_ODL);
	gpio_input(GPIO_EDP_HPD_1V8);
	gpio_input(GPIO_EC_AP_HPD_OD);
	gpio_input(GPIO_PCIE_WAKE_1V8_ODL);
	gpio_input(GPIO_PCIE_CLKREQ_1V8_ODL);
	gpio_input(GPIO_SAR_INT_ODL);
	gpio_input(GPIO_SD_CD_ODL);
	gpio_input(GPIO_HP_INT_ODL);
	gpio_input(GPIO_SPKR_INT_ODL);
	gpio_input(GPIO_TCHSCR_INT_1V8_L);

	/* Set up GPOs */
	gpio_output(GPIO_AP_EC_WARM_RST_REQ, 0);
	gpio_output(GPIO_EN_SPKR, 0);
	gpio_output(GPIO_RST_SPKR_L, 0);
	gpio_output(GPIO_XHCI_INIT_DONE, 0);
	gpio_output(GPIO_BEEP_ON_OD, 0);
	gpio_output(GPIO_MT7921_PMU_EN_1V8, 0);
}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{GPIO_EC_AP_INT_ODL.id, ACTIVE_LOW, -1, "EC interrupt"},
		{GPIO_GSC_AP_INT_ODL.id, ACTIVE_HIGH, -1, "TPM interrupt"},
		{GPIO_XHCI_INIT_DONE.id, ACTIVE_HIGH, -1, "XHCI init done"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));

	fill_lp_backlight_gpios(gpios);

	if (CONFIG(GERALT_SDCARD_INIT)) {
		struct lb_gpio sd_card_gpios[] = {
			{GPIO_SD_CD_ODL.id, ACTIVE_LOW, -1, "SD card detect"},
		};
		lb_add_gpios(gpios, sd_card_gpios, ARRAY_SIZE(sd_card_gpios));
	}

	if (CONFIG(GERALT_USE_NAU8318)) {
		struct lb_gpio nau8318_gpios[] = {
			{GPIO_EN_SPKR.id, ACTIVE_HIGH, -1, "speaker enable"},
			{GPIO_BEEP_ON_OD.id, ACTIVE_HIGH, -1, "beep enable"},
		};
		lb_add_gpios(gpios, nau8318_gpios, ARRAY_SIZE(nau8318_gpios));
	} else if (fw_config_probe(FW_CONFIG(AUDIO_AMP, AMP_MAX98390)) ||
		   fw_config_probe(FW_CONFIG(AUDIO_AMP, AMP_TAS2563))) {
		struct lb_gpio smartamp_gpios[] = {
			{GPIO_RST_SPKR_L.id, ACTIVE_LOW, -1, "speaker reset"},
		};
		lb_add_gpios(gpios, smartamp_gpios, ARRAY_SIZE(smartamp_gpios));
	}
}

int cr50_plat_irq_status(void)
{
	return gpio_eint_poll(GPIO_GSC_AP_INT_ODL);
}
