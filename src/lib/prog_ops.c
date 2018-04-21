/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Imagination Technologies
 * Copyright 2015 Google Inc.
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

#include <compiler.h>
#include <program_loading.h>

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
