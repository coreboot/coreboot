/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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

#include <program_loading.h>
#include <vm.h>
#include <arch/boot.h>
#include <arch/encoding.h>
#include <rules.h>
#include <console/console.h>

/*
 * A pointer to the Flattened Device Tree passed to coreboot by the boot ROM.
 * Presumably this FDT is also in ROM.
 *
 * This pointer is only used in ramstage!
 */
const void *rom_fdt;

void arch_prog_run(struct prog *prog)
{
	void (*doit)(void *) = prog_entry(prog);
	void riscvpayload(const void *fdt, void *payload);

	if (ENV_RAMSTAGE && prog_type(prog) == PROG_PAYLOAD) {
		/*
		 * FIXME: This is wrong and will crash. Linux can't (in early
		 * boot) access memory that's before its own loading address.
		 * We need to copy the FDT to a place where Linux can access it.
		 */
		const void *fdt = rom_fdt;

		printk(BIOS_SPEW, "FDT is at %p\n", fdt);
		printk(BIOS_SPEW, "OK, let's go\n");
		riscvpayload(fdt, doit);
	}

	doit(prog_entry_arg(prog));
}

int arch_supports_bounce_buffer(void)
{
	return 0;
}
