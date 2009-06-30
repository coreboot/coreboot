/*
 *  Copyright (C) 1998	Dan Malek <dmalek@jlc.net>
 *  Copyright (C) 1999	Magnus Damm <kieraypc01.p.y.kie.era.ericsson.se>
 *  Copyright (C) 2000,2001,2002 Wolfgang Denk <wd@denx.de>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 *
 * This source code has been made available to you by IBM on an AS-IS
 * basis. Anyone receiving this source is licensed under IBM
 * copyrights to use it in any way he or she deems fit, including
 * copying it, modifying it, compiling it, and redistributing it either
 * with or without modifications. No license under IBM patents or
 * patent applications is to be implied by the copyright license.
 *
 * Any user of this software should understand that IBM cannot provide 
 * technical support for this software and will not be responsible for
 * any consequences resulting from the use of this software.
 *
 * Any person who transfers this source code or any derivative work
 * must include the IBM copyright notice, this paragraph, and the
 * preceding two paragraphs in the transferred software.
 *
 * COPYRIGHT   I B M   CORPORATION 1995
 * LICENSED MATERIAL  -  PROGRAM PROPERTY OF I B M
 *
 */

#include <ppc_asm.tmpl>

#define CACHELINE_SIZE	32	/* 32 bytes (8 words) */

/* 
 * Cache functions.
 */
	.globl	invalidate_icache
invalidate_icache:
	iccci	r0,r0			/* for 405, iccci invalidates the */
	blr				/*   entire I cache */

	.globl	invalidate_dcache
invalidate_dcache:
	li	r6,0x0000		/* clear GPR 6 */
	/* Do loop for # of dcache congruence classes. */
	li	r7,(CONFIG_DCACHE_RAM_SIZE / CACHELINE_SIZE / 2)
					/* NOTE: dccci invalidates both */
	mtctr	r7			/* ways in the D cache */
1:
	dccci	0,r6			/* invalidate line */
	addi	r6,r6, CACHELINE_SIZE /* bump to next line */
	bdnz	1b
	blr

	.globl	flush_dcache
flush_dcache:
	lis	r9,0x0002		/* set mask for EE and CE msr bits */
	ori	r9,r9,0x8000
	mfmsr	r12			/* save msr */
	andc	r9,r12,r9
	mtmsr	r9			/* disable EE and CE */
	li	r10,0x0001		/* enable data cache for unused memory */
	mfdccr	r9			/* region 0xF8000000-0xFFFFFFFF via */
	or	r10,r10,r9		/* bit 31 in dccr */
	mtdccr	r10

	/* do loop for # of congruence classes. */
	li	r10,(CONFIG_DCACHE_RAM_SIZE / CACHELINE_SIZE / 2)
	li	r11,(CONFIG_DCACHE_RAM_SIZE / 2) /* D cache set size - 2 way sets */
	mtctr	r10
	li	r10,(0xE000-0x10000)	/* start at 0xFFFFE000 */
	add	r11,r10,r11		/* add to get to other side of cache line */
1:
	lwz	r3,0(r10)		/* least recently used side */
	lwz	r3,0(r11)		/* the other side */
	dccci	r0,r11			/* invalidate both sides */
	addi	r10,r10,CACHELINE_SIZE /* bump to next line */
	addi	r11,r11,CACHELINE_SIZE /* bump to next line */
	bdnz	1b
	sync				/* allow memory access to complete */
	mtdccr	r9			/* restore dccr */
	mtmsr	r12			/* restore msr */
	blr

	.globl	icache_enable
icache_enable:
	mflr	r8
	bl	invalidate_icache
	mtlr	r8
	isync
	lis	r3,0x8000	      /* set bit 0 */
	mticcr	r3
	blr

	.globl	icache_disable
icache_disable:
	lis	r3,0x0000	      /* clear bit 0 */
	mticcr	r3
	isync
	blr

	.globl	icache_status
icache_status:
	mficcr	r3
	srwi	r3, r3, 31	/* >>31 => select bit 0 */
	blr

	.globl	dcache_enable
dcache_enable:
	mflr	r8
	bl	invalidate_dcache
	mtlr	r8
	isync
	lis	r3,0x8000	      /* set bit 0 */
	mtdccr	r3
	blr

	.globl	dcache_disable
dcache_disable:
	mflr	r8
	bl	flush_dcache
	mtlr	r8
	lis	r3,0x0000	      /* clear bit 0 */
	mtdccr	r3
	blr

	.globl	dcache_status
dcache_status:
	mfdccr	r3
	srwi	r3, r3, 31	/* >>31 => select bit 0 */
	blr

/*------------------------------------------------------------------------------- */
/* Function:	 ppcDcbf */
/* Description:	 Data Cache block flush */
/* Input:	 r3 = effective address */
/* Output:	 none. */
/*------------------------------------------------------------------------------- */
	.globl	ppcDcbf
ppcDcbf:
	dcbf	r0,r3
	blr

/*------------------------------------------------------------------------------- */
/* Function:	 ppcDcbi */
/* Description:	 Data Cache block Invalidate */
/* Input:	 r3 = effective address */
/* Output:	 none. */
/*------------------------------------------------------------------------------- */
	.globl	ppcDcbi
ppcDcbi:
	dcbi	r0,r3
	blr

/*------------------------------------------------------------------------------- */
/* Function:	 ppcSync */
/* Description:	 Processor Synchronize */
/* Input:	 none. */
/* Output:	 none. */
/*------------------------------------------------------------------------------- */
	.globl	ppcSync
ppcSync:
	sync
	blr

