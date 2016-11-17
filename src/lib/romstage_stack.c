/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2016 Intel Corp.
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
#include <cbmem.h>

/*
 * Romstage needs quite a bit of stack for decompressing images since the lzma
 * lib keeps its state on the stack during romstage.
 */
#define ROMSTAGE_RAM_STACK_SIZE 0x5000

uintptr_t romstage_ram_stack_base(size_t size, int src)
{
	/* cbmem_add() does a find() before add(). */
	if (src == ROMSTAGE_STACK_CBMEM)
		return (uintptr_t)cbmem_add(CBMEM_ID_ROMSTAGE_RAM_STACK, size);
	if (src == ROMSTAGE_STACK_LOW_MEM)
		return CONFIG_RAMTOP - size;
	return 0;
}

uintptr_t romstage_ram_stack_bottom(void)
{
	return romstage_ram_stack_base(ROMSTAGE_RAM_STACK_SIZE,
		ROMSTAGE_STACK_CBMEM);
}

uintptr_t romstage_ram_stack_top(void)
{
	uintptr_t stack_top = romstage_ram_stack_base(ROMSTAGE_RAM_STACK_SIZE,
		ROMSTAGE_STACK_CBMEM);
	stack_top += ROMSTAGE_RAM_STACK_SIZE;

	/* Make it aligned to a 8-byte boundary. */
	stack_top = ALIGN_DOWN(stack_top, 8);
	return stack_top;
}
