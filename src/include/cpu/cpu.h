#ifndef CPU_CPU_H
#define CPU_CPU_H

#include <arch/cpu.h>

#if !defined(__PRE_RAM__) && !defined(__SMM__)
void cpu_initialize(unsigned int cpu_index);
struct bus;
void initialize_cpus(struct bus *cpu_bus);
void asmlinkage secondary_cpu_init(unsigned int cpu_index);

#if CONFIG_HAVE_SMI_HANDLER
void smm_init(void);
void smm_lock(void);
void smm_setup_structures(void *gnvs, void *tcg, void *smi1);
#endif

#define __cpu_driver __attribute__ ((used,__section__(".rodata.cpu_driver")))
/** start of compile time generated pci driver array */
extern struct cpu_driver cpu_drivers[];
/** end of compile time generated pci driver array */
extern struct cpu_driver ecpu_drivers[];
#endif /* !__PRE_RAM__ && !__SMM__ */

#endif /* CPU_CPU_H */
