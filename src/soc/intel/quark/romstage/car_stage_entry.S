/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000,2007 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2007-2008 coresystems GmbH
 * Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
 * Copyright (C) 2015-2016 Intel Corporation
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
 * Replacement for cache_as_ram.inc when using the C environment boot block.
 */

#include <rules.h>
#include <soc/sd.h>

	.section ".text"
	.global car_stage_entry

car_stage_entry:

	/* Enter the C code */
	call	car_stage_c_entry

	/* The code should never reach this point */
	movb	$0x69, %ah
	jmp	.Lhlt

#----------------------------------------------------------------------------
#
# Procedure:	.Lhlt
#
# Input:	ah - Upper 8-bits of POST code
#		al - Lower 8-bits of POST code
#
# Description:
#		Infinite loop displaying alternating POST code values
#
#----------------------------------------------------------------------------

#define FLASH_DELAY	0x1000	/* I/O delay between post codes on failure */
#define POST_DELAY	0x50

.Lhlt:
	xchg	%al, %ah
	mov	$POST_DELAY, %dh
#if CONFIG(POST_IO)
	outb	%al, $CONFIG_POST_IO_PORT
#else
	post_code(POST_DEAD_CODE)
#endif
.flash_setup:
	movl	$FLASH_DELAY, %ecx
.flash_delay:
	outb	%al, $0xED
	loop	.flash_delay
#if CONFIG(ENABLE_DEBUG_LED)
	movl	$SD_HOST_CTRL, %ebx
	movb	0(%ebx), %dl
	xorb	$1, %dl
	movb	%dl, 0(%ebx)
#endif /* CONFIG_ENABLE_DEBUG_LED */
	decb	%dh
	jnz	.flash_setup
	jmp	.Lhlt
