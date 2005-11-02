/*
 * (C) Copyright 2000, 2001
 * Erik Theisen, Wave 7 Optics, etheisen@mindspring.com.
 *  base on code by
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */
#include <ppc_asm.tmpl>

	.text
/*
 * unsigned long long _get_ticks(void);
 */
#ifndef __PPC64__
	.globl	_get_ticks
_get_ticks:
#else
	.globl	._get_ticks
._get_ticks:
#endif
1:	mftbu	r3
	mftb    r4
	mftbu	r5
	cmpw	0,r3,r5
	bne	1b
	blr

/*
 * Delay for a number of ticks
 */
#ifndef __PPC64__
        .globl  _wait_ticks
_wait_ticks:
#else
        .globl  ._wait_ticks
._wait_ticks:
#endif
        mflr    r8              /* save link register */
        mr      r7, r3          /* save tick count */
#ifndef __PPC64__
        bl      _get_ticks       /* Get start time */
#else
        bl      ._get_ticks       /* Get start time */
#endif

        /* Calculate end time */
        addc    r7, r4, r7      /* Compute end time lower */
        addze   r6, r3          /*     and end time upper */

#ifndef __PPC64__
1:      bl      _get_ticks       /* Get current time */
#else
1:      bl      ._get_ticks       /* Get current time */
#endif
        subfc   r4, r4, r7      /* Subtract current time from end time */
        subfe.  r3, r3, r6
        bge     1b              /* Loop until time expired */

        mtlr    r8              /* restore link register */
        blr

