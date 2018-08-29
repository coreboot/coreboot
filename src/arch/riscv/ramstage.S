/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 HardenedLinux
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

#include <arch/encoding.h>
#include <mcall.h>

.section ".text._start", "ax", %progbits
.globl _start
_start:
	# initialize stack point for each hart
	# and the stack must be page-aligned.
	# 0xDEADBEEF used to check stack overflow
	csrr a0, mhartid
	la   t0, _stack
	slli t1, a0, RISCV_PGSHIFT
	add  t0, t0, t1
	li   t1, 0xDEADBEEF
	sd   t1, 0(t0)
	li   t1, RISCV_PGSIZE - HLS_SIZE
	add  sp, t0, t1

	# initialize hart-local storage
	csrr a0, mhartid
	call hls_init

	# initialize entry of interrupt/exception
	la   t0, trap_entry
	csrw mtvec, t0

	# clear any pending interrupts
	csrwi mip, 0

	call exit_car
	# set up the mstatus register for VM
	call mstatus_init
	tail main

	# These codes need to be implemented on a specific SoC
	.weak exit_car
exit_car:
	ret
