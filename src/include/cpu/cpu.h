#ifndef CPU_CPU_H
#define CPU_CPU_H

struct device;
struct bus;
#include <arch/cpu.h>

void cpu_initialize(struct cpu_info *info);
void initialize_cpus(struct bus *cpu_bus);
void secondary_cpu_init(struct cpu_info *info);
int run_work(unsigned int core, workfunc f, u32 a, u32 b, u32 c, u32 *retval,
	unsigned int timeout);
void cpu_work(struct cpu_info *info);


#if !CONFIG_WAIT_BEFORE_CPUS_INIT
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
