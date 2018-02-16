/*
 * Early initialization code for riscv virtual memory
 *
 * Copyright 2015 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 */

#include <arch/encoding.h>
#include <stdint.h>
#include <vm.h>

/* Delegate controls which traps are delegated to the payload. If you
 * wish to temporarily disable some or all delegation you can, in a
 * debugger, set it to a different value (e.g. 0 to have all traps go
 * to M-mode). In practice, this variable has been a lifesaver.  It is
 * still not quite determined which delegation might by unallowed by
 * the spec so for now we enumerate and set them all. */
static int delegate = 0
	| (1 << CAUSE_MISALIGNED_FETCH)
	| (1 << CAUSE_FAULT_FETCH)
	| (1 << CAUSE_ILLEGAL_INSTRUCTION)
	| (1 << CAUSE_BREAKPOINT)
	| (1 << CAUSE_FAULT_LOAD)
	| (1 << CAUSE_FAULT_STORE)
	| (1 << CAUSE_USER_ECALL)
	;

void mstatus_init(void)
{
	uintptr_t ms = 0;

	ms = INSERT_FIELD(ms, MSTATUS_FS, 3);
	ms = INSERT_FIELD(ms, MSTATUS_XS, 3);
	write_csr(mstatus, ms);

	// clear any pending timer interrupts.
	clear_csr(mip,  MIP_STIP | MIP_SSIP);

	// enable machine and supervisor timer and
	// all other supervisor interrupts.
	set_csr(mie, MIP_MTIP | MIP_STIP | MIP_SSIP);

	// Delegate supervisor timer and other interrupts
	// to supervisor mode.
	set_csr(mideleg,  MIP_STIP | MIP_SSIP);

	set_csr(medeleg, delegate);

	// Enable all user/supervisor-mode counters using
	// v1.10 register addresses.
	// They moved from the earlier spec.
	// Until we trust our toolchain use the hardcoded constants.
	// These were in flux and people who get the older toolchain
	// will have difficult-to-debug failures.
	write_csr(/*mcounteren*/0x306, 7);
}
