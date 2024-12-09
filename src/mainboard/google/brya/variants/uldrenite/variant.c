/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <delay.h>

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
