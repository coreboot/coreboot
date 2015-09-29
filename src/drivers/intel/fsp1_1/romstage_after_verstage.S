/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc
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
 * Foundation, Inc.
 */

#define LHLT_DELAY	0x50000		/* I/O delay between post codes on failure */

.text
.global _start
_start:
	/* This is the romstage entry point when CONFIG_SEPARATE_VERSTAGE
	 * is used. The stack, descriptors, and gdt are already initialized
	 * by verstage. However, in order to maintain the semantics of
	 * CAR_GLOBAL variables we need to clear those to zero. */
	cld
	xor	%eax, %eax
	movl	$(_car_global_end), %ecx
	movl	$(_car_global_start), %edi
	sub	%edi, %ecx
	rep	stosl
	call	romstage_after_verstage
	#include "after_raminit.S"

	movb	$0x69, %ah
	jmp	.Lhlt

.Lhlt:
	xchg	%al, %ah
#if IS_ENABLED(CONFIG_POST_IO)
	outb	%al, $CONFIG_POST_IO_PORT
#else
	post_code(POST_DEAD_CODE)
#endif
	movl	$LHLT_DELAY, %ecx
.Lhlt_Delay:
	outb	%al, $0xED
	loop	.Lhlt_Delay
	jmp	.Lhlt
