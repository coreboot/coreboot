/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acp.h>
#include <amdblocks/chip.h>
#include <device/device.h>
#include <device/mmio.h>
#include <console/console.h>
#include "acp_def.h"

/* ACP registers and associated fields */
#define ACP_I2S_PIN_CONFIG	0x1400	/* HDA, Soundwire, I2S */
#define  PIN_CONFIG_MASK	(7 << 0)
#define ACP_I2S_WAKE_EN		0x1414
#define  WAKE_EN_MASK		(1 << 0)
#define ACP_PME_EN		0x1418
#define  PME_EN_MASK		(1 << 0)

static void acp_update32(uintptr_t bar, uint32_t reg, uint32_t clear, uint32_t set)
{
	clrsetbits32((void *)(bar + reg), clear, set);
}

void acp_init(struct device *dev)
{
	const struct soc_amd_common_config *cfg = soc_get_common_config();
	struct resource *res;
	uintptr_t bar;

	res = dev->resource_list;
	if (!res || !res->base) {
		printk(BIOS_ERR, "Error, unable to configure pin in %s\n", __func__);
		return;
	}

	/* Set the proper I2S_PIN_CONFIG state */
	bar = (uintptr_t)res->base;
	acp_update32(bar, ACP_I2S_PIN_CONFIG, PIN_CONFIG_MASK, cfg->acp_config.acp_pin_cfg);

	/* Enable ACP_PME_EN and ACP_I2S_WAKE_EN for I2S_WAKE event */
	acp_update32(bar, ACP_I2S_WAKE_EN, WAKE_EN_MASK, !!cfg->acp_config.acp_i2s_wake_enable);
	acp_update32(bar, ACP_PME_EN, PME_EN_MASK, !!cfg->acp_config.acp_pme_enable);
}
