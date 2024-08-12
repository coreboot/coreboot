/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Early initialization code for riscv
 */

#include <bits.h>
#include <mcall.h>

.macro restore_regs
	# restore x registers
	LOAD	 x1, 1 * REGBYTES(sp)
	LOAD	 x3, 3 * REGBYTES(sp)
	LOAD	 x4, 4 * REGBYTES(sp)
	LOAD	 x5, 5 * REGBYTES(sp)
	LOAD	 x6, 6 * REGBYTES(sp)
	LOAD	 x7, 7 * REGBYTES(sp)
	LOAD	 x8, 8 * REGBYTES(sp)
	LOAD	 x9, 9 * REGBYTES(sp)
	LOAD	x10, 10 * REGBYTES(sp)
	LOAD	x11, 11 * REGBYTES(sp)
	LOAD	x12, 12 * REGBYTES(sp)
	LOAD	x13, 13 * REGBYTES(sp)
	LOAD	x14, 14 * REGBYTES(sp)
	LOAD	x15, 15 * REGBYTES(sp)
	LOAD	x16, 16 * REGBYTES(sp)
	LOAD	x17, 17 * REGBYTES(sp)
	LOAD	x18, 18 * REGBYTES(sp)
	LOAD	x19, 19 * REGBYTES(sp)
	LOAD	x20, 20 * REGBYTES(sp)
	LOAD	x21, 21 * REGBYTES(sp)
	LOAD	x22, 22 * REGBYTES(sp)
	LOAD	x23, 23 * REGBYTES(sp)
	LOAD	x24, 24 * REGBYTES(sp)
	LOAD	x25, 25 * REGBYTES(sp)
	LOAD	x26, 26 * REGBYTES(sp)
	LOAD	x27, 27 * REGBYTES(sp)
	LOAD	x28, 28 * REGBYTES(sp)
	LOAD	x29, 29 * REGBYTES(sp)
	LOAD	x30, 30 * REGBYTES(sp)
	LOAD	x31, 31 * REGBYTES(sp)
.endm

.macro save_tf
	# save general purpose registers
	# no point in saving x0 since it is always 0
	STORE	 x1, 1 * REGBYTES(sp)
	# x2 is our stack pointer and is saved further below
	STORE	 x3, 3 * REGBYTES(sp)
	STORE	 x4, 4 * REGBYTES(sp)
	STORE	 x5, 5 * REGBYTES(sp)
	STORE	 x6, 6 * REGBYTES(sp)
	STORE	 x7, 7 * REGBYTES(sp)
	STORE	 x8, 8 * REGBYTES(sp)
	STORE	 x9, 9 * REGBYTES(sp)
	STORE	x10, 10 * REGBYTES(sp)
	STORE	x11, 11 * REGBYTES(sp)
	STORE	x12, 12 * REGBYTES(sp)
	STORE	x13, 13 * REGBYTES(sp)
	STORE	x14, 14 * REGBYTES(sp)
	STORE	x15, 15 * REGBYTES(sp)
	STORE	x16, 16 * REGBYTES(sp)
	STORE	x17, 17 * REGBYTES(sp)
	STORE	x18, 18 * REGBYTES(sp)
	STORE	x19, 19 * REGBYTES(sp)
	STORE	x20, 20 * REGBYTES(sp)
	STORE	x21, 21 * REGBYTES(sp)
	STORE	x22, 22 * REGBYTES(sp)
	STORE	x23, 23 * REGBYTES(sp)
	STORE	x24, 24 * REGBYTES(sp)
	STORE	x25, 25 * REGBYTES(sp)
	STORE	x26, 26 * REGBYTES(sp)
	STORE	x27, 27 * REGBYTES(sp)
	STORE	x28, 28 * REGBYTES(sp)
	STORE	x29, 29 * REGBYTES(sp)
	STORE	x30, 30 * REGBYTES(sp)
	STORE	x31, 31 * REGBYTES(sp)

	# get sr, epc, badvaddr, cause
	csrr	t0, mscratch
	bnez	t0, 1f	# t0 == 0, trap come from coreboot
			# t0 != 0, t0 is saved old sp
	add	t0, sp, MENTRY_FRAME_SIZE
1:
	csrr	s0, mstatus
	csrr	t1, mepc
	csrr	t2, mtval
	csrr	t3, mcause
	STORE	t0, 2 * REGBYTES(sp)
	STORE	s0, 32 * REGBYTES(sp)
	STORE	t1, 33 * REGBYTES(sp)
	STORE	t2, 34 * REGBYTES(sp)
	STORE	t3, 35 * REGBYTES(sp)

	# get faulting insn, if it wasn't a fetch-related trap
	li	x5, -1
	STORE	x5, 36 * REGBYTES(sp)
.endm

	.text
	.global  trap_entry
	.align 2 # four byte alignment, as required by mtvec
trap_entry:
	# mscratch is initialized to 0
	# when exiting coreboot, write sp to mscratch
	# before jumping to m-mode firmware we always set trap vector to the entry point of the
	# payload and we don't care about mscratch anymore. mscratch is only ever used as
	# exception stack if whatever coreboot jumps to is in s-mode.
	#TODO we could check MPP field in mstatus to see if come from unpriviledged code. That
	#     way we could still use mscratch for other purposes inside the code base.
	#TODO In case we got called from s-mode firmware we need to protect our stack and trap
	#     handler with a PMP region.
	csrrw	sp, mscratch, sp
	# sp == 0 => trap came from coreboot
	# sp != 0 => trap came from s-mode payload
	bnez	sp, 1f
	csrrw	sp, mscratch, sp
1:
	addi	sp, sp, -MENTRY_FRAME_SIZE
	save_tf

	mv a0,sp # put trapframe as first argument

	jal trap_handler

trap_return:
	restore_regs
	addi	sp, sp, MENTRY_FRAME_SIZE

	# restore original stack pointer (either sp or mscratch)
	csrrw	sp, mscratch, sp
	bnez	sp, 1f
	csrrw	sp, mscratch, sp
1:
	mret
