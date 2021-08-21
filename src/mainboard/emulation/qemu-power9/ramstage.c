/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/power/spr.h>
#include <program_loading.h>

/*
 * Payload's entry point is an offset to the real entry point, not to OPD
 * (Official Procedure Descriptor) for entry point.
 *
 * Also pass FDT address to payload stored in SPR_HSPRG0 by bootblock.
 */
void platform_prog_run(struct prog *prog)
{
	asm volatile(
	    "mfspr %%r27, %0\n" /* pass pointer to FDT */
	    "mtctr %2\n"
	    "mr 3, %1\n"
	    "bctr\n"
	    :: "i"(SPR_HSPRG0), "r"(prog_entry_arg(prog)), "r"(prog_entry(prog))
	    : "memory");
}
