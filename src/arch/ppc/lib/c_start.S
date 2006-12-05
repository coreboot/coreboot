/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2000 AG Electronics Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

/*
 * The assumption is that we're located in ROM and we have a fake stack
 * located in cache. Our task is to turn on memory proper, the finish
 * configuring the machine.
 */

#define ASM
#include "ppcreg.h"
#include <ppc_asm.tmpl>

.section ".text"
.globl _start

_start:
	/* 
	 * init stack pointer to real ram now that memory is on
	 * Note: We use the last 8 bytes on the stack to hold struct cpu_info,
	 *       Which are initialized to zero as we clear the stack.
	 */
	li	r0, 0
	lis     r1, _estack@ha
	addi    r1, r1, _estack@l
	stwu    r0,-64(r1) 
	stwu    r1,-24(r1) 

	/*
	 * Clear stack
	 */
	lis	r4, _stack@ha
	addi	r4, r4, _stack@l
	lis	r7, _estack@ha
	addi	r7, r7, _estack@l
	lis	r5, 0
1:	stwx	r5, 0, r4
	addi	r4, r4, 4
	cmp	0, 0, r4, r7
	ble	1b
	sync

	/*
	 * Clear bss
	 */
	lis	r4, _bss@ha
	addi	r4, r4, _bss@l
	lis	r7, _ebss@ha
	addi	r7, r7, _ebss@l
	lis	r5, 0
1:	stwx	r5, 0, r4
	addi	r4, r4, 4
	cmp	0, 0, r4, r7
	ble	1b
	sync

	/*
	 * Set up the EABI pointers, before we enter any C code
	 */
	lis     r13, _SDA_BASE_@ha
	addi    r13, r13, _SDA_BASE_@l
	lis     r2, _SDA2_BASE_@ha
	addi    r2, r2, _SDA2_BASE_@l

	/*
	 * load start address into SRR0 for rfi
	 */
	lis	r3, hardwaremain@ha
	addi	r3, r3, hardwaremain@l
	mtspr	SRR0, r3

	/*
	 * load the current MSR into SRR1 so that it will be copied 
	 * back into MSR on rfi
	 */
	mfmsr	r4
	mtspr	SRR1, r4	// load SRR1 with r4

	/*
	 * If something returns after rfi then die
	 */
	lis	r3, dead@ha
	addi	r3, r3, dead@l
	mtlr	r3

	/*
	 * Complete rest of initialization in C (hardwaremain)
	 */
	rfi

	/*
	 * Stop here if something goes wrong
	 */
dead:
	b	dead
	/*NOTREACHED*/

/* Remove need for ecrti.o and ectrn.o */
.globl __init
__init:
.globl __fini
__fini:
.globl __CTOR_LIST__
__CTOR_LIST__:
.globl __CTOR_END__
__CTOR_END__:
.globl __DTOR_LIST__
__DTOR_LIST__:
.globl __DTOR_END__
__DTOR_END__:
        blr
