/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 VIA Technologies, Inc.
 * (Written by Jason Zhao <jasonzhao@viatech.com.cn> for VIA)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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
 
        __asm__ volatile (
	/* 
	FIXME : backup stack in CACHE_AS_RAM into mmx and sse and after we get STACK up, we restore that.
		It is only needed if we want to go back
	*/
	
        /* We don't need cache as ram for now on */
        /* disable cache */
        "movl    %cr0, %eax\n\t"
        "orl    $(0x1<<30),%eax\n\t"
        "movl    %eax, %cr0\n\t"


        /* Set the default memory type and disable fixed and enable variable MTRRs */
        "movl    $0x2ff, %ecx\n\t"
	//"movl    $MTRRdefType_MSR, %ecx\n\t"
        "xorl    %edx, %edx\n\t"
        /* Enable Variable and Disable Fixed MTRRs */
        "movl    $0x00000800, %eax\n\t"
        "wrmsr\n\t"

        /* enable caching for first 1M using variable mtrr */
        "movl    $0x200, %ecx\n\t"
        "xorl    %edx, %edx\n\t"
        "movl     $(0 | 6), %eax\n\t"
	//"movl     $(0 | MTRR_TYPE_WRBACK), %eax\n\t"
        "wrmsr\n\t"
 
/*Jasonzhao@viatech.com.cn, I enable cache for 0-7ffff, 80000-9ffff, e0000-fffff;
if 1M cacheable,then when S3 resume, there is stange color on screen for 2 sec. 
suppose problem of a0000-dfffff and cache .
and in x86_setup_fixed_mtrrs()(mtrr.c), 0-256M is set cacheable.*/

        "movl    $0x201, %ecx\n\t"
        "movl    $0x0000000f, %edx\n\t" /* AMD 40 bit 0xff*/
 	 			"movl    $((~(( 0 + 0x80000) - 1)) | 0x800), %eax\n\t"
        "wrmsr\n\t"
        
        "movl    $0x202, %ecx\n\t"
        "xorl    %edx, %edx\n\t"
        "movl     $(0x80000 | 6), %eax\n\t"
        "orl     $(0 | 6), %eax\n\t"
        "wrmsr\n\t"

        "movl    $0x203, %ecx\n\t"
        "movl    $0x0000000f, %edx\n\t" /* AMD 40 bit 0xff*/
 	 			"movl    $((~(( 0 + 0x20000) - 1)) | 0x800), %eax\n\t"
        "wrmsr\n\t"
        
        "movl    $0x204, %ecx\n\t"
        "xorl    %edx, %edx\n\t"
        "movl     $(0xc0000 | 6), %eax\n\t"
        "orl     $(0 | 6), %eax\n\t"
        "wrmsr\n\t"

        "movl    $0x205, %ecx\n\t"
        "movl    $0x0000000f, %edx\n\t" /* AMD 40 bit 0xff*/
 	 			"movl    $((~(( 0 + 0x40000) - 1)) | 0x800), %eax\n\t"
        "wrmsr\n\t"        
        
	/*jasonzhao@viatech.com.cn add this 2008-11-27, cache CONFIG_XIP_ROM_BASE-SIZE to speedup the coreboot code*/
	 			"movl    $0x206, %ecx\n\t"
        "xorl    %edx, %edx\n\t"
        "movl     $CONFIG_XIP_ROM_BASE,%eax\n\t"
        "orl     $(0 | 6), %eax\n\t"
        "wrmsr\n\t"

	 			"movl    $0x207, %ecx\n\t"
        "xorl    %edx, %edx\n\t"
        "movl     $CONFIG_XIP_ROM_SIZE,%eax\n\t"
        "decl	%eax\n\t"
        "notl	%eax\n\t"
        "orl     $(0 | 0x800), %eax\n\t"
        "wrmsr\n\t"

        /* enable cache */
        "movl    %cr0, %eax\n\t"
        "andl    $0x9fffffff,%eax\n\t"
        "movl    %eax, %cr0\n\t"
	 			"invd\n\t"

	/* FIXME: These values might have to change for suspend-to-ram.
	   the 0x00400000 was chosen as this is a place in memory that
	   should exist in all contemporary configurations (ie. large
	   enough RAM), but doesn't collide with anything coreboot does.
	   Other than that, it's arbitrary. */
	"movl	$0x00400000,%esp\n\t"
	"movl	%esp,%ebp\n\t"
        );
