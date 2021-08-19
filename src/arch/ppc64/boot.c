/* SPDX-License-Identifier: GPL-2.0-only */

#include <program_loading.h>

#if ENV_PAYLOAD_LOADER

/*
 * Payload's entry point is an offset to the real entry point, not to OPD
 * (Official Procedure Descriptor) for entry point.
 */
void arch_prog_run(struct prog *prog)
{
	asm volatile(
	    "mtctr %1\n"
	    "mr 3, %0\n"
	    "bctr\n"
	    :: "r"(prog_entry_arg(prog)), "r"(prog_entry(prog)) : "memory");
}

#else

void arch_prog_run(struct prog *prog)
{
	void (*doit)(void *) = prog_entry(prog);

	doit(prog_entry_arg(prog));
}

#endif
