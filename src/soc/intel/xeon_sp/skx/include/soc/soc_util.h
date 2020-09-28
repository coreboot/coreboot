/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_UTIL_H_
#define _SOC_UTIL_H_

#include <hob_iiouds.h>
#include <hob_memmap.h>
#include <acpi/acpi.h>
#include <soc/util.h>

struct iiostack_resource {
	uint8_t     no_of_stacks;
	STACK_RES   res[CONFIG_MAX_SOCKET * MAX_IIO_STACK];
};

uintptr_t get_tolm(uint32_t bus);
void get_tseg_base_lim(uint32_t bus, uint32_t *base, uint32_t *limit);
uintptr_t get_cha_mmcfg_base(uint32_t bus);
uint32_t top_of_32bit_ram(void); // Top of 32bit usable memory

uint32_t pci_read_mmio_reg(int bus, uint32_t dev, uint32_t func, int offset);

uint32_t get_socket_stack_busno(uint32_t socket, uint32_t stack);
void get_iiostack_info(struct iiostack_resource *info);

int get_threads_per_package(void);
int get_platform_thread_count(void);
void get_core_thread_bits(uint32_t *core_bits, uint32_t *thread_bits);

void get_cpu_info_from_apicid(uint32_t apicid, uint32_t core_bits,
	uint32_t thread_bits, uint8_t *package, uint8_t *core, uint8_t *thread);

void xeonsp_init_cpu_config(void);
void set_bios_init_completion(void);
void config_reset_cpl3_csrs(void);

#endif /* _SOC_UTIL_H_ */
