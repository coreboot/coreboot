#ifndef CPU_CPU_H
#define CPU_CPU_H

struct device;
struct bus;
#include <arch/cpu.h>

void cpu_initialize(void);
void initialize_cpus(struct bus *cpu_bus);
void secondary_cpu_init(void);

#if !defined(CONFIG_WAIT_BEFORE_CPUS_INIT) || CONFIG_WAIT_BEFORE_CPUS_INIT==0
	#define cpus_ready_for_init() do {} while(0)
#else
	void cpus_ready_for_init(void);
#endif

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

#endif /* CPU_CPU_H */
