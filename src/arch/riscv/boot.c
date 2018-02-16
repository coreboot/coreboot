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
#include <console/console.h>

void arch_prog_run(struct prog *prog)
{
	void (*doit)(void *) = prog_entry(prog);
	void riscvpayload(const char *configstring, void *payload);
	const char *config = NULL;

	if (ENV_RAMSTAGE && prog_type(prog) == PROG_PAYLOAD) {
		printk(BIOS_SPEW, "Config string: '%s'\n", config);
		printk(BIOS_SPEW, "OK, let's go\n");
		riscvpayload(config, doit);
	}

	doit(prog_entry_arg(prog));
}

int arch_supports_bounce_buffer(void)
{
	return 0;
}
