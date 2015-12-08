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
#include <arch/encoding.h>
#include <rules.h>

void arch_prog_run(struct prog *prog)
{
	void (*doit)(void *) = prog_entry(prog);

	if (ENV_RAMSTAGE && prog_type(prog) == PROG_PAYLOAD) {
		initVirtualMemory();
		write_csr(mepc, doit);
		asm volatile("eret");
	} else {
		doit(prog_entry_arg(prog));
	}
}
