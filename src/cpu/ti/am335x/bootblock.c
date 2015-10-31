/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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

#include <types.h>

#include <arch/cache.h>
#include <bootblock_common.h>

void bootblock_soc_init(void)
{
	uint32_t sctlr;

	/* enable dcache */
	sctlr = read_sctlr();
	sctlr |= SCTLR_C;
	write_sctlr(sctlr);
}
