#ifndef CPU_P6_CPUFIXUP_H
#define CPU_P6_CPUFIXUP_H

#ifdef UPDATE_MICROCODE
#define CPU_FIXUP
#endif

void p6_cpufixup(unsigned long totalram);

#endif /* CPU_P6_CPUFIXUP_H */
