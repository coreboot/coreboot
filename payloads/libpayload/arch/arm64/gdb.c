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
	u64 x[31];
	u64 sp;		/* shares encoding 0b11111 with XZR in insns */
	u64 pc;		/* no longer encoded as a GPR in AArch64! */
	u32 spsr;
	struct fp_reg
	{
		u64 low;
		u64 high;
	} __packed v[32];
	u32 fpsr;
	u32 fpcr;
} __packed;

/* Scratch value to write reentrant exception states to. We never read it. */
static struct exception_state sentinel_exception_state;

static int gdb_exception_hook(u32 type)
{
	if (!gdb_handle_reentrant_exception()) {
		u8 signal;

		if (type >= EXC_SYNC_SPX) {
			printf("Impossible exception type: %d!\n", type);
			return 0;
		}

		if (type == EXC_IRQ_SP0 || type == EXC_FIQ_SP0)
			signal = GDB_SIGINT;
		else switch (exception_state.ec) {
		case ESR_EC_UNKNOWN:
			signal = GDB_SIGILL;
			break;
		case ESR_EC_SVC_64:	/* gdb_arch_enter() uses SVC */
		case ESR_EC_SS_SAME:	/* single-step causes this one */
		case ESR_EC_BKPT_64:	/* GDB itself likes to insert BRKs */
			signal = GDB_SIGTRAP;
			break;
		default:
			/* We mostly expect INSN_ABT, DATA_ABT and SERROR here,
			   but it makes for a good catchall signal anyway. */
			signal = GDB_SIGSEGV;
			/* GDB itself doesn't read out the ESR, so print it to
			   help people understand unexpected exceptions. But we
			   can't print anything if GDB is not connected yet. */
			if (gdb_state.connected)
				printf("Remote-GDB Exception %d, ESR: %#08x\n",
				       type, (uint32_t)exception_state.esr);
		}

		exception_set_state_ptr(&sentinel_exception_state);
		gdb_command_loop(signal);
	}

	exception_set_state_ptr(&exception_state);

	return 1;
}

void gdb_arch_init(void)
{
	exception_install_hook(&gdb_exception_hook);
	raw_write_oslar_el1(0);		/* Disable OS lock (whatever that is) */
	raw_write_mdcr_el2(MDCR_TDE);	/* Route debug exceptions to EL2 */
	raw_write_mdscr_el1(MDSCR_KDE);	/* Enable debugging of current EL */
}

void gdb_arch_enter(void)
{
	u64 *sp;

	asm volatile ("mov %0, sp" : "=r"(sp) );

	/* Avoid reentrant exceptions, just call the hook if in one already.
	   This is mostly important when gdb_enter() is called as result of an
	   exception (as part of the halt() at the end). */
	if (sp >= exception_stack && sp <= exception_stack_end)
		gdb_exception_hook(EXC_SYNC_SP0);
	else	/* BRK doesn't adjust ELR, so using SVC makes things easier. */
		asm volatile ("svc #0");
}

int gdb_arch_set_single_step(int on)
{
	raw_write_mdscr_el1(MDSCR_KDE | (on ? MDSCR_SS : 0));
	exception_state.pstate.ss = !!on;
	return 0;
}

void gdb_arch_encode_regs(struct gdb_message *message)
{
	gdb_message_encode_bytes(message, &exception_state.regs,
				 sizeof(exception_state.regs));
	gdb_message_encode_bytes(message, &exception_state.sp,
				 sizeof(exception_state.sp));
	gdb_message_encode_bytes(message, &exception_state.elr,
				 sizeof(exception_state.elr));
	gdb_message_encode_bytes(message, &exception_state.spsr,
				 sizeof(exception_state.spsr));
	gdb_message_encode_zero_bytes(message,
		sizeof(struct gdb_regs) - offsetof(struct gdb_regs, v));
}

void gdb_arch_decode_regs(int offset, struct gdb_message *message)
{
	gdb_message_decode_bytes(message, offset,
			&exception_state.regs, sizeof(exception_state.regs));
	offset += sizeof(exception_state.regs) * 2;
	gdb_message_decode_bytes(message, offset,
			&exception_state.sp, sizeof(exception_state.sp));
	offset += sizeof(exception_state.sp) * 2;
	gdb_message_decode_bytes(message, offset,
			&exception_state.elr, sizeof(exception_state.elr));
	offset += sizeof(exception_state.elr) * 2;
	gdb_message_decode_bytes(message, offset,
			&exception_state.spsr, sizeof(exception_state.spsr));
	offset += sizeof(exception_state.spsr) * 2;
}
