/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>
#include <ec/google/chromeec/ec.h>

static const struct soc_amd_gpio trembyle_bid1_bid2_gpio_set_stage_ram[] = {
	/* DMIC_SEL */
	PAD_GPO(GPIO_13, LOW), // Select Camera 1 Dmic
	/* USB_OC4_L - USB_A1 */
	PAD_NF(GPIO_14, USB_OC4_L, PULL_NONE),
	/* USB_OC2_L - USB A0 */
	PAD_NF(GPIO_18, USB_OC2_L, PULL_NONE),
	/* EN_PWR_WIFI */
	PAD_GPO(GPIO_29, HIGH),
	/* EN_PWR_TOUCHPAD_PS2 */
	PAD_GPO(GPIO_67, HIGH),
	/* DMIC_AD_EN */
	PAD_GPO(GPIO_84, HIGH),
	/* MST_GPIO_2 (Fw Update HDMI hub) */
	PAD_GPI(GPIO_86, PULL_NONE),
	/* EN_DEV_BEEP_L */
	PAD_GPO(GPIO_89, HIGH),
	/* MST_GPIO_3 (Fw Update HDMI hub) */
	PAD_GPI(GPIO_90, PULL_NONE),
	/* USI_RESET */
	PAD_GPO(GPIO_140, HIGH),
};

static const struct soc_amd_gpio trembyle_bid3_gpio_set_stage_ram[] = {
	/* DMIC_SEL */
	PAD_GPO(GPIO_13, LOW), // Select Camera 1 Dmic
	/* USB_OC4_L - USB_A1 */
	PAD_NF(GPIO_14, USB_OC4_L, PULL_NONE),
	/* USB_OC2_L - USB A0 */
	PAD_NF(GPIO_18, USB_OC2_L, PULL_NONE),
	/* EN_PWR_WIFI */
	PAD_GPO(GPIO_29, HIGH),
	/* EN_PWR_TOUCHPAD_PS2 */
	PAD_GPO(GPIO_67, HIGH),
	/* MST_GPIO_2 (Fw Update HDMI hub) */
	PAD_GPI(GPIO_86, PULL_NONE),
	/* EN_DEV_BEEP_L */
	PAD_GPO(GPIO_89, HIGH),
	/* MST_GPIO_3 (Fw Update HDMI hub) */
	PAD_GPI(GPIO_90, PULL_NONE),
	/* USI_RESET */
	PAD_GPO(GPIO_140, HIGH),
};

const struct soc_amd_gpio *variant_override_gpio_table(size_t *size)
{
	uint32_t board_version;

	/*
	 * If board version cannot be read, assume that this is an older revision of the board
	 * and so apply overrides. If board version is provided by the EC, then apply overrides
	 * if version < 2.
	 */
	if (google_chromeec_cbi_get_board_version(&board_version) != 0)
		board_version = 1;

	if (board_version <= 2) {
		*size = ARRAY_SIZE(trembyle_bid1_bid2_gpio_set_stage_ram);
		return trembyle_bid1_bid2_gpio_set_stage_ram;
	} else if (board_version <= 3) {
		*size = ARRAY_SIZE(trembyle_bid3_gpio_set_stage_ram);
		return trembyle_bid3_gpio_set_stage_ram;
	}

	*size = 0;
	return NULL;
}
