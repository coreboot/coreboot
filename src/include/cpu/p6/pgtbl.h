#ifndef CPU_P6_PGTBL_H
#define CPU_P6_PGTBL_H 

#define MAPPING_ERROR ((void *)0xffffffffUL)
void *map_2M_page(int cpu_index, unsigned long page);

#endif /* CPU_P6_PGTBL_H  */
