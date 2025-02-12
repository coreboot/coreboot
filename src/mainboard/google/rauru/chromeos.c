/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <drivers/tpm/cr50.h>
#include <fw_config.h>
#include <gpio.h>

#include "gpio.h"

void setup_chromeos_gpios(void)
{
	gpio_input(GPIO_EC_AP_INT_ODL);
	gpio_input(GPIO_GSC_AP_INT_ODL);
	gpio_input(GPIO_SD_CD_AP_ODL);
	gpio_output(GPIO_AP_EC_WARM_RST_REQ, 0);
	gpio_output(GPIO_AP_FP_FW_UP_STRAP, 0);
	gpio_output(GPIO_BEEP_ON, 0);
	gpio_output(GPIO_EN_PWR_FP, 0);
	gpio_output(GPIO_EN_SPKR, 0);
	gpio_output(GPIO_FP_RST_1V8_S3_L, 0);
	gpio_output(GPIO_XHCI_INIT_DONE, 0);
	gpio_output(GPIO_AP_SUSPEND_L, 1);
}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{ GPIO_XHCI_INIT_DONE.id, ACTIVE_HIGH, -1, "XHCI init done" },
		{ GPIO_EC_AP_INT_ODL.id, ACTIVE_LOW, -1, "EC interrupt" },
		{ GPIO_GSC_AP_INT_ODL.id, ACTIVE_HIGH, -1, "TPM interrupt" },
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));

	if (CONFIG(RAURU_SDCARD_INIT)) {
		struct lb_gpio sd_card_gpios[] = {
			{ GPIO_SD_CD_AP_ODL.id, ACTIVE_LOW, -1, "SD card detect" },
		};
		lb_add_gpios(gpios, sd_card_gpios, ARRAY_SIZE(sd_card_gpios));
	}

	struct lb_gpio nau8318_gpios[] = {
		{GPIO_EN_SPKR.id, ACTIVE_HIGH, -1, "speaker enable"},
		{GPIO_BEEP_ON.id, ACTIVE_HIGH, -1, "beep enable"},
	};

	struct lb_gpio smartamp_gpios[] = {
		{GPIO_EN_SPKR.id, ACTIVE_LOW, -1, "speaker reset"},
	};

	struct lb_gpio alc5645_gpios[] = {
		{GPIO_EN_SPKR.id, ACTIVE_HIGH, -1, "speaker enable"},
	};

	if (fw_config_probe(FW_CONFIG(AUDIO_AMP, AMP_TAS2563)))
		lb_add_gpios(gpios, smartamp_gpios, ARRAY_SIZE(smartamp_gpios));
	else if (fw_config_probe(FW_CONFIG(AUDIO_AMP, AMP_ALC5645)))
		lb_add_gpios(gpios, alc5645_gpios, ARRAY_SIZE(alc5645_gpios));
	else
		lb_add_gpios(gpios, nau8318_gpios, ARRAY_SIZE(nau8318_gpios));

	struct lb_gpio edp_pwm_backlight_gpios[] = {
		{GPIO_BL_PWM_1V8.id, ACTIVE_HIGH, -1, "PWM control"},
		{GPIO_AP_EDP_BKLTEN.id, ACTIVE_HIGH, -1, "backlight enable"},
	};
	lb_add_gpios(gpios, edp_pwm_backlight_gpios, ARRAY_SIZE(edp_pwm_backlight_gpios));
}

int cr50_plat_irq_status(void)
{
	return gpio_eint_poll(GPIO_GSC_AP_INT_ODL);
}
