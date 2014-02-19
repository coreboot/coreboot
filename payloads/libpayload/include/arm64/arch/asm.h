/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
 *
 */

#ifndef __ARM64_ASM_H
#define __ARM64_ASM_H

#  define ARM64(x...)	x
#  define W(instr)	instr

#define ALIGN .align 2

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

#endif	/* __ARM64_ASM_H */
