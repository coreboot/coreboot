/*
 *
 * Copyright 2013 Google Inc.
 *
 */

#ifndef __RISCV64_ASM_H
#define __RISCV64_ASM_H

#define ENDPROC(name) \
	.type name, %function; \
	END(name)

#define ENTRY(name) \
	.section .text.name, "ax", %progbits; \
	.global name; \
	.align 2; \
	name:

#define END(name) \
	.size name, .-name

#endif	/* __RISCV64_ASM_H */
