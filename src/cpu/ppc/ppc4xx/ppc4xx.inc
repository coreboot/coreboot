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

/*
 * Startup Code for IBM 4xx PowerPC based Embedded Boards
 *
 *  Base on the U-Boot Startup Code
 *
 *  The processor starts at 0xfffffffc and the code is executed
 *  from flash/rom. 
 *
 *  The purpose of this code is:
 *  - initalize the processor to a known state
 *  - turn on the I cache so things execute at a reasonable speed
 *  - set up a temporary stack in D cache
 */

#include "ppc4xx.h"

	li	r4,0x0000
	mtspr	sgr,r4
	mtspr	dcwr,r4
	mtesr	r4			/* clear Exception Syndrome Reg */
	mttcr	r4			/* clear Timer Control Reg */
	mtxer	r4			/* clear Fixed-Point Exception Reg */
	mtevpr	r4			/* clear Exception Vector Prefix Reg */
	li	r4,0x1000		/* set ME bit (Machine Exceptions) */
	oris	r4,r4,0x0002		/* set CE bit (Critical Exceptions) */
	mtmsr	r4			/* change MSR */
	li	r4,(0xFFFF-0x10000)	/* set r4 to 0xFFFFFFFF (status in */
					/* the dbsr is cleared by setting */
					/* bits to 1) */
	mtdbsr	r4			/* clear/reset the dbsr */

	/*
	 * Invalidate I and D caches. Enable I cache for defined memory       
	 * regions to speed things up. Enable D cache for use as
	 * temporary memory until real memory is enabled.
	 */

	bl	invalidate_icache
	bl	invalidate_dcache

	/* 
	 * Enable two 128MB cachable instruction regions 
	 *
	 *	0x00000000 - 0x07FFFFFF
	 *	0xF8000000 - 0xFFFFFFFF
	 */

	lis	r4,0x8000
	ori	r4,r4,0x0001
	mticcr	r4			/* instruction cache enable */
	isync

	/*
	 * Enable dcache region containing CONFIG_DCACHE_RAM_BASE
	 * On reset all regions are set to write-back, so we
	 * just leave them alone.
	 *
	 * dccr = (1 << (0x1F - (CONFIG_DCACHE_RAM_BASE >> 27))
	 */

        lis     r4, CONFIG_DCACHE_RAM_BASE@ha
	ori     r4, r4, CONFIG_DCACHE_RAM_BASE@l
	srwi	r4, r4, 27
	subfic	r4, r4, 31
	li	r0, 1
	slw	r4, r0, r4
	mtdccr	r4			/* data cache enable */
	sync
