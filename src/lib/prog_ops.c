/* SPDX-License-Identifier: GPL-2.0-only */

#include <program_loading.h>
#include <types.h>

/* For each segment of a program loaded this function is called*/
void prog_segment_loaded(uintptr_t start, size_t size, int flags)
{
	platform_segment_loaded(start, size, flags);
	arch_segment_loaded(start, size, flags);
}

void __weak platform_segment_loaded(uintptr_t start,
							size_t size, int flags)
{
	/* do nothing */
}

void __weak arch_segment_loaded(uintptr_t start, size_t size,
						int flags)
{
	/* do nothing */
}

void prog_run(struct prog *prog)
{
	platform_prog_run(prog);
	arch_prog_run(prog);
}

void __weak platform_prog_run(struct prog *prog)
{
	/* do nothing */
}
