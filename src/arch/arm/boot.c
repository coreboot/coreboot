/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cache.h>
#include <program_loading.h>

void arch_prog_run(struct prog *prog)
{
	void (*doit)(void *);

	cache_sync_instructions();

	doit = prog_entry(prog);
	doit(prog_entry_arg(prog));
}
