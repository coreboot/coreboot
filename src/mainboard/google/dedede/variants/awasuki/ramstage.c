/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <fw_config.h>
#include <soc/soc_chip.h>
#include <soc/gpio.h>
#include <static.h>

static const struct pad_config ts_disable_pad[] = {
	/* A11 : TOUCH_RPT_EN */
	PAD_NC(GPP_A11, NONE),

	/* D4  : TOUCH_INT_ODL */
	PAD_NC(GPP_D4, NONE),
	/* D5  : TOUCH_RESET_L */
	PAD_NC(GPP_D5, NONE),
	/* D6  : EN_PP3300_TOUCH_S0 */
	PAD_NC(GPP_D6, NONE),

	/* H4  : AP_I2C_TS_SDA */
	PAD_NC(GPP_H4, NONE),
	/* H5  : AP_I2C_TS_SCL */
	PAD_NC(GPP_H5, NONE),
};

void variant_devtree_update(void)
{
	struct soc_intel_jasperlake_config *cfg = config_of_soc();

	if (fw_config_probe(FW_CONFIG(EXT_VR, EXT_VR_ABSENT))) {
		cfg->disable_external_bypass_vr = 1;
	}

	if (!fw_config_probe(FW_CONFIG(TOUCHSCREEN, TOUCHSCREEN_PRESENT))) {
		cfg->SerialIoI2cMode[PchSerialIoIndexI2C2] = PchSerialIoDisabled;
		gpio_configure_pads(ts_disable_pad, ARRAY_SIZE(ts_disable_pad));
	}
}
