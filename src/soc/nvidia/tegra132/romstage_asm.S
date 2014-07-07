/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/asm.h>
#include "stack.S"

        .section ".text", "ax", %progbits

ENTRY(main)
	stack_init stack=.Stack size=.Stack_size seed=0 func=romstage
ENDPROC(main)

/* we do it this way because it's a 32-bit constant and
 * in some cases too far away to be loaded as just an offset
 * from IP
 */
.align 2
.Stack:
	.word CONFIG_ROMSTAGE_STACK_TOP
.align 2
.Stack_size:
	.word CONFIG_ROMSTAGE_STACK_TOP - CONFIG_ROMSTAGE_STACK_BOTTOM

