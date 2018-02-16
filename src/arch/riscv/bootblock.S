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
#include <mcall.h>

.section ".text._start", "ax", %progbits

.globl _stack
.global _estack
.globl _start
_start:

	# The boot ROM may pass the following arguments to coreboot:
	#   a0: the value of mhartid
	#   a1: a pointer to the flattened devicetree
	#
	# Preserve only the FDT pointer. We can query mhartid ourselves at any
	# time.
	#
	csrw mscratch, a1

	# N.B. This only works on low 4G of the address space
	# and the stack must be page-aligned.
	la sp, _estack

	# poison the stack
	la t1, _stack
	li t0, 0xdeadbeef
	sd t0, 0(t1)

	# make room for HLS and initialize it
	addi sp, sp, -HLS_SIZE

	// Once again, the docs and toolchain disagree.
	// Rather than get fancy I'll just lock this down
	// until it all stabilizes.
	//csrr a0, mhartid
	csrr a0, 0xf14
	call hls_init

	la t0, trap_entry
	csrw mtvec, t0

	# clear any pending interrupts
	csrwi mip, 0

	# set up the mstatus register for VM
	call mstatus_init
	tail main
