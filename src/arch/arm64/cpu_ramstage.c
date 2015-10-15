/*
 * This file is part of the coreboot project.
 *
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
 */

#include <stdint.h>
#include <stdlib.h>
#include <arch/cache.h>
#include <arch/cpu.h>
#include <arch/lib_helpers.h>
#include <cpu/cpu.h>
#include <console/console.h>
#include <gic.h>
#include <timer.h>
#include "cpu-internal.h"

static struct cpu_info cpu_info;

void __attribute__((weak)) arm64_arch_timer_init(void)
{
	/* Default weak implementation does nothing. */
}

static inline void cpu_disable_dev(device_t dev)
{
	dev->enabled = 0;
}

static struct cpu_driver *locate_cpu_driver(uint32_t midr)
{
	struct cpu_driver *cur;

	for (cur = _cpu_drivers; cur != _ecpu_drivers; cur++) {
		const struct cpu_device_id *id_table = cur->id_table;

		for (; id_table->midr != CPU_ID_END; id_table++) {
			if (id_table->midr == midr)
				return cur;
		}
	}
	return NULL;
}

static int cpu_set_device_operations(device_t dev)
{
	uint32_t midr;
	struct cpu_driver *driver;

	midr = raw_read_midr_el1();
	driver = locate_cpu_driver(midr);

	if (driver == NULL) {
		printk(BIOS_WARNING, "No CPU driver for MIDR %08x\n", midr);
		return -1;
	}
	dev->ops = driver->ops;
	return 0;
}

/* Set up default SCR values. */
static void el3_init(void)
{
	uint32_t scr;

	if (get_current_el() != EL3)
		return;

	scr = raw_read_scr_el3();
	/* Default to non-secure EL1 and EL0. */
	scr &= ~(SCR_NS_MASK);
	scr |= SCR_NS_ENABLE;
	/* Disable IRQ, FIQ, and external abort interrupt routing. */
	scr &= ~(SCR_IRQ_MASK | SCR_FIQ_MASK | SCR_EA_MASK);
	scr |= SCR_IRQ_DISABLE | SCR_FIQ_DISABLE | SCR_EA_DISABLE;
	/* Enable HVC */
	scr &= ~(SCR_HVC_MASK);
	scr |= SCR_HVC_ENABLE;
	/* Disable SMC */
	scr &= ~(SCR_SMC_MASK);
	scr |= SCR_SMC_DISABLE;
	/* Disable secure instruction fetches. */
	scr &= ~(SCR_SIF_MASK);
	scr |= SCR_SIF_DISABLE;
	/* All lower exception levels 64-bit by default. */
	scr &= ~(SCR_RW_MASK);
	scr |= SCR_LOWER_AARCH64;
	/* Disable secure EL1 access to secure timer. */
	scr &= ~(SCR_ST_MASK);
	scr |= SCR_ST_DISABLE;
	/* Don't trap on WFE or WFI instructions. */
	scr &= ~(SCR_TWI_MASK | SCR_TWE_MASK);
	scr |= SCR_TWI_DISABLE | SCR_TWE_DISABLE;
	raw_write_scr_el3(scr);
	isb();
}

static void init_this_cpu(void)
{
	struct cpu_info *ci = &cpu_info;
	device_t dev = ci->cpu;

	cpu_set_device_operations(dev);

	printk(BIOS_DEBUG, "CPU%x: MPIDR: %llx\n", ci->id, ci->mpidr);

	/* Initialize the GIC. */
	gic_init();

	/*
	 * Disable coprocessor traps to EL3:
	 * TCPAC [20] = 0, disable traps for EL2 accesses to CPTR_EL2 or HCPTR
	 * and EL2/EL1 access to CPACR_EL1.
	 * TTA [20] = 0, disable traps for trace register access from any EL.
	 * TFP [10] = 0, disable traps for floating-point instructions from any
	 * EL.
	 */
	raw_write_cptr_el3(CPTR_EL3_TCPAC_DISABLE | CPTR_EL3_TTA_DISABLE |
			   CPTR_EL3_TFP_DISABLE);

	/*
	 * Allow FPU accesses:
	 * FPEN [21:20] = 3, disable traps for floating-point instructions from
	 * EL0/EL1.
	 * TTA [28] = 0, disable traps for trace register access from EL0/EL1.
	 */
	raw_write_cpacr_el1(CPACR_TRAP_FP_DISABLE | CPACR_TTA_DISABLE);

	/* Arch Timer init: setup cntfrq per CPU */
	arm64_arch_timer_init();

	if (dev->ops != NULL && dev->ops->init != NULL) {
		dev->initialized = 1;
		printk(BIOS_DEBUG, "%s init\n", dev_path(dev));
		dev->ops->init(dev);
	}
}

/* Fill in cpu_info structures according to device tree. */
static void init_cpu_info(struct bus *bus)
{
	device_t cur;

	for (cur = bus->children; cur != NULL; cur = cur->sibling) {
		struct cpu_info *ci;
		unsigned int id = cur->path.cpu.id;

		if (cur->path.type != DEVICE_PATH_CPU)
			continue;

		/* IDs are currently mapped 1:1 with logical CPU numbers. */
		if (id != 0) {
			printk(BIOS_WARNING,
				"CPU id %x too large. Disabling.\n", id);
			cpu_disable_dev(cur);
			continue;
		}

		ci = &cpu_info;
		if (ci->cpu != NULL) {
			printk(BIOS_WARNING,
				"Duplicate ID %x in device tree.\n", id);
			cpu_disable_dev(cur);
		}

		ci->cpu = cur;
		ci->id = cur->path.cpu.id;
	}
}

void arch_initialize_cpu(device_t cluster)
{
	struct bus *bus;

	if (cluster->path.type != DEVICE_PATH_CPU_CLUSTER) {
		printk(BIOS_ERR,
			"CPU init failed. Device is not a CPU_CLUSTER: %s\n",
			dev_path(cluster));
		return;
	}

	bus = cluster->link_list;

	/* Check if no children under this device. */
	if (bus == NULL)
		return;

	el3_init();

	/* Initialize the cpu_info structures. */
	init_cpu_info(bus);

	/* Send it the init action. */
	init_this_cpu();
}
