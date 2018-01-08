/*
 * Early initialization code for riscv
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

#include <bits.h>
#include <mcall.h>

.macro restore_regs
    # restore x registers
    LOAD  x1,1*REGBYTES(a0)
    LOAD  x2,2*REGBYTES(a0)
    LOAD  x3,3*REGBYTES(a0)
    LOAD  x4,4*REGBYTES(a0)
    LOAD  x5,5*REGBYTES(a0)
    LOAD  x6,6*REGBYTES(a0)
    LOAD  x7,7*REGBYTES(a0)
    LOAD  x8,8*REGBYTES(a0)
    LOAD  x9,9*REGBYTES(a0)
    LOAD  x11,11*REGBYTES(a0)
    LOAD  x12,12*REGBYTES(a0)
    LOAD  x13,13*REGBYTES(a0)
    LOAD  x14,14*REGBYTES(a0)
    LOAD  x15,15*REGBYTES(a0)
    LOAD  x16,16*REGBYTES(a0)
    LOAD  x17,17*REGBYTES(a0)
    LOAD  x18,18*REGBYTES(a0)
    LOAD  x19,19*REGBYTES(a0)
    LOAD  x20,20*REGBYTES(a0)
    LOAD  x21,21*REGBYTES(a0)
    LOAD  x22,22*REGBYTES(a0)
    LOAD  x23,23*REGBYTES(a0)
    LOAD  x24,24*REGBYTES(a0)
    LOAD  x25,25*REGBYTES(a0)
    LOAD  x26,26*REGBYTES(a0)
    LOAD  x27,27*REGBYTES(a0)
    LOAD  x28,28*REGBYTES(a0)
    LOAD  x29,29*REGBYTES(a0)
    LOAD  x30,30*REGBYTES(a0)
    LOAD  x31,31*REGBYTES(a0)
    # restore a0 last
    LOAD  x10,10*REGBYTES(a0)


    .endm
.macro save_tf
  # save gprs
  STORE  x1,1*REGBYTES(x2)
  STORE  x3,3*REGBYTES(x2)
  STORE  x4,4*REGBYTES(x2)
  STORE  x5,5*REGBYTES(x2)
  STORE  x6,6*REGBYTES(x2)
  STORE  x7,7*REGBYTES(x2)
  STORE  x8,8*REGBYTES(x2)
  STORE  x9,9*REGBYTES(x2)
  STORE  x10,10*REGBYTES(x2)
  STORE  x11,11*REGBYTES(x2)
  STORE  x12,12*REGBYTES(x2)
  STORE  x13,13*REGBYTES(x2)
  STORE  x14,14*REGBYTES(x2)
  STORE  x15,15*REGBYTES(x2)
  STORE  x16,16*REGBYTES(x2)
  STORE  x17,17*REGBYTES(x2)
  STORE  x18,18*REGBYTES(x2)
  STORE  x19,19*REGBYTES(x2)
  STORE  x20,20*REGBYTES(x2)
  STORE  x21,21*REGBYTES(x2)
  STORE  x22,22*REGBYTES(x2)
  STORE  x23,23*REGBYTES(x2)
  STORE  x24,24*REGBYTES(x2)
  STORE  x25,25*REGBYTES(x2)
  STORE  x26,26*REGBYTES(x2)
  STORE  x27,27*REGBYTES(x2)
  STORE  x28,28*REGBYTES(x2)
  STORE  x29,29*REGBYTES(x2)
  STORE  x30,30*REGBYTES(x2)
  STORE  x31,31*REGBYTES(x2)

  # get sr, epc, badvaddr, cause
  csrrw  t0,mscratch,x0
  csrr   s0,mstatus
  csrr   t1,mepc
  csrr   t2,mbadaddr
  csrr   t3,mcause
  STORE  t0,2*REGBYTES(x2)
  STORE  s0,32*REGBYTES(x2)
  STORE  t1,33*REGBYTES(x2)
  STORE  t2,34*REGBYTES(x2)
  STORE  t3,35*REGBYTES(x2)

  # get faulting insn, if it wasn't a fetch-related trap
  li x5,-1
  STORE x5,36*REGBYTES(x2)

  .endm

.globl estack
  .text

    .global  trap_entry
    .align 2	# four byte alignment, as required by mtvec
trap_entry:
  csrw mscratch, sp

  # SMP isn't supported yet, to avoid overwriting the same stack with different
  # harts that handle traps at the same time.
  # someday this gets fixed.
  //csrr sp, mhartid
  csrr sp, 0xf14
.Lsmp_hang:
  bnez sp, .Lsmp_hang

  # Use a different stack than in the main context, to to avoid overwriting
  # stack data.
  # TODO: Maybe use the old stack pointer (plus an offset) instead. But only if
  # the previous mode was M, because it would be a very bad idea to use a stack
  # pointer provided by unprivileged code!
  la	sp, _estack
  addi	sp, sp, -2048	# 2 KiB is half of the stack space
  addi	sp, sp, -MENTRY_FRAME_SIZE

  save_tf
  move  a0,sp
  jal trap_handler

trap_return:
	csrr	a0, mscratch
	restore_regs
	# go back to the previous mode
	mret
