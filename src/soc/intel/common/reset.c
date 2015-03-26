/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/hlt.h>
#include <arch/io.h>
#include <reset.h>

/* Reset control port */
#define RST_CNT			0xcf9
#define FULL_RST		(1 << 3)
#define RST_CPU			(1 << 2)
#define SYS_RST			(1 << 1)

void hard_reset(void)
{
	/* S0->S5->S0 trip. */
	outb(RST_CPU | SYS_RST | FULL_RST, RST_CNT);
	while (1)
		hlt();
}

void soft_reset(void)
{
	/* PMC_PLTRST# asserted. */
	outb(RST_CPU | SYS_RST, RST_CNT);
	while (1)
		hlt();
}

void cpu_reset(void)
{
	/* Sends INIT# to CPU */
	outb(RST_CPU, RST_CNT);
	while (1)
		hlt();
}
