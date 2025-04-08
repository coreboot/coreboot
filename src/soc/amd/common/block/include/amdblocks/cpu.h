/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_CPU_H
#define AMD_BLOCK_CPU_H

#include <acpi/acpi.h>
#include <types.h>

#define MAX_CSTATE_COUNT	8

void early_cache_setup(void);
int get_cpu_count(void);
unsigned int get_threads_per_core(void);
void set_cstate_io_addr(void);
void write_resume_eip(void);

union pstate_msr; /* proper definition is in soc/msr.h */

uint32_t get_uvolts_from_vid(uint16_t core_vid);
uint32_t get_pstate_0_reg(void);
uint32_t get_pstate_latency(void);
uint32_t get_pstate_core_freq(union pstate_msr pstate_reg);
uint32_t get_pstate_core_uvolts(union pstate_msr pstate_reg);
const acpi_cstate_t *get_cstate_config_data(size_t *size);
void amd_cpu_init(struct device *dev);

#endif /* AMD_BLOCK_CPU_H */
