/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _XEON_SP_SOC_UTIL_H_
#define _XEON_SP_SOC_UTIL_H_

#include <cpu/x86/msr.h>
#include <intelblocks/p2sb.h>
#include <soc/soc_util.h>

#define MEM_ADDR_64MB_SHIFT_BITS 26

msr_t read_msr_ppin(void);
int get_platform_thread_count(void);
const IIO_UDS *get_iio_uds(void);
unsigned int soc_get_num_cpus(void);
bool soc_cpu_is_enabled(const size_t idx);
void set_bios_init_completion(void);
uint8_t soc_get_iio_ioapicid(int socket, int stack);

bool is_memtype_non_volatile(uint16_t mem_type);
bool is_memtype_reserved(uint16_t mem_type);
bool is_memtype_processor_attached(uint16_t mem_type);

bool is_pcie_iio_stack_res(const xSTACK_RES *res);
bool is_ubox_stack_res(const xSTACK_RES *res);
bool is_ioat_iio_stack_res(const xSTACK_RES *res);
bool is_iio_cxl_stack_res(const xSTACK_RES *res);
void bios_done_msr(void *unused);
union p2sb_bdf soc_get_hpet_bdf(void);
union p2sb_bdf soc_get_ioapic_bdf(void);

bool get_mmio_high_base_size(resource_t *base, resource_t *size);

#endif
