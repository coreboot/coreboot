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

/* Load an immediate 32-bit value into a register */
#define LOAD_IMM32(r, e)			\
	li      r, 0;				\
	oris    r,r, (e)@h;			\
	ori     r,r, (e)@l;

/*
 * On POWER, 0 is wherever HRMOR points to rather than physical DRAM start.
 * HRMOR is ORed with address, not added to it, meaning that memory space
 * overlaps after 2^(least significant set bit of HRMOR). This becomes
 * chaotic when nonconsecutive bits are set...
 *
 * Two and a half possible cases:
 * 0. bootblock started with QEMU in hb-mode
 *    - NIA =                          0x10  (bug?)
 *    - HRMOR =                  0x08000000  (128M)
 *    - no physical memory to enable/train, everything accessible from start
 * 1. bootblock loaded by HBBL
 *    - NIA =                             0
 *    - HRMOR =                  0xF8000000  (4G - 128M)
 *    - initialized L3 =           0x400000  (4M)
 *    - top address before RAM = 0xF8400000
 * 2. bootblock in SEEPROM, loaded by SBE
 *    - NIA =                        0x3000  (placeholder for int. vectors)
 *    - HRMOR =                  0xF8200000  (4G - 128 M + 2 M)
 *    - initialized L3 =             0x8000  (bootblock/HBBL size = 32K)
 *    - no way 32K will be enough, must initialize more L3 in bootblock
 *    - HRMOR still applies, so memory overlaps every 2M
 *
 * Common subset (assuming 2. initializes as much memory as possible) is
 * 0xF8200000-0xF8400000. 2M should be more than enough for pre-RAM code,
 * but it isn't enough to load ramstage. We could implement postcar stage,
 * but KISS: initialize L3 from _ebootblock to 0xF8980000: up to 9.5M into
 * cache, leaving bottom 2M (0xF8000000-0xF8200000) either uninitialized
 * (when started from SEEPROM) or just unused for anything but bootblock
 * (loaded by HBBL). Last 0.5M of L3 cache is left for interrupt vectors
 * normally located at address 0.
 *
 * Set HRMOR to 0 before jumping to C code in bootblock and forget it even
 * exists.
 *
 * For QEMU s/0xF8/0x08/ in above description but code remains the same.
 * L3 initialization is unnecessary in this case but won't break anything.
 *
 * TODO: there is a structure with SBE->HBBL data at 0 in 2nd option. It
 * holds some useful data like XSCOM BAR and LPC BAR. If, for any reason,
 * these addresses are different than default, they should be used instead
 * of predefined values.
 */

.section ".text._start", "ax", %progbits
.globl _start
_start:
	/* QEMU with hb-mode=on starts at address 0x10, while hardware at 0x0 */
#if !CONFIG(BOOTBLOCK_IN_SEEPROM)
	nop
	nop
	nop
	nop
	FIXUP_ENDIAN
#endif

	/* Store FDT address provided by QEMU in %r3 to pass it later to
	 * payload */
	mtspr	SPR_HSPRG0, %r3

	/* Set program priority to medium */
	or	%r2, %r2, %r2

	li	%r10, 1
	rotldi	%r10, %r10, 63		/* MSB is "ignore HRMOR" */

	/* Assumption: we are linked at address that isn't changed by HRMOR */
	LOAD_IMM32(%r7, ignoreHRMOR)
	or	%r9, %r7, %r10

	mtlr %r9
	blr

ignoreHRMOR:
	/* Now we are at 0x8000000000000000 | linked address */
	li	%r0, 0
	mtspr	SPR_HRMOR, %r0		/* Clear HRMOR */
	isync

	/* We can't just "b stopIgnoringHRMOR", it would use relative offset */
	addi	%r9, %r7, stopIgnoringHRMOR - ignoreHRMOR
	mtlr	%r9
	blr

stopIgnoringHRMOR:
	/* Now we are at linked address */
	slbia 7
	sync
	isync

	/*
	 * When coming from SBE, L3 cache is invalid except for [2M, end of HBBL]
	 * range. Make the rest of it valid, or embrace the checkstops.
	 */

	/* Validate and initialize to zeroes [end of HBBL, 9.5M] range */
	LOAD_IMM32(%r5, _ebootblock)	/* Assume it is at least 128B aligned */
	LOAD_IMM32(%r6, _epreram_cbfs_cache)	/* Same */
	addi	%r6, %r6, -1
1:
	dcbz	0, %r5
	addi	%r5, %r5, 128
	cmpld	cr7, %r5, %r6
	blt	cr7, 1b

	/* Stack */
	LOAD_IMM32(%r1, _estack)

	/* This is tested by checkstack() just before jumping to payload */
	LOAD_IMM64(%r3, 0xDEADBEEFDEADBEEF)
	LOAD_IMM32(%r5, _stack - 8)
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
	mtmsrd	%r3

	/* Load official procedure descriptor address for main() */
	LOAD_IMM32(%r12, main)

	/* Load TOC pointer and jump to main() */
	ld	%r2, 8(%r12)
	b main
