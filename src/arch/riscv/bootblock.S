/*
 * Early initialization code for RISC-V
 *
 * Copyright 2013 Google Inc.
 * Copyright 2016 Jonathan Neuschäfer <j.neuschaefer@gmx.net>
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

.section ".text._start", "ax", %progbits

.globl _stack
.global _estack
.globl _start
_start:



	# N.B. This only works on low 4G of the address space
	# and the stack must be page-aligned.
	la sp, _estack

	# make room for HLS and initialize it
	addi sp, sp, -64 // MENTRY_FRAME_SIZE
	csrr a0, mhartid
	call hls_init

	# poison the stack
	la t1, _stack
	li t0, 0xdeadbeef
	sd t0, 0(t1)

	la t0, trap_entry
	csrw mtvec, t0

	# clear any pending interrupts
	csrwi mip, 0

	# set up the mstatus register for VM
	call mstatus_init
	tail main
