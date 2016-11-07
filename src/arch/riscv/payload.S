/*
 * This file is part of the coreboot project.
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

// "return" to a payload pointed to by a1 with
// an M-mode pointer (or, to upper levels, physical address)
// to the config string in a0.
	.global riscvpayload
riscvpayload:
	mv t0,a1
	csrw mepc, t0
	csrr t0, mstatus
	li t1, ~(3<<11)
	and t0, t0, t1
	li t2,  (1<<11)
	or t0, t0, t2
	csrw mstatus, t0

	// We're still in toolchain no mans land.
	.word 0x30200073
	//mret
