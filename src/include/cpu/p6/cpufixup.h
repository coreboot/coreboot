#ifndef CPU_P6_CPUFIXUP_H
#define CPU_P6_CPUFIXUP_H

#if UPDATE_MICROCODE == 1
#define CPU_FIXUP 1
#endif

void p6_cpufixup(struct mem_range *mem);

#endif /* CPU_P6_CPUFIXUP_H */
