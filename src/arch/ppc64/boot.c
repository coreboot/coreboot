/* SPDX-License-Identifier: GPL-2.0-only */

#include <program_loading.h>

void arch_prog_run(struct prog *prog)
{
	void (*doit)(void *) = prog_entry(prog);

	doit(prog_entry_arg(prog));
}
