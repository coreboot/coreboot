/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmem.h>
#include <device/device.h>
#include <symbols.h>

static void soc_enable(struct device *dev)
{
	ram_range(dev, 0, 0, CONFIG_DRAM_SIZE_MB * (uint64_t)MiB);
}

struct chip_operations soc_xilinx_zynq7000_ops = {
	.name = "Xilinx Zynq 7000",
	.enable_dev = soc_enable,
};
