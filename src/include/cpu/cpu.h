#ifndef CPU_CPU_H
#define CPU_CPU_H

#include <mem.h>

unsigned long cpu_initialize(struct mem_range *mem);
#define CPU_ENABLED		1	/* Processor is available */
#define CPU_BOOTPROCESSOR	2	/* Processor is the BP */


#endif /* CPU_CPU_H */
