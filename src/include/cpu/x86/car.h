/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000,2007 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2007-2008 coresystems GmbH
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

#include <cpu/x86/mtrr.h>

/* Save the BIST result. */
#define save_bist_result() \
	movl	%eax, %ebp

/* Restore the BIST result. */
#define restore_bist_result() \
	movl	%ebp, %eax

/* Enable cache. */
#define enable_cache() \
	movl	%cr0, %eax; \
	andl	$(~((1 << 30) | (1 << 29))), %eax; \
	movl	%eax, %cr0

/* Disable cache. */
#define disable_cache() \
	movl	%cr0, %eax; \
	orl	$(1 << 30), %eax; \
	movl	%eax, %cr0

/* Enable MTRR. */
#define enable_mtrr() \
	movl	$MTRRdefType_MSR, %ecx; \
	rdmsr; \
	orl	$(1 << 11), %eax; \
	wrmsr

/* Disable MTRR. */
#define disable_mtrr() \
	movl	$MTRRdefType_MSR, %ecx; \
	rdmsr; \
	andl	$(~(1 << 11)), %eax; \
	wrmsr

/* Enable L2 cache. */
#define enable_l2_cache() \
	movl	$0x11e, %ecx; \
	rdmsr; \
	orl	$(1 << 8), %eax; \
	wrmsr

/* Enable SSE. */
#define enable_sse() \
	movl	%cr4, %eax; \
	orl	$(3 << 9), %eax; \
	movl	%eax, %cr4

/* Disable SSE. */
#define disable_sse() \
	movl	%cr4, %eax; \
	andl	$~(3 << 9), %eax; \
	movl	%eax, %cr4

