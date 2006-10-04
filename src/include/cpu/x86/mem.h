#ifndef CPU_X86_MEM_H
#define CPU_X86_MEM_H

/* Optimized generic x86 assembly for clearing memory */
static inline void clear_memory(void *addr, unsigned long size)
{
        asm volatile(
                "cld \n\t"
                "rep; stosl\n\t"
                : /* No outputs */
                : "a" (0), "D" (addr), "c" (size>>2)
                );

}

#endif /* CPU_X86_MEM_H */
