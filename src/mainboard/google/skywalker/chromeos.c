/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <boot/coreboot_tables.h>
#include <drivers/tpm/cr50.h>
#include <ec/google/chromeec/ec.h>
#include <fw_config.h>
#include <gpio.h>

#include "gpio.h"

void setup_chromeos_gpios(void)
{
	gpio_input(GPIO_EC_AP_INT_ODL);
	gpio_input(GPIO_GSC_AP_INT_ODL);
	gpio_input(GPIO_HP_INT_ODL);
	gpio_input(GPIO_SPKR_INT_ODL);
	gpio_input(GPIO_SAR_INT_ODL);
	gpio_input(GPIO_PEN_EJECT_OD);
	gpio_input(GPIO_TCHSCR_INT_1V8_ODL);
	gpio_input(GPIO_AP_WP_ODL);
	gpio_input(GPIO_EC_AP_DPOC_EN_ODL);
	gpio_input(GPIO_TCHPAD_INT_ODL);
	gpio_input(GPIO_HDMI_AP_INT_ODL);
	gpio_input(GPIO_FP_AP_INT_1V8_S3_ODL);
	gpio_input(GPIO_SD_CD_ODL);
	gpio_input(GPIO_HDMI_HPD_1V8_ODL);

	gpio_output(GPIO_EN_SPKR, 0);
	gpio_output(GPIO_RST_SPKR_L, 0);
	gpio_output(GPIO_USB3_HUB_RST_L, 0);
	gpio_output(GPIO_WWAN_RESET_L, 0);
	gpio_output(GPIO_EN_PWR_FP, 0);
	gpio_output(GPIO_EDP_BL_EN_1V8, 0);
	gpio_output(GPIO_TCHSCR_REPORT_DISABLE, 1);
	gpio_output(GPIO_CODEC_PWR_SOC_EN, 0);
	gpio_output(GPIO_EN_PP3300_EDP_X, 0);
	gpio_output(GPIO_EN_PP3300_WWAN_X, 0);
	gpio_output(GPIO_WWAN_SAR_DETECT_L, 1);
	gpio_output(GPIO_TCHSCR_RST_1V8_L, 0);
	gpio_output(GPIO_AP_HDMI_RST_ODL, 0);
	gpio_output(GPIO_AP_XHCI_INIT_DONE, 0);
	gpio_output(GPIO_AP_FP_FW_UP_STRAP, 0);
	gpio_output(GPIO_FP_RST_1V8_S3_L, 0);
	gpio_output(GPIO_EN_HDMI_PWR, 0);
	gpio_output(GPIO_AP_EC_WARM_RST_REQ, 0);
	gpio_output(GPIO_AP_SUSPEND_L, 1);
	gpio_output(GPIO_BT_KILL_1V8_L, 0);
	gpio_output(GPIO_WIFI_KILL_1V8_L, 0);
	gpio_output(GPIO_WWAN_PWR_OFF_L, 0);
	gpio_output(GPIO_WWAN_W_DISABLE_L, 0);
}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{GPIO_AP_XHCI_INIT_DONE.id, ACTIVE_HIGH, -1, "XHCI init done"},
		{GPIO_EC_AP_INT_ODL.id, ACTIVE_LOW, -1, "EC interrupt"},
		{GPIO_GSC_AP_INT_ODL.id, ACTIVE_HIGH, -1, "TPM interrupt"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));

	if (CONFIG(SKYWALKER_SDCARD_INIT)) {
		struct lb_gpio sd_card_gpios[] = {
			{ GPIO_SD_CD_ODL.id, ACTIVE_LOW, -1, "SD card detect" },
		};
		lb_add_gpios(gpios, sd_card_gpios, ARRAY_SIZE(sd_card_gpios));
	}

	if (fw_config_probe(FW_CONFIG(AUDIO_AMP, AMP_RT9123))) {
		struct lb_gpio rt9123_gpios[] = {
			{GPIO_EN_SPKR.id, ACTIVE_HIGH, -1, "rt9123_spk_en"},
		};
		lb_add_gpios(gpios, rt9123_gpios, ARRAY_SIZE(rt9123_gpios));
	} else if (fw_config_probe(FW_CONFIG(AUDIO_AMP, AMP_RT1019))) {
		struct lb_gpio rt1019_gpios[] = {
			{GPIO_EN_SPKR.id, ACTIVE_HIGH, -1, "rt1019_spk_en"},
		};
		lb_add_gpios(gpios, rt1019_gpios, ARRAY_SIZE(rt1019_gpios));
	} else if (fw_config_probe(FW_CONFIG(AUDIO_AMP, AMP_ALC5645))) {
		struct lb_gpio alc5645_gpios[] = {
			{GPIO_EN_SPKR.id, ACTIVE_HIGH, -1, "alc5645_spk_en"},
		};
		lb_add_gpios(gpios, alc5645_gpios, ARRAY_SIZE(alc5645_gpios));
	}

	struct lb_gpio edp_pwm_backlight_gpios[] = {
		{GPIO_BL_PWM_1V8.id, ACTIVE_HIGH, -1, "PWM control"},
		{GPIO_EDP_BL_EN_1V8.id, ACTIVE_HIGH, -1, "backlight enable"},
	};
	lb_add_gpios(gpios, edp_pwm_backlight_gpios, ARRAY_SIZE(edp_pwm_backlight_gpios));
}

int cr50_plat_irq_status(void)
{
	return gpio_eint_poll(GPIO_GSC_AP_INT_ODL);
}
