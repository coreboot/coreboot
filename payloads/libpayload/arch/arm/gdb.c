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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <exception.h>
#include <gdb.h>
#include <libpayload.h>

struct gdb_regs
{
	u32 r[16];
	struct fp_reg
	{
		u8 byte[12];
	} __attribute__((packed)) f[8];
	u32 fps;
	u32 cpsr;
} __attribute__((packed));

static const u8 type_to_signal[] = {
	[EXC_UNDEF]  = GDB_SIGILL,
	[EXC_SWI]    = GDB_SIGTRAP,
	[EXC_PABORT] = GDB_SIGSEGV,
	[EXC_DABORT] = GDB_SIGSEGV,
};

/* Scratch value to write reentrant exception states to. We never read it. */
static struct exception_state sentinel_exception_state;

static int gdb_exception_hook(u32 type)
{
	/*
	 * If we were not resumed we are in deep trouble here. GDB probably told
	 * us to do something stupid and caused a reentrant exception. All we
	 * can do is just blindly send an error code and keep going. Eventually
	 * GDB will tell us to resume and we return right back to the original
	 * exception state ("jumping over" all the nested ones).
	 */
	if (gdb_state.connected && !gdb_state.resumed) {
		static const char error_code[] = "E22";	/* EINVAL? */
		static const struct gdb_message tmp_reply = {
			.buf = (u8 *)error_code,
			.used = sizeof(error_code),
			.size = sizeof(error_code),
		};
		gdb_send_reply(&tmp_reply);
		gdb_command_loop(gdb_state.signal); /* preserve old signal */
	} else {
		if (type >= ARRAY_SIZE(type_to_signal) || !type_to_signal[type])
			return 0;
		exception_state_ptr = &sentinel_exception_state;
		gdb_command_loop(type_to_signal[type]);
	}

	exception_state_ptr = &exception_state;
	return 1;
}

void gdb_arch_init(void)
{
	exception_install_hook(&gdb_exception_hook);
}

void gdb_arch_enter(void)
{
	u32 *sp;

	asm volatile ("mov %0, %%sp" : "=r"(sp) );

	/* Avoid reentrant exceptions, just call the hook if in one already. */
	if (sp >= exception_stack && sp <= exception_stack_end)
		gdb_exception_hook(EXC_SWI);
	else
		asm volatile ("svc #0");
}

int gdb_arch_set_single_step(int on)
{
	/* GDB seems to only need this on x86, ARM works fine without it. */
	return -1;
}

void gdb_arch_encode_regs(struct gdb_message *message)
{
	gdb_message_encode_bytes(message, exception_state.regs,
				 sizeof(exception_state.regs));
	gdb_message_encode_zero_bytes(message,
		offsetof(struct gdb_regs, cpsr) - offsetof(struct gdb_regs, f));
	gdb_message_encode_bytes(message, &exception_state.cpsr,
				 sizeof(exception_state.cpsr));
}

void gdb_arch_decode_regs(int offset, struct gdb_message *message)
{
	const int cpsr_hex_offset = offsetof(struct gdb_regs, cpsr) * 2;
	gdb_message_decode_bytes(message, offset,
			exception_state.regs, sizeof(exception_state.regs));
	gdb_message_decode_bytes(message, offset + cpsr_hex_offset,
			&exception_state.cpsr, sizeof(exception_state.cpsr));
}
