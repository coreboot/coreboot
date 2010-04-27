#ifndef CPU_X86_PAE_H
#define CPU_X86_PAE_H

#define MAPPING_ERROR ((void *)0xffffffffUL)
void *map_2M_page(unsigned long page);

#endif /* CPU_X86_PAE_H  */
