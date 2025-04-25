/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <soc/pcie.h>

bool mainboard_needs_pcie_init(void)
{
	/* Placeholder */
	return false;
}

static void mainboard_init(struct device *dev)
{
	/* Placeholder */
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
