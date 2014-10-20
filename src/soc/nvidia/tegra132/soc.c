/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 * Copyright 2014 Google Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
#include <arch/cache.h>
#include <arch/spintable.h>
#include <cpu/cpu.h>
#include <cbmem.h>
#include <console/console.h>
#include <device/device.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/cpu.h>
#include <soc/nvidia/tegra/apbmisc.h>
#include <string.h>
#include <timer.h>
#include <vendorcode/google/chromeos/chromeos.h>

#include "chip.h"

static void soc_read_resources(device_t dev)
{
	unsigned long index = 0;
	int i; uintptr_t begin, end;
	size_t size;

	for (i = 0; i < CARVEOUT_NUM; i++) {
		carveout_range(i, &begin, &size);
		if (size == 0)
			continue;
		reserved_ram_resource(dev, index++, begin * KiB, size * KiB);
	}

	memory_in_range_below_4gb(&begin, &end);
	size = end - begin;
	ram_resource(dev, index++, begin * KiB, size * KiB);

	memory_in_range_above_4gb(&begin, &end);
	size = end - begin;
	ram_resource(dev, index++, begin * KiB, size * KiB);
}

static size_t cntrl_total_cpus(void)
{
	return CONFIG_MAX_CPUS;
}

static int cntrl_start_cpu(unsigned int id, void (*entry)(void))
{
	if (id != 1)
		return -1;
	start_cpu(1, entry);
	return 0;
}

static struct cpu_control_ops cntrl_ops = {
	.total_cpus = cntrl_total_cpus,
	.start_cpu = cntrl_start_cpu,
};

static void soc_init(device_t dev)
{
	struct soc_nvidia_tegra132_config *cfg;

	clock_init_arm_generic_timer();

	cfg = dev->chip_info;
	spintable_init((void *)cfg->spintable_addr);
	arch_initialize_cpus(dev, &cntrl_ops);
}

static void soc_noop(device_t dev)
{
}

static struct device_operations soc_ops = {
	.read_resources   = soc_read_resources,
	.set_resources    = soc_noop,
	.enable_resources = soc_noop,
	.init             = soc_init,
	.scan_bus         = NULL,
};

static void enable_tegra132_dev(device_t dev)
{
	if (dev->path.type == DEVICE_PATH_CPU_CLUSTER)
		dev->ops = &soc_ops;
}

static void tegra132_init(void *chip_info)
{
	struct tegra_revision rev;

	tegra_revision_info(&rev);

	printk(BIOS_INFO, "chip %x rev %02x.%x\n",
		rev.chip_id, rev.major, rev.minor);

	printk(BIOS_INFO, "MTS build %u\n", raw_read_aidr_el1());
}

struct chip_operations soc_nvidia_tegra132_ops = {
	CHIP_NAME("SOC Nvidia Tegra132")
	.init = tegra132_init,
	.enable_dev = enable_tegra132_dev,
};

static void tegra132_cpu_init(device_t cpu)
{
}

static const struct cpu_device_id ids[] = {
	{ 0x4e0f0000 },
	{ CPU_ID_END },
};

static struct device_operations cpu_dev_ops = {
	.init = tegra132_cpu_init,
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = ids,
};
