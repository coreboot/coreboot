/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <main_decl.h>

/* Provide an entry point for verstage when it's a separate stage. */
asmlinkage void car_stage_entry(void)
{
	main();
}
