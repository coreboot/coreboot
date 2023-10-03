/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <console/console.h>
#include <fw_config.h>
#include <gpio.h>

static const struct pad_config fp_disable_pads[] = {
	PAD_NC(GPP_D0, NONE),	/* D0  : ISH_GP0 ==> PCH_FP_BOOT0 */
	PAD_NC(GPP_D1, NONE),	/* D1  : ISH_GP1 ==> FP_RST_ODL */
	PAD_NC(GPP_D2, NONE),	/* D2  : ISH_GP2 ==> EN_FP_PWR */
};

static void fw_config_handle(void *unused)
{
	if (fw_config_probe(FW_CONFIG(FPMCU_MASK, FPMCU_DISABLED))) {
		printk(BIOS_INFO, "Disabling FP pads\n");
		gpio_configure_pads(fp_disable_pads, ARRAY_SIZE(fp_disable_pads));
	}
}
BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, fw_config_handle, NULL);
