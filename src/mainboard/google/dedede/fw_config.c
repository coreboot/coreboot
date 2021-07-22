/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <fw_config.h>
#include <gpio.h>

static const struct pad_config spkr_pads[] = {
	PAD_CFG_GPO_VCCIOSEL(GPP_D17, 0, PLTRST, 1V8),
};

static void fw_config_handle(void *unused)
{
	if (fw_config_probe(FW_CONFIG(AUDIO_AMP, RT1015P_AUTO))) {
		gpio_configure_pads(spkr_pads, ARRAY_SIZE(spkr_pads));
	}
}
BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, fw_config_handle, NULL);
