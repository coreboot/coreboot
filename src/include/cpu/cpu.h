#ifndef CPU_CPU_H
#define CPU_CPU_H

#include <mem.h>

unsigned long cpu_initialize(struct mem_range *mem);
#define CPU_ENABLED		1	/* Processor is available */
#define CPU_BOOTPROCESSOR	2	/* Processor is the BP */

#define __cpu_driver __attribute__ ((unused,__section__(".rodata.cpu_driver")))
/** start of compile time generated pci driver array */
extern struct pci_driver cpu_drivers[];
/** end of compile time generated pci driver array */
extern struct pci_driver ecpu_drivers[];

#endif /* CPU_CPU_H */
