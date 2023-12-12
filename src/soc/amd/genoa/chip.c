/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>

static void soc_init(void *chip_info)
{
}

static void soc_final(void *chip_info)
{
}

struct chip_operations soc_amd_genoa_ops = {
	CHIP_NAME("AMD Genoa SoC")
	.init = soc_init,
	.final = soc_final,
};
