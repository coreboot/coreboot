/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Early initialization code for RISC-V
 */

#include <arch/encoding.h>
#include <bits.h>
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

	# initialize cache as ram
	call cache_as_ram

	# initialize stack point for each hart
	# and the stack must be page-aligned.
	# 0xDEADBEEF used to check stack overflow
	csrr a0, mhartid
	la   t0, _stack
	slli t1, a0, RISCV_PGSHIFT
	add  t0, t0, t1
	li   t1, 0xDEADBEEF
	STORE t1, 0(t0)
	li   t1, RISCV_PGSIZE - HLS_SIZE
	add  sp, t0, t1

	# initialize hart-local storage
	csrr a0, mhartid
	csrrw a1, mscratch, zero
	call hls_init

	li   a0, CONFIG_RISCV_WORKING_HARTID
	call smp_pause

	# initialize entry of interrupt/exception
	la t0, trap_entry
	csrw mtvec, t0

	# clear any pending interrupts
	csrwi mip, 0

	# set up the mstatus register
	call mstatus_init
	tail main

	// These codes need to be implemented on a specific SoC.
	.weak cache_as_ram
cache_as_ram:
	ret
