/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000,2007 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2007-2008 coresystems GmbH
 * Copyright (C) 2018 Patrick Rudolph <siro@das-labor.org>
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

#include <cpu/x86/cache.h>
#include <cpu/x86/post_code.h>

.global bootblock_pre_c_entry
bootblock_pre_c_entry:

cache_as_ram:
	post_code(0x20)
	/*
	 * Nothing to do here on qemu, RAM works just fine without any
	 * initialization.
	 */

	post_code(0x21)

	movl $_car_stack_end, %esp
	/* Align the stack and keep aligned for call to bootblock_c_entry() */
	and	$0xfffffff0, %esp
	sub	$12, %esp

	/* Clear the cache memory region. This will also clear CAR GLOBAL */
	movl	$_car_region_start, %esi
	movl	%esi, %edi
	movl	$_car_region_end, %ecx
	sub	$_car_region_start, %ecx
	shr	$2, %ecx
	xorl	%eax, %eax
	rep	stosl

	/* Restore the BIST result and timestamps. */
	movd	%mm0, %ebx
	movd	%mm1, %eax
	movd	%mm2, %edx

	pushl	%ebx
	pushl	%edx
	pushl	%eax

before_c_entry:
	post_code(0x29)
	call	bootblock_c_entry_bist
	/* Never returns */
.Lhlt:
	post_code(POST_DEAD_CODE)
	hlt
	jmp	.Lhlt
