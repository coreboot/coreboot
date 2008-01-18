/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Luis Correia <luis.f.correia@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#define ASSEMBLY 1

#include <stdint.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include "arch/romcc_io.h"
#include "pc80/mc146818rtc_early.c"

static unsigned long main(unsigned long bist)
{
	/* This is the primary cpu how should I boot? */
	if (do_normal_boot()) {
		goto normal_image;
	}
	else {
		goto fallback_image;
	}
 normal_image:
	asm volatile ("jmp __normal_image" 
		: /* outputs */ 
		: "a" (bist) /* inputs */
		: /* clobbers */
		);
 cpu_reset:
	asm volatile ("jmp __cpu_reset"
		: /* outputs */ 
		: "a"(bist) /* inputs */
		: /* clobbers */
		);
 fallback_image:
	return bist;
}
