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
#include <stddef.h>

static const u8 type_to_signal[] = {
	[EXC_DE]  = GDB_SIGFPE,
	[EXC_DB]  = GDB_SIGTRAP,
	[EXC_NMI] = GDB_SIGKILL,
	[EXC_BP]  = GDB_SIGTRAP,
	[EXC_OF]  = GDB_SIGFPE,
	[EXC_BR]  = GDB_SIGSEGV,
	[EXC_UD]  = GDB_SIGILL,
	[EXC_NM]  = GDB_SIGEMT,
	[EXC_DF]  = GDB_SIGKILL,
	[EXC_TS]  = GDB_SIGSEGV,
	[EXC_NP]  = GDB_SIGSEGV,
	[EXC_SS]  = GDB_SIGBUS,
	[EXC_GP]  = GDB_SIGSEGV,
	[EXC_PF]  = GDB_SIGSEGV,
	[EXC_MF]  = GDB_SIGEMT,
	[EXC_AC]  = GDB_SIGBUS,
	[EXC_MC]  = GDB_SIGKILL,
	[EXC_XF]  = GDB_SIGFPE,
	[EXC_SX]  = GDB_SIGFPE,
};

static void gdb_exception_hook(u8 vector)
{
	gdb_command_loop(type_to_signal[vector]);
}

void gdb_arch_init(void)
{
	for (int vector = 0; vector < ARRAY_SIZE(type_to_signal); ++vector) {
		if (type_to_signal[vector])
			set_interrupt_handler(vector, &gdb_exception_hook);
	}
}

void gdb_arch_enter(void)
{
	u8 *stack_pointer;
#if CONFIG(LP_ARCH_X86_64)
	asm volatile ("movq %%rsp, %0" : "=r"(stack_pointer));
#else
	asm volatile ("mov %%esp, %0" : "=r"(stack_pointer));
#endif

	/* Avoid reentrant exceptions, just call the hook if in one already. */
	if (stack_pointer >= exception_stack && stack_pointer <= exception_stack_end)
		gdb_exception_hook(EXC_BP);
	else
		asm volatile ("int3");
}

int gdb_arch_set_single_step(int on)
{
	const size_t tf_bit = 1 << 8;

	if (on)
		exception_state->regs.reg_flags |= tf_bit;
	else
		exception_state->regs.reg_flags &= ~tf_bit;

	return 0;
}

void gdb_arch_encode_regs(struct gdb_message *message)
{
	gdb_message_encode_bytes(message, &exception_state->regs,
				 sizeof(exception_state->regs));
}

void gdb_arch_decode_regs(int offset, struct gdb_message *message)
{
	gdb_message_decode_bytes(message, offset, &exception_state->regs,
				 sizeof(exception_state->regs));
}
