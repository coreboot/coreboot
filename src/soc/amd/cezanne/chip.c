/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <fsp/api.h>
#include <soc/southbridge.h>
#include <types.h>
#include "chip.h"

static void enable_dev(struct device *dev)
{
}

static void soc_init(void *chip_info)
{
	fsp_silicon_init(false); /* no S3 support yet */

	fch_init(chip_info);
}

static void soc_final(void *chip_info)
{
	fch_final(chip_info);
}

struct chip_operations soc_amd_cezanne_ops = {
	CHIP_NAME("AMD Cezanne SoC")
	.enable_dev = enable_dev,
	.init = soc_init,
	.final = soc_final
};
