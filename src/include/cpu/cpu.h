#ifndef CPU_CPU_H
#define CPU_CPU_H

struct device;
#include <arch/cpu.h>

void cpu_initialize(void);
void initialize_cpus(struct device *root);

#define __cpu_driver __attribute__ ((used,__section__(".rodata.cpu_driver")))
/** start of compile time generated pci driver array */
extern struct cpu_driver cpu_drivers[];
/** end of compile time generated pci driver array */
extern struct cpu_driver ecpu_drivers[];

#endif /* CPU_CPU_H */
