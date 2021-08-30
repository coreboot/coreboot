/* SPDX-License-Identifier: GPL-2.0-only */

#include "board.h"
#include <boardid.h>
#include <bootblock_common.h>
#include <soc/clock.h>
#include <console/console.h>
#include <device/device.h>
#include <device/mmio.h>
#include <bootblock_common.h>
#include <soc/clock.h>

static void configure_sdhci(void)
{
	/* Program eMMC drive strength to 16/10/10 mA */
	write32((void *)SDC1_TLMM_CFG_ADDR, 0x9FE4);
	/* Program SD card drive strength to 16/10/10 mA */
	write32((void *)SDC2_TLMM_CFG_ADDR, 0x1FE4);
}

static void mainboard_init(struct device *dev)
{
	/* Configure clock for eMMC */
	clock_configure_sdcc(1, 384 * MHz);
	/* Configure clock for SD card */
	clock_configure_sdcc(2, 50 * MHz);
	configure_sdhci();
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name = CONFIG_MAINBOARD_PART_NUMBER,
	.enable_dev = mainboard_enable,
};
