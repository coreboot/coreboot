/*
 * Copyright (c) 2013-2014, ARM Limited and Contributors. All rights reserved.
 * Copyright (c) 2015, NVIDIA CORPORATION. All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of ARM nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <arch/asm.h>
#include <arch/cache_helpers.h>

	/* ---------------------------------------------------------------
	 * Data cache operations by set/way to the level specified
	 *
	 * The main function, do_dcsw_op requires:
	 * x0: The operation type (0-2), as defined in cache_helpers.h
	 * x3: The last cache level to operate on
	 * x9: clidr_el1
	 * and will carry out the operation on each data cache from level 0
	 * to the level in x3 in sequence
	 *
	 * The dcsw_op macro sets up the x3 and x9 parameters based on
	 * clidr_el1 cache information before invoking the main function
	 * ---------------------------------------------------------------
	 */

.macro	dcsw_op shift, fw, ls
	mrs	x9, clidr_el1
	ubfx	x3, x9, \shift, \fw
	lsl	x3, x3, \ls
	b	do_dcsw_op
.endm

ENTRY(do_dcsw_op)
	cbz	x3, exit
	mov	x10, xzr
	adr	x14, dcsw_loop_table	// compute inner loop address
	add	x14, x14, x0, lsl #5	// inner loop is 8x32-bit instructions
	mov	x0, x9
	mov	w8, #1
loop1:
	add	x2, x10, x10, lsr #1	// work out 3x current cache level
	lsr	x1, x0, x2		// extract cache type bits from clidr
	and	x1, x1, #7		// mask the bits for current cache only
	cmp	x1, #2			// see what cache we have at this level
	b.lt	level_done		// nothing to do if no cache or icache

	msr	csselr_el1, x10		// select current cache level in csselr
	isb				// isb to sych the new cssr&csidr
	mrs	x1, ccsidr_el1		// read the new ccsidr
	and	x2, x1, #7		// extract the length of the cache lines
	add	x2, x2, #4		// add 4 (line length offset)
	ubfx	x4, x1, #3, #10		// maximum way number
	clz	w5, w4			// bit position of way size increment
	lsl	w9, w4, w5		// w9 = aligned max way number
	lsl	w16, w8, w5		// w16 = way number loop decrement
	orr	w9, w10, w9		// w9 = combine way and cache number
	ubfx	w6, w1, #13, #15	// w6 = max set number
	lsl	w17, w8, w2		// w17 = set number loop decrement
	dsb	sy			// barrier before we start this level
	br	x14			// jump to DC operation specific loop

level_done:
	add	x10, x10, #2		// increment cache number
	cmp	x3, x10
	b.gt    loop1
	msr	csselr_el1, xzr		// select cache level 0 in csselr
	dsb	sy			// barrier to complete final cache operation
	isb
exit:
	ret
ENDPROC(do_dcsw_op)

.macro	dcsw_loop _op
loop2_\_op:
	lsl	w7, w6, w2		// w7 = aligned max set number

loop3_\_op:
	orr	w11, w9, w7		// combine cache, way and set number
	dc	\_op, x11
	subs	w7, w7, w17		// decrement set number
	b.ge	loop3_\_op

	subs	x9, x9, x16		// decrement way number
	b.ge	loop2_\_op

	b	level_done
.endm

dcsw_loop_table:
	dcsw_loop isw
	dcsw_loop cisw
	dcsw_loop csw

ENTRY(flush_dcache_louis)
	dcsw_op #LOUIS_SHIFT, #CLIDR_FIELD_WIDTH, #LEVEL_SHIFT
ENDPROC(flush_dcache_louis)

ENTRY(flush_dcache_all)
	dcsw_op #LOC_SHIFT, #CLIDR_FIELD_WIDTH, #LEVEL_SHIFT
ENDPROC(flush_dcache_all)
