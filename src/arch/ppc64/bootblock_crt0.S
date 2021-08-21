/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Early initialization code for POWER8/POWER9.
 */

#include <cpu/power/spr.h>

#define FIXUP_ENDIAN						   \
	tdi   0,0,0x48;	  /* Reverse endian of b . + 8		*/ \
	b     $+44;	  /* Skip trampoline if endian is good	*/ \
	.long 0xa600607d; /* mfmsr r11				*/ \
	.long 0x01006b69; /* xori r11,r11,1			*/ \
	.long 0x00004039; /* li r10,0				*/ \
	.long 0x6401417d; /* mtmsrd r10,1			*/ \
	.long 0x05009f42; /* bcl 20,31,$+4			*/ \
	.long 0xa602487d; /* mflr r10				*/ \
	.long 0x14004a39; /* addi r10,r10,20			*/ \
	.long 0xa6035a7d; /* mtsrr0 r10				*/ \
	.long 0xa6037b7d; /* mtsrr1 r11				*/ \
	.long 0x2400004c  /* rfid				*/

/* Load an immediate 64-bit value into a register */
#define LOAD_IMM64(r, e)			\
	lis     r,(e)@highest;			\
	ori     r,r,(e)@higher;			\
	rldicr  r,r, 32, 31;			\
	oris    r,r, (e)@h;			\
	ori     r,r, (e)@l;

.section ".text._start", "ax", %progbits
.globl _start
_start:
	/* QEMU with hb-mode=on starts at address 0x10, while hardware at 0x0 */
	nop
	nop
	nop
	nop
	FIXUP_ENDIAN

	/* Store FDT address provided by QEMU in %r3 to pass it later to
	 * payload */
	mtspr	SPR_HSPRG0, %r3

	/* Set program priority to medium */
	or	%r2, %r2, %r2

	/* Stack */
	lis	%r1, _estack@ha
	addi	%r1, %r1, _estack@l

	/* Clear .bss section */
	/* Currently not needed, .bss is zeroed in the file. If it were to be
	 * used, make sure that .bss is 128B aligned (size of cache line),
	 * otherwise dcbz will clear (part of) .opd section! */
/*
	lis	%r5, _bss@ha
	addi	%r5, %r5, _bss@l
	lis	%r6, _ebss@ha
	addi	%r6, %r6, _ebss@l
	addi	%r6, %r6, -1
1:
	dcbz	0, %r5
	addi	%r5, %r5, 128
	cmpld	cr7, %r5, %r6
	blt	cr7, 1b
*/

	/* This is tested by checkstack() just before jumping to payload */
	LOAD_IMM64(%r3, 0xDEADBEEFDEADBEEF)
	lis	%r5, _stack@ha
	addi	%r5, %r5, _stack@l
	subi	%r5, %r5, 8
	sub	%r4, %r1, %r5
	sradi	%r4, %r4, 3		/* Divide by 8 */
	mtctr	%r4
1:
	stdu	%r3, 8(%r5)
	bc	25, 0, 1b

	/* Enable floating point and vector operations */
	/* Vector operations are sometimes generated for code like
	 * 'uint8_t x[32] = {0}', this results in an exception when vector
	 * registers (VEC) are not enabled. VSX (vector-scalar extension) is
	 * also enabled, there is no reason not to. Floating point must also be
	 * enabled for VSX.
	 */
	mfmsr	%r3
	ori	%r3, %r3, 0x2000	/* FP = 1 */
	oris	%r3, %r3, 0x0280	/* VEC = 1, VSX = 1 */
	mtmsr	%r3

	/* Load official procedure descriptor address for main() */
	lis	%r12, main@ha
	addi	%r12, %r12, main@l

	/* Load TOC pointer and jump to main() */
	ld	%r2, 8(%r12)
	b main
