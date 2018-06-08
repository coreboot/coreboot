/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Indrek Kruusa <indrek.kruusa@artecdesign.ee>
 * Copyright (C) 2006 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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
 */

#include <arch/io.h>
#include <cpu/amd/lxdef.h>

/**
 *	StartTimer1
 *
 *	Entry: none
 *	Exit: Starts Timer 1 for port 61 use
 *	Destroys: Al,
 */
static void StartTimer1(void)
{
	outb(0x56, 0x43);
	outb(0x12, 0x41);
}

void SystemPreInit(void)
{
	/* they want a jump ... */
	StartTimer1();
}
