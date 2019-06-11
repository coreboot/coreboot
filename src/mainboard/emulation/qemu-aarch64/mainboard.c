/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 Asami Doi <d0iasm.pub@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <cbmem.h>
#include <ramdetect.h>
#include <symbols.h>
#include <device/device.h>

static void mainboard_enable(struct device *dev)
{
	int ram_size_mb = probe_ramsize((uintptr_t)_dram, CONFIG_DRAM_SIZE_MB);
	ram_resource(dev, 0, (uintptr_t)_dram / KiB, ram_size_mb * KiB);
}

struct chip_operations mainboard_ops = {
	.name = "qemu_aarch64",
	.enable_dev = mainboard_enable,
};
