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

#include <string.h>
#include <console/console.h>
#include <device/device.h>
#include <arch/io.h>
#include <arch/cache.h>
#include <cpu/cpu.h>
#include <cbmem.h>
#include <timer.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/cpu.h>
#include <soc/ramstage.h>
#include <soc/nvidia/tegra/apbmisc.h>
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

	/*
	 * TODO: Frame buffer needs to handled as a carveout from the below_4G
	 * uintptr_t framebuffer_begin = framebuffer_attributes(&framebuffer_size);
	 */


	memory_in_range_below_4gb(&begin, &end);
	size = end - begin;
	ram_resource(dev, index++, begin * KiB, size * KiB);

	memory_in_range_above_4gb(&begin, &end);
	size = end - begin;
	ram_resource(dev, index++, begin * KiB, size * KiB);
}

static void *spintable_entry;
static uint64_t * const spintable_magic = (void *)(uintptr_t)0x80000008;

static void spintable_init(void)
{
	extern void __wait_for_spin_table_request(void);
	const size_t spintable_entry_size = 4096;

	spintable_entry =
		cbmem_add(0x11111111, spintable_entry_size);

	memcpy(spintable_entry, __wait_for_spin_table_request,
		spintable_entry_size);

	/* Ensure the memory location is zero'd out. */
	*spintable_magic = 0;

	dcache_clean_invalidate_by_mva(spintable_magic,
					sizeof(*spintable_magic));
	dcache_clean_invalidate_by_mva(&spintable_entry,
					sizeof(spintable_entry));
	dcache_clean_invalidate_by_mva(spintable_entry, spintable_entry_size);
	dsb();
}

static void spintable_wait(void *monitor_address)
{
	uint32_t sctlr_el2;
	uint32_t spsr_el3;
	uint32_t scr_el3;

	sctlr_el2 = raw_read_sctlr_el2();
	/* Make sure EL2 is in little endian without any caching enabled. */
	sctlr_el2 &= ~(1 << 25);
	sctlr_el2 &= ~(1 << 19);
	sctlr_el2 &= ~(1 << 12);
	sctlr_el2 &= ~0xf;
	raw_write_sctlr_el2(sctlr_el2);
	/* Ensure enter into EL2t with interrupts disabled. */
	spsr_el3 = (1 << 9) | (0xf << 6) | (1 << 3);
	raw_write_spsr_el3(spsr_el3);
	raw_write_elr_el3((uintptr_t)spintable_entry);
	/*
	 * Lower exception level is 64 bit. HVC and SMC allowed. EL0 and EL1
	 * in non-secure mode. No interrupts routed to EL3.
	 */
	scr_el3 = raw_read_scr_el3();
	scr_el3 |= (1 << 10) | (1 << 8) | (0x3 << 4) | (1 << 0);
	scr_el3 &= ~((0x7 << 1) | (1 << 7) | (1 << 9) | (1 << 13) | (1 << 12));
	raw_write_scr_el3(scr_el3);
	isb();
	asm volatile(
		"mov	x0, %0\n\t"
		"eret\n\t" : : "r" (monitor_address) : "x0" );
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
	struct cpu_action action = {
		.run = spintable_wait,
		.arg = spintable_magic,
	};

	clock_init_arm_generic_timer();

	spintable_init();
	arch_initialize_cpus(dev, &cntrl_ops);
	arch_run_on_cpu_async(1, &action);
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
	gic_init();
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
