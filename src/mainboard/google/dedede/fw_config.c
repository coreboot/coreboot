/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <device/mmio.h>
#include <fw_config.h>
#include <gpio.h>
#include <intelblocks/gpio.h>

#define PAD_CFG_DW2_OFFSET	(2 * sizeof(uint32_t))
#define VCCIOSEL_1V8		(1 << 8)

static void fw_config_handle(void *unused)
{
	void *pad_conf_offset = gpio_dwx_address(GPP_D17) + PAD_CFG_DW2_OFFSET;
	uint32_t pad_conf = read32(pad_conf_offset);

	if (fw_config_probe(FW_CONFIG(AUDIO_AMP, RT1015P_AUTO))) {
		pad_conf |= VCCIOSEL_1V8;
		write32(pad_conf_offset, pad_conf);
	}
}
BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, fw_config_handle, NULL);
