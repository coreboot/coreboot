#ifndef CPU_X86_PAE_H
#define CPU_X86_PAE_H

#include <stdint.h>

/* Set/Clear NXE bit in IA32_EFER MSR */
void paging_set_nxe(int enable);

/* Set PAT MSR */
void paging_set_pat(uint64_t pat);

#define MAPPING_ERROR ((void *)0xffffffffUL)
void *map_2M_page(unsigned long page);

#endif /* CPU_X86_PAE_H  */
