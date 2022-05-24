/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <ramdetect.h>
#include <symbols.h>
#include <device/device.h>
#include <bootmem.h>

void bootmem_platform_add_ranges(void)
{
	bootmem_add_range((uintptr_t)_bl31, REGION_SIZE(bl31), BM_MEM_BL31);
}

static void mainboard_enable(struct device *dev)
{
	int ram_size_mb = probe_ramsize((uintptr_t)_dram, CONFIG_DRAM_SIZE_MB);
	ram_resource_kb(dev, 0, (uintptr_t)_dram / KiB, ram_size_mb * KiB);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};

struct chip_operations mainboard_emulation_qemu_aarch64_ops = { };
