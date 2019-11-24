/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/******************************************************************************
 * AMD Generic Encapsulated Software Architecture
 *
 * $Workfile:: cache_as_ram.S
 *
 * Description: cache_as_ram.S - AGESA Module Entry Point for GCC complier
 *
 ******************************************************************************
 */

#include "gcccar.inc"
#include <cpu/x86/post_code.h>

.code32
.globl _cache_as_ram_setup, _cache_as_ram_setup_end

_cache_as_ram_setup:

	/* Preserve BIST. */
	movd	%eax, %mm0

	post_code(0xa0)

	AMD_ENABLE_STACK

	/* Align the stack. */
	and	$0xFFFFFFF0, %esp

#ifdef __x86_64__
	/* switch to 64 bit long mode */
	mov	%esi, %ecx
	add	$0, %ecx # core number
	xor	%eax, %eax
	lea	(0x1000+0x23)(%ecx), %edi
	mov	%edi, (%ecx)
	mov	%eax, 4(%ecx)

	lea	0x1000(%ecx), %edi
	movl	$0x000000e3, 0x00(%edi)
	movl	%eax, 0x04(%edi)
	movl	$0x400000e3, 0x08(%edi)
	movl	%eax, 0x0c(%edi)
	movl	$0x800000e3, 0x10(%edi)
	movl	%eax, 0x14(%edi)
	movl	$0xc00000e3, 0x18(%edi)
	movl	%eax, 0x1c(%edi)

	# load ROM based identity mapped page tables
	mov	%ecx, %eax
	mov	%eax, %cr3

	# enable PAE
	mov	%cr4, %eax
	bts	$5, %eax
	mov	%eax, %cr4

	# enable long mode
	mov	$0xC0000080, %ecx
	rdmsr
	bts	$8, %eax
	wrmsr

	# enable paging
	mov	%cr0, %eax
	bts	$31, %eax
	mov	%eax, %cr0

	# use call far to switch to 64-bit code segment
	ljmp	$0x18, $1f
1:

#endif

	/* Must maintain 16-byte stack alignment here. */
	pushl	$0x0
	pushl	$0x0
	pushl	$0x0
	movd	%mm0, %eax		/* bist */
	pushl	%eax
	call	romstage_main

	/* Should never see this postcode */
	post_code(0xae)

stop:
	hlt
	jmp	stop

_cache_as_ram_setup_end:
