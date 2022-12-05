/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmem.h>
#include <device/device.h>
#include <program_loading.h>
#include <symbols.h>
#include <soc/emi.h>

int payload_arch_usable_ram_quirk(uint64_t start, uint64_t size)
{
	if (size > REGION_SIZE(sram))
		return 0;

	if (start >= (uintptr_t)_sram && (start + size) <= (uintptr_t)_esram) {
		printk(BIOS_DEBUG, "MT8173 uses SRAM for loading BL31.\n");
		return 1;
	}

	return 0;
}

void bootmem_platform_add_ranges(void)
{
	bootmem_add_range(0x101000, 124 * KiB, BM_MEM_BL31);
}

static void soc_read_resources(struct device *dev)
{
	ram_range(dev, 0, (uintptr_t)_dram, sdram_size());
}

static void soc_init(struct device *dev)
{
}

static struct device_operations soc_ops = {
	.read_resources = soc_read_resources,
	.set_resources = noop_set_resources,
	.init = soc_init,
};

static void enable_soc_dev(struct device *dev)
{
	dev->ops = &soc_ops;
}

struct chip_operations soc_mediatek_mt8173_ops = {
	CHIP_NAME("SOC Mediatek MT8173")
	.enable_dev = enable_soc_dev,
};
