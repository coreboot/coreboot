/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/encoding.h>
#include <bits.h>
#include <mcall.h>

.section ".text._start", "ax", %progbits
.globl _start
_start:
	/* cbmem_top is passed via a2 */
	la   t0, _cbmem_top_ptr
#if __riscv_xlen == 32
	sw   a2, (t0)
#elif __riscv_xlen == 64
	sd   a2, (t0)
#endif
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
	call hls_init

	li   a0, CONFIG_RISCV_WORKING_HARTID
	call smp_pause

	# initialize entry of interrupt/exception
	la   t0, trap_entry
	csrw mtvec, t0

	# clear any pending interrupts
	csrwi mip, 0

	call exit_car
	# set up the mstatus register
	call mstatus_init
	tail main

	# These codes need to be implemented on a specific SoC
	.weak exit_car
exit_car:
	ret
