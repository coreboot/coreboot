/*
 * This file is part of the coreboot project.
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

#include <commonlib/helpers.h>
#include <console/console.h>
#include <program_loading.h>
#include <ip_checksum.h>
#include <symbols.h>

int payload_arch_usable_ram_quirk(uint64_t start, uint64_t size)
{
	if (start < 1 * MiB && (start + size) <= 1 * MiB) {
		printk(BIOS_DEBUG,
			"Payload being loaded at below 1MiB without region being marked as RAM usable.\n");
		return 1;
	}

	return 0;
}

void arch_prog_run(struct prog *prog)
{
#ifdef __x86_64__
    printk(BIOS_DEBUG, "%s is x86_64\n", __func__);
	void (*doit)(void *arg);
    printk(BIOS_DEBUG, "%s after void doit\n", __func__);
#else
    printk(BIOS_DEBUG, "%s is NOT x86_64\n", __func__);
	/* Ensure the argument is pushed on the stack. */
	asmlinkage void (*doit)(void *arg);
    printk(BIOS_DEBUG, "%s after asmlinkage\n", __func__);
#endif
    printk(BIOS_DEBUG, "%s before prog_entry\n", __func__);
	doit = prog_entry(prog);
    printk(BIOS_DEBUG, "%s after prog_entry\n", __func__);
    printk(BIOS_DEBUG, "%s doit pointer: %p\n", __func__, doit);
	doit(prog_entry_arg(prog));
    printk(BIOS_DEBUG, "%s after doit\n", __func__);
}
