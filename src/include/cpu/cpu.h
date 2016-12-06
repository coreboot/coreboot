#ifndef CPU_CPU_H
#define CPU_CPU_H

#include <arch/cpu.h>

#if !defined(__ROMCC__)
void cpu_initialize(unsigned int cpu_index);
struct bus;
void initialize_cpus(struct bus *cpu_bus);
void asmlinkage secondary_cpu_init(unsigned int cpu_index);

#define __cpu_driver __attribute__ ((used,__section__(".rodata.cpu_driver")))
#ifndef __SIMPLE_DEVICE__
/** start of compile time generated pci driver array */
extern struct cpu_driver _cpu_drivers[];
/** end of compile time generated pci driver array */
extern struct cpu_driver _ecpu_drivers[];
#endif
#endif /* !__ROMCC__ */

#endif /* CPU_CPU_H */
