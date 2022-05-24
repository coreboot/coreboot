/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <symbols.h>
#include <ramdetect.h>

static void mainboard_enable(struct device *dev)
{
	size_t dram_mb_detected;

	if (!dev) {
		die("No dev0; die\n");
	}

	dram_mb_detected = probe_ramsize((uintptr_t)_dram, CONFIG_DRAM_SIZE_MB);
	ram_resource_kb(dev, 0, (uintptr_t)_dram / KiB, dram_mb_detected * MiB / KiB);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
