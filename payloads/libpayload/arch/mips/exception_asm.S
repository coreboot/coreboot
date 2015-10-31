/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2014 Imagination Technologies
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

#define	STATUS_REGISTER		$12,0
#define	BOOT_EXC_VECTOR_MASK	(1 << 22)
#define	EBASE_REGISTER		$15,1
#define	EXCEPTION_BASE_MASK	(0xFFFFF000)

	/* Don't reorder instructions */
	.set noreorder
	.set noat

	.align 4
        .global exception_stack_end
exception_stack_end:
	.word 0

	.global exception_state_ptr
exception_state_ptr:
	.word 0

/* Temporary variables. */
ret_addr:
	.word 0
exception_sp:
        .word 0
vector:
	.word 0

/* Cache error */
.org 0x100
	li	$v0, 0x0
	la	$at, vector
	sw	$v0, 0x00($at)
	b	exception_common
	nop

/* TLB refill and all others */
.org 0x180
	li	$v0, 0x1
	la	$at, vector
	sw	$v0, 0x00($at)
	b	exception_common
	nop

/* Interrupt */
.org 0x200
	li	$v0, 0x2
	la	$at, vector
	sw	$v0, 0x00($at)
	b	exception_common
	nop

/* EJTAG debug exception */
.org 0x480
	li	$v0, 0x3
	la	$at, vector
	sw	$v0, 0x00($at)
	b	exception_common
	nop

exception_common:
	/* Obtain return address of exception */
	la	$v0, ret_addr
	sw	$ra, 0x00($v0)

	/* Initialize $gp */
	bal	1f
	nop
	.word	_gp
1:
	lw	$gp, 0($ra)

	la	$at, exception_sp
	sw	$sp, 0x00($at)
	lw	$sp, exception_state_ptr

	/* Save all registers */
	sw	$zero,	0x00($sp)
	sw	$at,	0x04($sp)
	sw	$v0, 	0x08($sp)
	sw	$v1,	0x0C($sp)
	sw	$a0,	0x10($sp)
	sw	$a1,	0x14($sp)
	sw	$a2,	0x18($sp)
	sw	$a3,	0x1C($sp)
	sw	$t0,	0x20($sp)
	sw	$t1,	0x34($sp)
	sw	$t2,	0x28($sp)
	sw	$t3,	0x2C($sp)
	sw	$t4,	0x30($sp)
	sw	$t5,	0x34($sp)
	sw	$t6,	0x38($sp)
	sw	$t7,	0x3C($sp)
	sw	$s0,	0x40($sp)
	sw	$s1,	0x44($sp)
	sw	$s2,	0x48($sp)
	sw	$s3,	0x4C($sp)
	sw	$s4,	0x50($sp)
	sw	$s5,	0x54($sp)
	sw	$s6,	0x58($sp)
	sw	$s7,	0x5C($sp)
	sw	$t8,	0x60($sp)
	sw	$t9,	0x64($sp)
	sw	$k0,	0x68($sp)
	sw	$k1,	0x6C($sp)
	sw	$gp,	0x70($sp)
	lw	$v0,	exception_sp
	sw	$v0,	0x74($sp)
	sw	$fp,	0x78($sp)
	lw	$v0,	ret_addr
	sw	$v0,	0x7C($sp)
	lw	$v0,	vector
	sw	$v0,	0x80($sp)

	/* Point SP to the stack for C code */
	lw	$sp, exception_stack_end
	/* Give control to exception dispatch */
	la	$a2, exception_dispatch
	jalr	$a2
	nop
	lw	$sp, exception_state_ptr
	/* Restore registers */
	lw	$zero,	0x00($sp)
	lw	$at,	0x04($sp)
	lw	$v0,	0x08($sp)
	lw	$v1,	0x0C($sp)
	lw	$a0,	0x10($sp)
	lw	$a1,	0x14($sp)
	lw	$a2,	0x18($sp)
	lw	$a3,	0x1C($sp)
	lw	$t0,	0x20($sp)
	lw	$t1,	0x24($sp)
	lw	$t2,	0x28($sp)
	lw	$t3,	0x2C($sp)
	lw	$t4,	0x30($sp)
	lw	$t5,	0x34($sp)
	lw	$t6,	0x38($sp)
	lw	$t7,	0x3C($sp)
	lw	$s0,	0x40($sp)
	lw	$s1,	0x44($sp)
	lw	$s2,	0x48($sp)
	lw	$s3,	0x4C($sp)
	lw	$s4,	0x50($sp)
	lw	$s5,	0x54($sp)
	lw	$s6,	0x58($sp)
	lw	$s7,	0x5C($sp)
	lw	$t8,	0x60($sp)
	lw	$t9,	0x64($sp)
	lw	$k0,	0x68($sp)
	sw	$k1,	0x6C($sp)
	sw	$gp,	0x70($sp)
	sw	$fp,	0x78($sp)
	sw	$ra,	0x7C($sp)
	/* Return */
	eret

	.global exception_init_asm
exception_init_asm:
	.set 	push
	/* Make sure boot exception vector is 1 before writing EBASE */
	mfc0	$t0, STATUS_REGISTER
	li	$t1, BOOT_EXC_VECTOR_MASK
	or	$t0, $t0, $t1
	mtc0	$t0, STATUS_REGISTER

	/*Prepare base address */
	la	$t1, exception_stack_end
	li	$t2, EXCEPTION_BASE_MASK
	and	$t1, $t1, $t2

	/* Prepare EBASE register value */
	mfc0	$t0, EBASE_REGISTER
	li	$t2, ~(EXCEPTION_BASE_MASK)
	and	$t0, $t0, $t2
	/* Filling base address */
	or	$t0, $t0, $t1
	mtc0	$t0, EBASE_REGISTER

	/* Clear boot exception vector bit for EBASE value to take effect */
	mfc0	$t0, STATUS_REGISTER
	li	$t1, ~BOOT_EXC_VECTOR_MASK
	and	$t0, $t0, $t1
	mtc0	$t0, STATUS_REGISTER

	.set 	pop
	/* Return */
	jr 	$ra
