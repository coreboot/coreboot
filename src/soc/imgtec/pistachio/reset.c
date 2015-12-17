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

#include <arch/io.h>
#include <console/console.h>
#include <reset.h>

#define PISTACHIO_WD_ADDR		0xB8102100
#define PISTACHIO_WD_SW_RST_OFFSET	0x0000

void hard_reset(void)
{
	/* Generate system reset */
	write32(PISTACHIO_WD_ADDR + PISTACHIO_WD_SW_RST_OFFSET, 0x1);
}
