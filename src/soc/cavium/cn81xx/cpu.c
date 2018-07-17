/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018-present  Facebook, Inc.
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

#include <types.h>
#include <soc/addressmap.h>
#include <arch/io.h>
#include <soc/cpu.h>
#include <bdk-coreboot.h>
#include <console/console.h>
#include <timer.h>
#include <delay.h>

uint64_t cpu_get_available_core_mask(void)
{
	return read64((void *)RST_PP_AVAILABLE);
}

size_t cpu_get_num_available_cores(void)
{
	return bdk_dpop(cpu_get_available_core_mask());
}

static void (*secondary_c_entry)(size_t core_id);
static size_t secondary_booted;

void secondary_cpu_init(size_t core_id)
{
	write64(&secondary_booted, 1);
	dmb();

	if (secondary_c_entry)
		secondary_c_entry(core_id);
	else
		asm("wfi");
}

size_t cpu_self_get_core_id(void)
{
	u32 mpidr_el1;
	asm("mrs %0, MPIDR_EL1\n\t" : "=r" (mpidr_el1) :: "memory");

	/* Core is 4 bits from AFF0 and rest from AFF1 */
	size_t core_num;
	core_num = mpidr_el1 & 0xf;
	core_num |= (mpidr_el1 & 0xff00) >> 4;

	return core_num;
}

uint64_t cpu_self_get_core_mask(void)
{
	return 1ULL << cpu_self_get_core_id();
}

size_t start_cpu(size_t cpu, void (*entry_64)(size_t core_id))
{
	const uint64_t coremask = 1ULL << cpu;
	struct stopwatch sw;
	uint64_t pending;

	printk(BIOS_DEBUG, "CPU: Starting CPU%zu @ %p.\n", cpu, entry_64);

	/* Core not available */
	if (!(coremask & cpu_get_available_core_mask()))
		return 1;

	/* Only secondary CPUs are supported */
	if (cpu == cpu_self_get_core_id())
		return 1;

	/* Check stack here, instead of in cpu_secondary.S */
	if ((CONFIG_STACK_SIZE * cpu) > _stack_sec_size)
		return 1;

	/* Write the address of the main entry point */
	write64((void *)MIO_BOOT_AP_JUMP, (uintptr_t)secondary_init);

	/* Get coremask of cores in reset */
	const uint64_t reset = read64((void *)RST_PP_RESET);
	printk(BIOS_INFO, "CPU: Cores currently in reset: 0x%llx\n", reset);

	/* Setup entry for secondary core */
	write64(&secondary_c_entry, (uintptr_t)entry_64);
	write64(&secondary_booted, 0);
	dmb();

	printk(BIOS_DEBUG, "CPU: Taking core %zu out of reset.\n", cpu);

	/* Release core from reset */
	write64((void *)RST_PP_RESET, reset & ~coremask);

	/* Wait for cores to finish coming out of reset */
	udelay(1);

	stopwatch_init_usecs_expire(&sw, 1000000);
	do {
		pending = read64((void *)RST_PP_PENDING);
	} while (!stopwatch_expired(&sw) && (pending & coremask));

	if (stopwatch_expired(&sw)) {
		printk(BIOS_ERR, "ERROR: Timeout waiting for reset "
		       "pending to clear.");
		return 1;
	}

	stopwatch_init_usecs_expire(&sw, 1000000);

	printk(BIOS_DEBUG, "CPU: Wait up to 1s for the core to boot...\n");
	while (!stopwatch_expired(&sw) && !read64(&secondary_booted))
		;

	/* Cleanup */
	write64(&secondary_c_entry, 0);
	dmb();

	if (!read64(&secondary_booted)) {
		printk(BIOS_ERR, "ERROR: Core %zu failed to start.\n", cpu);
		return 1;
	}

	printk(BIOS_INFO, "CPU: Core %zu booted\n", cpu);
	return 0;
}
