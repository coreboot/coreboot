#ifndef _INTEL_CPUID_H_
#define _INTEL_CPUID_H_

#ifdef i586
int intel_mtrr_check(void);
#endif
void intel_display_cpuid(void);

/*
 *      Generic CPUID function. copied from Linux kernel headers
 */

extern inline void intel_cpuid(int op, int *eax, int *ebx, int *ecx, int *edx)
{
        __asm__("cpuid"
                : "=a" (*eax),
                  "=b" (*ebx),
                  "=c" (*ecx),
                  "=d" (*edx)
                : "a" (op)
                : "cc");
}


#endif /* _INTEL_CPUID_H_ */
