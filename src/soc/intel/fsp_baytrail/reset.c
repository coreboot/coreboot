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

#include <arch/io.h>
#include <soc/pmc.h>
#include <soc/reset.h>

void cold_reset(void)
{
	/* S0->S5->S0 trip. */
	outb(RST_CPU | SYS_RST | FULL_RST, RST_CNT);
}

void warm_reset(void)
{
	/* PMC_PLTRST# asserted. */
	outb(RST_CPU | SYS_RST, RST_CNT);
}

void soft_reset(void)
{
	/* Sends INIT# to CPU */
	outb(RST_CPU, RST_CNT);
}

void hard_reset(void)
{
	/* Don't power cycle on hard_reset(). It's not really clear what the
	 * semantics should be for the meaning of hard_reset(). */
	warm_reset();
}
