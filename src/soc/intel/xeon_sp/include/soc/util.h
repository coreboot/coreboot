/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _XEON_SP_SOC_UTIL_H_
#define _XEON_SP_SOC_UTIL_H_

#include <cpu/x86/msr.h>
#include <hob_iiouds.h>

void get_cpubusnos(uint32_t *bus0, uint32_t *bus1, uint32_t *bus2, uint32_t *bus3);
void unlock_pam_regions(void);
void get_stack_busnos(uint32_t *bus);
msr_t read_msr_ppin(void);
int get_threads_per_package(void);
int get_platform_thread_count(void);
const IIO_UDS *get_iio_uds(void);
unsigned int soc_get_num_cpus(void);
void xeonsp_init_cpu_config(void);
void set_bios_init_completion(void);

#endif
