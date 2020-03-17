/*
 * This file is part of the coreboot project.
 *
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <cbmem.h>
#include <ramdetect.h>
#include <symbols.h>
#include <device/device.h>
#include <bootmem.h>

extern u8 _secram[], _esecram[];

void bootmem_platform_add_ranges(void)
{
	bootmem_add_range((uintptr_t)_secram, REGION_SIZE(secram), BM_MEM_BL31);
}

static void mainboard_enable(struct device *dev)
{
	int ram_size_mb = probe_ramsize((uintptr_t)_dram, CONFIG_DRAM_SIZE_MB);
	ram_resource(dev, 0, (uintptr_t)_dram / KiB, ram_size_mb * KiB);
}

struct chip_operations mainboard_ops = {
	.name = "qemu_aarch64",
	.enable_dev = mainboard_enable,
};
