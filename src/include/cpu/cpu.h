/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CPU_CPU_H
#define CPU_CPU_H

#include <arch/cpu.h>  /* IWYU pragma: export */
#include <stdint.h>

void cpu_initialize(void);
/* Returns default APIC id based on logical_cpu number or < 0 on failure. */
int cpu_get_apic_id(int logical_cpu);
uintptr_t cpu_get_lapic_addr(void);
/* Function to keep track of cpu default apic_id */
void cpu_add_map_entry(unsigned int index);
struct bus;
int cpu_phys_address_size(void);

#if ENV_RAMSTAGE
#define __cpu_driver __attribute__((used, __section__(".rodata.cpu_driver")))
#else
#define __cpu_driver __attribute__((unused))
#endif

/** start of compile time generated pci driver array */
extern struct cpu_driver _cpu_drivers[];
/** end of compile time generated pci driver array */
extern struct cpu_driver _ecpu_drivers[];

#endif /* CPU_CPU_H */
