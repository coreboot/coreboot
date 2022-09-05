/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Optimized assembly for low-level CPU operations on ARMv7 processors.
 *
 * Cache flushing code based off sys/arch/arm/arm/cpufunc_asm_armv7.S in NetBSD
 */

#include <arch/asm.h>

/*
 * Dcache invalidations by set/way work by passing a [way:sbz:set:sbz:level:0]
 * bitfield in a register to the appropriate MCR instruction. This algorithm
 * works by initializing a bitfield with the highest-numbered set and way, and
 * generating a "set decrement" and a "way decrement". The former just contains
 * the LSB of the set field, but the latter contains the LSB of the way field
 * minus the highest valid set field... such that when you subtract it from a
 * [way:0:level] field you end up with a [way - 1:highest_set:level] field
 * through the magic of double subtraction. It's quite ingenious, really.
 * Takes care to only use r0-r3 and ip so it's pefectly ABI-compatible without
 * needing to write to memory.
 *
 * THIS FUNCTION MUST PRESERVE THE VALUE OF r10
 */

#if ENV_USER_SPACE
/*
 * Empty macro for code running in userspace.  Trying to manipulate the
 * cache from userspace hangs the system. To run code at a privileged level,
 * the userspace code needs to execute an API call to the privileged mode
 * code.
 */
.macro	dcache_apply_all crm
	bx	lr
.endm

#else

.macro	dcache_apply_all crm
	dsb
	mov	r3, #-2			@ initialize level so that we start at 0

1:	@next_level
	add	r3, r3, #2		@ increment level

	mrc	p15, 1, r0, c0, c0, 1	@ read CLIDR
	and	ip, r0, #0x07000000	@ narrow to LoC
	lsr	ip, ip, #23		@ left align LoC (low 4 bits)
	cmp	r3, ip			@ compare
	bge	3f @done		@ else fall through (r0 == CLIDR)

	add	r2, r3, r3, lsr #1	@ r2 = (level << 1) * 3 / 2
	mov	r1, r0, lsr r2		@ r1 = cache type
	and	r1, r1, #7
	cmp	r1, #2			@ is it data or i&d?
	blt	1b @next_level		@ nope, skip level

	mcr	p15, 2, r3, c0, c0, 0	@ select cache level
	isb
	mrc	p15, 1, r0, c0, c0, 0	@ read CCSIDR

	ubfx	ip, r0, #0, #3		@ get linesize from CCSIDR
	add	ip, ip, #4		@ apply bias
	ubfx	r2, r0, #13, #15	@ get numsets - 1 from CCSIDR
	lsl	r2, r2, ip		@ shift to set position
	orr	r3, r3, r2		@ merge set into way/set/level
	mov	r1, #1
	lsl	r1, r1, ip		@ r1 = set decr

	ubfx	ip, r0, #3, #10		@ get numways - 1 from [to be discarded] CCSIDR
	clz	r2, ip			@ number of bits to MSB of way
	lsl	ip, ip, r2		@ shift by that into way position
	mov	r0, #1
	lsl	r2, r0, r2		@ r2 now contains the way decr
	mov	r0, r3			@ get sets/level (no way yet)
	orr	r3, r3, ip		@ merge way into way/set/level
	bfc	r0, #0, #4		@ clear low 4 bits (level) to get numset - 1
	sub	r2, r2, r0		@ subtract from way decr

	/* r3 = ways/sets/level, r2 = way decr, r1 = set decr, r0 and ip are free */
2:	mcr	p15, 0, r3, c7, \crm, 2	@ writeback and/or invalidate line
	cmp	r3, #15			@ are we done with this level (way/set == 0)
	bls	1b @next_level		@ yes, go to next level
	lsr	r0, r3, #4		@ clear level bits leaving only way/set bits
	lsls	r0, r0, #14		@ clear way bits leaving only set bits
	subne	r3, r3, r1		@ non-zero?, decrement set #
	subeq	r3, r3, r2		@ zero?, decrement way # and restore set count
	b	2b

3:	@done
	mov	r0, #0			@ default back to cache level 0
	mcr	p15, 2, r0, c0, c0, 0	@ select cache level
	dsb
	isb
	bx	lr
.endm

#endif /* ENV_USER_SPACE */

/*
 * Bring an ARM processor we just gained control of (e.g. from IROM) into a
 * known state regarding caches/SCTLR. Completely cleans and invalidates
 * icache/dcache, disables MMU and dcache (if active), and enables unaligned
 * accesses, icache and branch prediction (if inactive). Clobbers r4 and r5.
 *
 * THIS FUNCTION MUST PRESERVE THE VALUE OF r10
 */
ENTRY(arm_init_caches)
	/* r4: SCTLR, return address: r5 (stay valid for the whole function) */
	mov	r5, lr
	mrc	p15, 0, r4, c1, c0, 0

	/* Activate ICache (12) and Branch Prediction (11) already for speed */
	orr	r4, # (1 << 11) | (1 << 12)
	mcr	p15, 0, r4, c1, c0, 0

	/* Flush and invalidate dcache in ascending order */
	bl	dcache_invalidate_all

#if ENV_ARMV7_A
	/* Deactivate MMU (0), Alignment Check (1) and DCache (2) */
	and	r4, # ~(1 << 0) & ~(1 << 1) & ~(1 << 2)
	mcr	p15, 0, r4, c1, c0, 0

	/* Invalidate icache and TLB for good measure */
	mcr	p15, 0, r0, c7, c5, 0
	mcr	p15, 0, r0, c8, c7, 0
#endif

#if ENV_ARMV7_R
	/* Deactivate Alignment Check (1) and DCache (2) */
	and	r4, # ~(1 << 1) & ~(1 << 2)
	mcr	p15, 0, r4, c1, c0, 0

	/* Invalidate icache for good measure */
	mcr	p15, 0, r0, c7, c5, 0
#endif
	dsb
	isb

	bx	r5
ENDPROC(arm_init_caches)

ENTRY(dcache_invalidate_all)
	dcache_apply_all crm=c6
ENDPROC(dcache_invalidate_all)

ENTRY(dcache_clean_all)
	dcache_apply_all crm=c10
ENDPROC(dcache_clean_all)

ENTRY(dcache_clean_invalidate_all)
	dcache_apply_all crm=c14
ENDPROC(dcache_clean_invalidate_all)
