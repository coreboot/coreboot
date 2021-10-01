/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ARM_ARM64_ASM_H
#define __ARM_ARM64_ASM_H

#define ENDPROC(name) \
	.type name, %function; \
	END(name)

#define ENTRY_WITH_ALIGN(name, bits) \
	.section .text.name, "ax", %progbits; \
	.global name; \
	.align bits; \
	name:

#define ENTRY(name) ENTRY_WITH_ALIGN(name, 2)

#define END(name) \
	.size name, .-name

/*
 * Certain SoCs have an alignment requirement for the CPU reset vector.
 * Align to a 64 byte typical cacheline for now.
 */
#define CPU_RESET_ENTRY(name) ENTRY_WITH_ALIGN(name, 6)

#define ENTRY_WEAK(name)	\
	ENTRY(name)		\
	.weak name		\

#endif	/* __ARM_ARM64_ASM_H */
