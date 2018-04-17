/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018       Facebook, Inc.
 * Copyright 2003-2017  Cavium Inc.  <support@cavium.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Derived from Cavium's BSD-3 Clause OCTEONTX-SDK-6.2.0.
 */

#include <bootmode.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <device/device.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/sdram.h>
#include <soc/timer.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <symbols.h>
#include <libbdk-boot/bdk-boot.h>
#include <soc/ecam0.h>

static void soc_read_resources(device_t dev)
{
	ram_resource(dev, 0, (uintptr_t)_dram / KiB, sdram_size_mb() * KiB);
}

static void soc_init(device_t dev)
{
	/* Init ECAM, MDIO, PEM, PHY, QLM ... */
	bdk_boot();

	/* TODO: additional trustzone init */
}

static void soc_final(device_t dev)
{
	watchdog_disable(0);
}

static struct device_operations soc_ops = {
	.read_resources   = soc_read_resources,
	.set_resources    = DEVICE_NOOP,
	.enable_resources = DEVICE_NOOP,
	.init             = soc_init,
	.final            = soc_final,
	.scan_bus         = NULL,
};

static void enable_soc_dev(device_t dev)
{
	if (dev->path.type == DEVICE_PATH_DOMAIN &&
		dev->path.domain.domain == 0) {
		dev->ops = &pci_domain_ops_ecam0;
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &soc_ops;
	}
}

struct chip_operations soc_cavium_cn81xx_ops = {
	CHIP_NAME("SOC Cavium CN81XX")
	.enable_dev = enable_soc_dev,
};
