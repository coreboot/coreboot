/*
 * This file is part of the coreboot project.
 * 
 * Copyright (C) 2007-2008 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
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

        __asm__ volatile (

	"movb	$0x30, %al\noutb %al, $0x80\n"

	/* Disable Cache */
	"movl	%cr0, %eax\n"
	"orl    $(1 << 30), %eax\n"
	"movl	%eax, %cr0\n"

	"movb	$0x31, %al\noutb %al, $0x80\n"

	/* Disable MTRR */
	"movl	$MTRRdefType_MSR, %ecx\n"
	"rdmsr\n"
	"andl	$(~(1 << 11)), %eax\n"
	"wrmsr\n"

	"movb	$0x32, %al\noutb %al, $0x80\n"

	"invd\n"
#if 0
	"xorl	%eax, %eax\n"
	"xorl	%edx, %edx\n"
	"movl	$MTRRphysBase_MSR(0), %ecx\n"
	"wrmsr\n"
	"movl	$MTRRphysMask_MSR(0), %ecx\n"
	"wrmsr\n"
	"movl	$MTRRphysBase_MSR(1), %ecx\n"
	"wrmsr\n"
	"movl	$MTRRphysMask_MSR(1), %ecx\n"
	"wrmsr\n"
#endif

	"movb	$0x33, %al\noutb %al, $0x80\n"
#ifdef CLEAR_FIRST_1M_RAM
	"movb	$0x34, %al\noutb %al, $0x80\n"
	/* Enable Write Combining and Speculative Reads for the first 1MB */
	"movl	$MTRRphysBase_MSR(0), %ecx\n"
	"movl	$(0x00000000 | MTRR_TYPE_WRCOMB), %eax\n"
	"xorl	%edx, %edx\n"
	"wrmsr\n"
	"movl	$MTRRphysMask_MSR(0), %ecx\n"
	"movl	$(~(1024*1024 -1) | (1 << 11)), %eax\n"
	"movl	$0x0000000f, %edx\n"	// 36bit address space
	"wrmsr\n"
	"movb	$0x35, %al\noutb %al, $0x80\n"
#endif

	/* Enable Cache */
	"movl	%cr0, %eax\n"
	"andl    $~( (1 << 30) | (1 << 29) ), %eax\n"
	"movl	%eax, %cr0\n"

	"movb	$0x36, %al\noutb %al, $0x80\n"
#ifdef CLEAR_FIRST_1M_RAM

	/* Clear first 1MB of RAM */
	"movl	$0x00000000, %edi\n"
	"cld\n"
	"xorl	%eax, %eax\n"
	"movl	$((1024*1024) / 4), %ecx\n"
	"rep stosl\n"
	
	"movb	$0x37, %al\noutb %al, $0x80\n"
#endif

	/* Disable Cache */
	"movl	%cr0, %eax\n"
	"orl    $(1 << 30), %eax\n"
	"movl	%eax, %cr0\n"

	"movb	$0x38, %al\noutb %al, $0x80\n"

	/* Enable Write Back and Speculative Reads for the first 1MB */
	"movl	$MTRRphysBase_MSR(0), %ecx\n"
	"movl	$(0x00000000 | MTRR_TYPE_WRBACK), %eax\n"
	"xorl	%edx, %edx\n"
	"wrmsr\n"
	"movl	$MTRRphysMask_MSR(0), %ecx\n"
	"movl	$(~(1024*1024 -1) | (1 << 11)), %eax\n"
	"movl	$0x0000000f, %edx	// 36bit address space\n"
	"wrmsr\n"

	"movb	$0x39, %al\noutb %al, $0x80\n"

	/* And Enable Cache again after setting MTRRs */
	"movl	%cr0, %eax\n"
	"andl    $~( (1 << 30) | (1 << 29) ), %eax\n"
	"movl	%eax, %cr0\n"

	"movb	$0x3a, %al\noutb %al, $0x80\n"

	/* Enable MTRR */
	"movl	$MTRRdefType_MSR, %ecx\n"
	"rdmsr\n"
	"orl	$(1 << 11), %eax\n"
	"wrmsr\n"

	"movb	$0x3b, %al\noutb %al, $0x80\n"

	/* Enable prefetchers */
	"movl	$0x01a0, %eax\n"
	"rdmsr\n"
	"andl	$~((1 << 9) | (1 << 19)), %eax\n"
	"andl	$~((1 << 5) | (1 << 7)), %edx\n"
	"wrmsr\n"

	/* Invalidate the cache again */
	"invd\n"
	"movb	$0x3c, %al\noutb %al, $0x80\n"
        );
