/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __ARM_ASM_H
#define __ARM_ASM_H

/* __arm__ is defined regardless of Thumb mode, so need to order this right */
#if defined __thumb2__
#  define ARM(x...)
#  define THUMB(x...)	x
#  define W(instr)	instr.w
#  if __COREBOOT_ARM_ARCH__ < 7
#    error thumb mode has not been tested with ARM < v7!
#  endif
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
