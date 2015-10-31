/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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

#ifndef __ARM_ASM_H
#define __ARM_ASM_H

/* __arm__ is defined regardless of Thumb mode, so need to order this right */
#if defined __thumb2__
#  define ARM(x...)
#  define THUMB(x...)	x
#  define W(instr)	instr.w
#elif defined __thumb__
#  error You are not compiling Thumb2, this won't work!
#else
#  define ARM(x...)	x
#  define THUMB(x...)
#  define W(instr)	instr
#endif

#define ALIGN .align 0

#define ENDPROC(name) \
	.type name, %function; \
	END(name)

#define ENTRY(name) \
	.section .text.name, "ax", %progbits; \
	.global name; \
	ALIGN; \
	name:

#define END(name) \
	.size name, .-name

/* Thumb code uses the (new) unified assembly syntax. */
THUMB(	.syntax unified )

#endif	/* __ARM_ASM_H */
