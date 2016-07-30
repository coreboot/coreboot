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
 */

#include <arch/hlt.h>
#include <arch/io.h>
#include <reset.h>

/* Reset control port */
#define RST_CNT			0xcf9
#define FULL_RST		(1 << 3)
#define RST_CPU			(1 << 2)
#define SYS_RST			(1 << 1)

#ifdef __ROMCC__
#define WEAK
#else
#define WEAK __attribute__((weak))
#endif

void WEAK reset_prepare(void) { /* do nothing */ }

#if IS_ENABLED(CONFIG_HAVE_HARD_RESET)
void hard_reset(void)
{
	reset_prepare();
	/* S0->S5->S0 trip. */
	outb(RST_CPU | SYS_RST | FULL_RST, RST_CNT);
	while (1)
		hlt();
}
#endif

void soft_reset(void)
{
	reset_prepare();
	/* PMC_PLTRST# asserted. */
	outb(RST_CPU | SYS_RST, RST_CNT);
	while (1)
		hlt();
}

void cpu_reset(void)
{
	reset_prepare();
	/* Sends INIT# to CPU */
	outb(RST_CPU, RST_CNT);
	while (1)
		hlt();
}
