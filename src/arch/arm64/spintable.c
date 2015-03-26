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
 * Foundation, Inc.
 */

#include <arch/cache.h>
#include <arch/spintable.h>
#include <arch/transition.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <cbmem.h>
#include <string.h>

static struct spintable_attributes spin_attrs;

void spintable_init(void *monitor_address)
{
	extern void __wait_for_spin_table_request(void);
	const size_t code_size = 4096;

	if (monitor_address == NULL) {
		printk(BIOS_ERR, "spintable: NULL address to monitor.\n");
		return;
	}

	spin_attrs.entry = cbmem_add(CBMEM_ID_SPINTABLE, code_size);

	if (spin_attrs.entry == NULL)
		return;

	spin_attrs.addr = monitor_address;

	printk(BIOS_INFO, "spintable @ %p will monitor %p\n",
		spin_attrs.entry, spin_attrs.addr);

	/* Ensure the memory location is zero'd out. */
	*(uint64_t *)monitor_address = 0;

	memcpy(spin_attrs.entry, __wait_for_spin_table_request, code_size);

	dcache_clean_invalidate_by_mva(monitor_address, sizeof(uint64_t));
	dcache_clean_invalidate_by_mva(spin_attrs.entry, code_size);
}

static void spintable_enter(void *unused)
{
	struct exc_state state;
	const struct spintable_attributes *attrs;
	int current_el;

	attrs = spintable_get_attributes();

	current_el = get_current_el();

	if (current_el != EL3)
		attrs->entry(attrs->addr);

	memset(&state, 0, sizeof(state));
	state.elx.spsr = get_eret_el(EL2, SPSR_USE_L);

	transition_with_entry(attrs->entry, attrs->addr, &state);
}

const struct spintable_attributes *spintable_get_attributes(void)
{
	if (spin_attrs.entry == NULL) {
		printk(BIOS_ERR, "spintable: monitor code not present.\n");
		return NULL;
	}

	return &spin_attrs;
}

void spintable_start(void)
{
	struct cpu_action action = {
		.run = spintable_enter,
	};

	if (spintable_get_attributes() == NULL)
		return;

	printk(BIOS_INFO, "All non-boot CPUs to enter spintable.\n");

	arch_run_on_all_cpus_but_self_async(&action);
}
