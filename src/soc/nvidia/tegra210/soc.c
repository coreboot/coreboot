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
 * Foundation, Inc.
 */

#include <arch/io.h>
#include <arch/cache.h>
#include <arch/spintable.h>
#include <cpu/cpu.h>
#include <bootmode.h>
#include <bootstate.h>
#include <cbmem.h>
#include <console/console.h>
#include <device/device.h>
#include <soc/nvidia/tegra/dc.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/cpu.h>
#include <soc/mc.h>
#include <soc/mtc.h>
#include <soc/nvidia/tegra/apbmisc.h>
#include <string.h>
#include <timer.h>
#include <soc/sdram.h>
#include <soc/sdram_configs.h>

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
	if (id >= CONFIG_MAX_CPUS)
		return -1;
	start_cpu(id, entry);
	return 0;
}

static struct cpu_control_ops cntrl_ops = {
	.total_cpus = cntrl_total_cpus,
	.start_cpu = cntrl_start_cpu,
};


static void soc_init(device_t dev)
{
	struct soc_nvidia_tegra210_config *cfg;

	clock_init_arm_generic_timer();

	cfg = dev->chip_info;
	spintable_init((void *)cfg->spintable_addr);
	arch_initialize_cpus(dev, &cntrl_ops);

	if (!IS_ENABLED(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT))
		return;

	if (display_init_required())
		display_startup(dev);
	else
		printk(BIOS_INFO, "Skipping display init.\n");
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

static void enable_tegra210_dev(device_t dev)
{
	if (dev->path.type == DEVICE_PATH_CPU_CLUSTER)
		dev->ops = &soc_ops;
}

static void tegra210_init(void *chip_info)
{
	struct tegra_revision rev;

	tegra_revision_info(&rev);

	printk(BIOS_INFO, "chip %x rev %02x.%x\n",
		rev.chip_id, rev.major, rev.minor);

	/* Save sdram parameters to scratch regs to be used in LP0 resume */
	sdram_lp0_save_params(get_sdram_config());
	printk(BIOS_INFO, "sdram params saved.\n");
}

struct chip_operations soc_nvidia_tegra210_ops = {
	CHIP_NAME("SOC Nvidia Tegra210")
	.init = tegra210_init,
	.enable_dev = enable_tegra210_dev,
};

static void tegra210_cpu_init(device_t cpu)
{
	if (cpu_is_bsp())
		if (tegra210_run_mtc() != 0)
			printk(BIOS_ERR, "MTC: No training data.\n");
}

static const struct cpu_device_id ids[] = {
	{ 0x411fd071 },
	{ CPU_ID_END },
};

static struct device_operations cpu_dev_ops = {
	.init = tegra210_cpu_init,
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = ids,
};

static void enable_plld(void *unused)
{
	/*
	 * Configure a conservative 300MHz clock for PLLD. The kernel cannot
	 * handle PLLD not being configured so enable PLLD unconditionally
	 * with a default clock rate.
	 */
	clock_configure_plld(300 * MHz);
}

/*
 * The PLLD being enabled is done at BS_DEV_INIT  time because mainboard_init()
 * is the first thing called. This ensures PLLD is up and functional before
 * anything that mainboard can do that implicitly relies on PLLD.
 */
BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_ENTRY, enable_plld, NULL);
