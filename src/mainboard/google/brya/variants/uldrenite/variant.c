/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <delay.h>
#include <fw_config.h>

#define RW350R_RST_DELAY_MS 20
#define RW350R_PERST_DELAY_MS 30

static const struct pad_config rw350r_en_pad[] = {
	/* H23 : LTE_PWR_OFF_EN */
	PAD_CFG_GPO(GPP_H23, 1, DEEP),
};
static const struct pad_config rw350r_rst_pad[] = {
	/* F12 : WWAN_RST_L */
	PAD_CFG_GPO_LOCK(GPP_F12, 1, LOCK_CONFIG),
};
static const struct pad_config rw350r_perst_pad[] = {
	/* F13 : PLTRST_WWAN# */
	PAD_CFG_GPO(GPP_F13, 1, DEEP),
};

static const struct pad_config touchscreen_disable_pads[] = {
	/* A21  : TCHSCR_REPORT_EN */
	PAD_NC(GPP_A21, NONE),
	/* B16  : SOC_I2C_TCHSCR_SDA */
	PAD_NC(GPP_B16, NONE),
	/* B17  : SOC_I2C_TCHSCR_SCL */
	PAD_NC(GPP_B17, NONE),
	/* D15  : TCHSCR_RST_L */
	PAD_NC(GPP_D15, NONE),
	/* D16  : TCHSCR_INT_ODL */
	PAD_NC(GPP_D16, NONE),
	/* E17  : EN_PP3300_TCHSCR_X */
	PAD_NC(GPP_E17, NONE),
};

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	if (fw_config_probe(FW_CONFIG(TOUCHSCREEN, TOUCHSCREEN_NONE))) {
		printk(BIOS_INFO, "Disable touchscreen GPIO pins.\n");
		gpio_padbased_override(padbased_table, touchscreen_disable_pads,
				ARRAY_SIZE(touchscreen_disable_pads));
	}
}

void variant_init(void)
{
	/*
	 * RW350R power on seuqence:
	 * De-assert WWAN_EN -> 20ms -> de-assert WWAN_RST -> 30ms ->
	 * de-assert WWAN_PERST
	 */
	gpio_configure_pads(rw350r_en_pad, ARRAY_SIZE(rw350r_en_pad));
	mdelay(RW350R_RST_DELAY_MS);
	gpio_configure_pads(rw350r_rst_pad, ARRAY_SIZE(rw350r_rst_pad));
	mdelay(RW350R_PERST_DELAY_MS);
	gpio_configure_pads(rw350r_perst_pad, ARRAY_SIZE(rw350r_perst_pad));
}
