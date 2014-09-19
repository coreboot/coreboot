/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 *
 * secmon_loader.c: Responsible for loading the rmodule, providing entry point
 * and parameter location for the rmodule.
 */

#include <arch/lib_helpers.h>
#include <arch/secmon.h>
#include <arch/spintable.h>
#include <arch/stages.h>
#include <console/console.h>
#include <rmodule.h>
#include <string.h>

/* SECMON entry point encoded as an rmodule */
extern unsigned char _binary_secmon_start[];

typedef void (*secmon_entry_t)(struct secmon_params *);

void __attribute__((weak)) soc_get_secmon_base_size(uint64_t *secmon_base, size_t *secmon_size)
{
	/* Default weak implementation initializes to 0 */
	*secmon_base = 0;
	*secmon_size = 0;
}

static secmon_entry_t secmon_load_rmodule(void)
{
	struct rmodule secmon_mod;
	uint64_t secmon_base;
	size_t secmon_size;

	/* Get base address and size of the area available for secure monitor
	 * rmodule.
	 */
	soc_get_secmon_base_size(&secmon_base, &secmon_size);

	if ((secmon_base == 0) || (secmon_size == 0)) {
		printk(BIOS_ERR, "ARM64: secmon_base / secmon_size invalid\n");
		return NULL;
	}

	printk(BIOS_DEBUG,"secmon_base:%lx,secmon_size:%lx\n",
	       (unsigned long)secmon_base, (unsigned long)secmon_size);

	/* Fail if can't parse secmon module */
	if (rmodule_parse(&_binary_secmon_start, &secmon_mod)) {
		printk(BIOS_ERR, "ARM64: secmon_mod not found\n");
		return NULL;
	}

	/* Load rmodule at secmon_base */
	if (rmodule_load((void *)secmon_base, &secmon_mod)) {
		printk(BIOS_ERR, "ARM64:secmon_mod cannot load\n");
		return NULL;
	}

	/* Identify the entry point for secure monitor */
	return rmodule_entry(&secmon_mod);
}

struct secmon_runit {
	secmon_entry_t entry;
	struct secmon_params bsp_params;
	struct secmon_params secondary_params;
};

static void secmon_start(void *arg)
{
	uint32_t scr;
	secmon_entry_t entry;
	struct secmon_params *p = NULL;
	struct secmon_runit *r = arg;

	entry = r->entry;

	if (cpu_is_bsp())
		p = &r->bsp_params;
	else {
		entry = secondary_entry_point(entry);
		if (r->secondary_params.entry != NULL)
			p = &r->secondary_params;
	}

	printk(BIOS_DEBUG, "CPU%x entering secure monitor %p.\n",
		cpu_info()->id, entry);

	/* We want to enforce the following policies:
	 * NS bit is set for lower EL
	 */
	scr = raw_read_scr_el3();
	scr |= SCR_NS;
	raw_write_scr_el3(scr);

	entry(p);
}

void secmon_run(void (*entry)(void *), void *cb_tables)
{
	const struct spintable_attributes *spin_attrs;
	static struct secmon_runit runit;
	struct cpu_action action = {
		.run = secmon_start,
		.arg = &runit,
	};

	printk(BIOS_SPEW, "payload jump @ %p\n", entry);

	if (get_current_el() != EL3) {
		printk(BIOS_DEBUG, "Secmon Error: Can only be loaded in EL3\n");
		return;
	}

	runit.entry = secmon_load_rmodule();

	if (runit.entry == NULL)
		die("ARM64 Error: secmon load error");

	printk(BIOS_DEBUG, "ARM64: Loaded the el3 monitor...jumping to %p\n",
	       runit.entry);

	runit.bsp_params.entry = entry;
	runit.bsp_params.arg = cb_tables;

	spin_attrs = spintable_get_attributes();

	if (spin_attrs != NULL) {
		runit.secondary_params.entry = spin_attrs->entry;
		runit.secondary_params.arg = spin_attrs->addr;
	}

	arch_run_on_all_cpus_but_self_async(&action);
	secmon_start(&runit);
}
