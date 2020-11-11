/* SPDX-License-Identifier: GPL-2.0-only */

#include "board.h"
#include <boardid.h>
#include <bootblock_common.h>
#include <soc/clock.h>
#include <console/console.h>
#include <device/device.h>
#include <bootblock_common.h>
#include <soc/clock.h>

static void mainboard_init(struct device *dev)
{
	/* Configure clock for eMMC */
	clock_configure_sdcc(1, 384 * MHz);
	/* Configure clock for SD card */
	clock_configure_sdcc(2, 50 * MHz);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name = CONFIG_MAINBOARD_PART_NUMBER,
	.enable_dev = mainboard_enable,
};
