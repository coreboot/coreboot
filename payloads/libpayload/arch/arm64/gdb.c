/*
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but without any warranty; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <exception.h>
#include <gdb.h>
#include <libpayload.h>

struct gdb_regs
{
	u64 x[32];
	struct fp_reg
	{
		u64 quad[2];
	} __packed f[32];
	u32 fpcr;
	u32 fpsr;
	u32 spsr;
} __packed;

static int gdb_exception_hook(u32 type)
{
	return -1;
}

void gdb_arch_init(void)
{
	exception_install_hook(&gdb_exception_hook);
}

void gdb_arch_enter(void)
{
}

int gdb_arch_set_single_step(int on)
{
	/* GDB seems to only need this on x86, ARM works fine without it. */
	return -1;
}

void gdb_arch_encode_regs(struct gdb_message *message)
{
}

void gdb_arch_decode_regs(int offset, struct gdb_message *message)
{
}
