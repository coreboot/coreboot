/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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

#include <rules.h>

/* We use ELF as output format. So that we can debug the code in some form. */
OUTPUT_ARCH(riscv)

PHDRS
{
	to_load PT_LOAD;
}

#if ENV_BOOTBLOCK || ENV_RAMSTAGE
ENTRY(_start)
#else
ENTRY(stage_entry)
#endif
