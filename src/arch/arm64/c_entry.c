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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/cache.h>
#include <arch/cpu.h>
#include <arch/exception.h>
#include <arch/mmu.h>
#include <arch/stages.h>
#include "cpu-internal.h"

void __attribute__((weak)) arm64_soc_init(void)
{
	/* Default weak implementation does nothing. */
}

static void seed_stack(void)
{
	char *stack_begin;
	uint64_t *slot;
	int i;
	int size;

	stack_begin = cpu_get_stack(smp_processor_id());
	stack_begin -= CONFIG_STACK_SIZE;
	slot = (void *)stack_begin;

	/* Pad out 256 bytes for current usage. */
	size = CONFIG_STACK_SIZE - 256;
	size /= sizeof(*slot);
	for (i = 0; i < size; i++)
		*slot++ = 0xdeadbeefdeadbeefULL;
}

static void arm64_init(void)
{
	cpu_set_bsp();
	seed_stack();
	arm64_soc_init();
	main();
}

static void secondary_cpu_start(void)
{
	mmu_enable();
	exception_hwinit();

	/* This will never return. */
	arch_secondary_cpu_init();
}

/*
 * This variable holds entry point for CPUs starting up. The first
 * element is the BSP path, and the second is the non-BSP path.
 */
void (*c_entry[2])(void) = { &arm64_init, &secondary_cpu_start };

void *prepare_secondary_cpu_startup(void)
{
	return secondary_entry_point(&arm64_cpu_startup);
}
