#ifndef ALPHA_ARCH_ASM_H
#define ALPHA_ARCH_ASM_H

/* assembly language utility macros */

/* Load immediate signed constants */
#define LOAD_CONSTANT16(reg, constant) \
	lda	reg, (constant)(zero)

#define LOAD_CONSTANT32(reg, constant) \
	ldah	reg, ((constant + 0x8000) >> 16)(zero) ; \
	lda	reg, (constant & 0xffff)(reg)

/* Declare a global symbol */
#define GLOBL(label) .globl label ;  label:


#endif /* ALPHA_ARCH_ASM_H */
