/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <symbols.h>
#include <bootmem.h>

static void soc_enable(struct device *dev)
{
	ram_range(dev, 0, (uintptr_t)_dram, CONFIG_DRAM_SIZE_MB * (uint64_t)MiB);
}

struct chip_operations soc_ti_am335x_ops = {
	CHIP_NAME("TI AM335X")
	.enable_dev = soc_enable,
};
