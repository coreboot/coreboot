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

size_t get_pstate_info(struct acpi_sw_pstate *pstate_values,
		       struct acpi_xpss_sw_pstate *pstate_xpss_values);
const acpi_cstate_t *get_cstate_config_data(size_t *size);

#endif /* AMD_BLOCK_CPU_H */
