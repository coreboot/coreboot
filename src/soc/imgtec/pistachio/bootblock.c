/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Imagination Technologies
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/cpu.h>
#include <arch/mmu.h>
#include <assert.h>
#include <stdint.h>
#include <symbols.h>

static void bootblock_cpu_init(void)
{
	uint32_t cause;

	/*
	 * Make sure the count register is counting by clearing the "Disable
	 * Counter" bit, in case it is set.
	 */
	cause = read_c0_cause();
	if (cause & C0_CAUSE_DC)
		write_c0_cause(cause & ~(C0_CAUSE_DC));

	/* And make sure that it starts from zero. */
	write_c0_count(0);
}

static void bootblock_mmu_init(void)
{
	uint32_t null_guard_size =  1 * MiB;
	uint32_t dram_base, dram_size;

	write_c0_wired(0);

	dram_base = (uint32_t)_dram;
	dram_size = CONFIG_DRAM_SIZE_MB * MiB;

	/*
	 * To be able to catch NULL pointer dereference attempts, lets not map
	 * memory close to zero.
	 */
	if (dram_base < null_guard_size) {
		dram_base += null_guard_size;
		dram_size -= null_guard_size;
	}
	assert(!identity_map((uint32_t)_sram, _sram_size,
						C0_ENTRYLO_COHERENCY_WB));
	assert(!identity_map(dram_base, dram_size, C0_ENTRYLO_COHERENCY_WB));
	assert(!identity_map((uint32_t)_soc_registers, _soc_registers_size,
                                                C0_ENTRYLO_COHERENCY_UC));
}
