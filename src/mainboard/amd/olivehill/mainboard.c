/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>

/**********************************************
 * enable the dedicated function in mainboard.
 **********************************************/
static void mainboard_enable(struct device *dev)
{
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
