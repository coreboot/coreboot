#ifndef CPU_X86_MEM_H
#define CPU_X86_MEM_H

/* Optimized generic x86 assembly for clearing memory */
static inline void clear_memory(void *addr, unsigned long size)
{
	asm volatile(
		"1: \n\t"
		"movl %0, (%1)\n\t"
		"addl $4, %1\n\t"
		"subl $4, %2\n\t"
		"jnz 1b\n\t"
		: /* No outputs */
		: "a" (0), "D" (addr), "c" (size)
		);
}

#endif /* CPU_X86_MEM_H */
