/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <console/console.h>
#include <fw_config.h>
#include <gpio.h>

static const struct pad_config touch_enable_pads[] = {
	/* C0  : SMBCLK ==> EN_PP3300_TCHSCR */
	PAD_CFG_GPO(GPP_C0, 1, DEEP),
};

static const struct pad_config touch_disable_pads[] = {
	/* C0  : SMBCLK ==> NC */
	PAD_NC(GPP_C0, NONE),
};

static void fw_config_handle(void *unused)
{
	if (fw_config_probe(FW_CONFIG(TOUCH, TOUCH_ELAN0001))) {
		printk(BIOS_INFO, "Configure Touch GPIO GPP_C0 to GPO_H.\n");
		gpio_configure_pads(touch_enable_pads, ARRAY_SIZE(touch_enable_pads));
	} else {
		printk(BIOS_INFO, "Configure Touch GPIO GPP_C0 to NC.\n");
		gpio_configure_pads(touch_disable_pads, ARRAY_SIZE(touch_disable_pads));
	}
}
BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, fw_config_handle, NULL);
