#ifndef CPU_P5_CPUID_H
#define CPU_P5_CPUID_H

int mtrr_check(void);
void display_cpuid(void);

/*
 *      Generic CPUID function. copied from Linux kernel headers
 */

static inline void cpuid(int op, int *eax, int *ebx, int *ecx, int *edx)
{
        __asm__("pushl %%ebx\n\t"
		"cpuid\n\t"
		"movl	%%ebx, %%esi\n\t"
		"popl	%%ebx\n\t"
                : "=a" (*eax),
                  "=S" (*ebx),
                  "=c" (*ecx),
                  "=d" (*edx)
                : "a" (op)
                : "cc");
}

#endif /* CPU_P5_CPUID_H */
