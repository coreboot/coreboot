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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/cache.h>
#include <arch/lib_helpers.h>
#include <arch/secmon.h>
#include <arch/stages.h>
#include <arch/spintable.h>
#include <arch/transition.h>
#include <console/console.h>
#include <program_loading.h>
#include <rules.h>
#include <string.h>

static void run_payload(struct prog *prog)
{
	void (*doit)(void *);
	void *arg;

	doit = prog_entry(prog);
	arg = prog_entry_arg(prog);

	uint8_t current_el = get_current_el();

	printk(BIOS_SPEW, "entry    = %p\n", doit);

	secmon_run(doit, arg);

	/* Start the other CPUs spinning. */
	spintable_start();

	/* If current EL is not EL3, jump to payload at same EL. */
	if (current_el != EL3) {
		cache_sync_instructions();
		/* Point of no-return */
		doit(arg);
	}

	/* If current EL is EL3, we transition to payload in EL2. */
	struct exc_state exc_state;

	memset(&exc_state, 0, sizeof(exc_state));

	exc_state.elx.spsr = get_eret_el(EL2, SPSR_USE_L);

	cache_sync_instructions();
	transition_with_entry(doit, arg, &exc_state);
}

void arch_prog_run(struct prog *prog)
{
	void (*doit)(void *);
	void *arg;

	if (ENV_RAMSTAGE && prog->type == PROG_PAYLOAD)
		run_payload(prog);
		return;

	doit = prog_entry(prog);
	arg = prog_entry_arg(prog);

	doit(prog_entry_arg(prog));
}
