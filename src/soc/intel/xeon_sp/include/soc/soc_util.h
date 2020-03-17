/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#ifndef _SOC_UTIL_H_
#define _SOC_UTIL_H_

#include <hob_iiouds.h>
#include <hob_memmap.h>
#include <arch/acpi.h>

#define LOG_MEM_RESOURCE(type, dev, index, base_kb, size_kb) \
	printk(BIOS_SPEW, "%s:%d res: %s, dev: %s, index: 0x%x, base: 0x%llx, " \
		"end: 0x%llx, size_kb: 0x%llx\n", \
		__func__, __LINE__, type, dev_path(dev), index, (base_kb << 10), \
		(base_kb << 10) + (size_kb << 10) - 1, size_kb)

#define LOG_IO_RESOURCE(type, dev, index, base, size) \
	printk(BIOS_SPEW, "%s:%d res: %s, dev: %s, index: 0x%x, base: 0x%llx, " \
		"end: 0x%llx, size: 0x%llx\n", \
		__func__, __LINE__, type, dev_path(dev), index, base, base + size - 1, size)

#define DEV_FUNC_ENTER(dev) \
	printk(BIOS_SPEW, "%s:%s:%d: ENTER (dev: %s)\n", \
		__FILE__, __func__, __LINE__, dev_path(dev))

#define DEV_FUNC_EXIT(dev) \
	printk(BIOS_SPEW, "%s:%s:%d: EXIT (dev: %s)\n", __FILE__, \
		__func__, __LINE__, dev_path(dev))

#define FUNC_ENTER() \
	printk(BIOS_SPEW, "%s:%s:%d: ENTER\n", __FILE__, __func__, __LINE__)

#define FUNC_EXIT() \
	printk(BIOS_SPEW, "%s:%s:%d: EXIT\n", __FILE__, __func__, __LINE__)

struct iiostack_resource {
	uint8_t     no_of_stacks;
	STACK_RES   res[CONFIG_MAX_SOCKET * MAX_IIO_STACK];
};

uintptr_t get_tolm(uint32_t bus);
void get_tseg_base_lim(uint32_t bus, uint32_t *base, uint32_t *limit);
uintptr_t get_cha_mmcfg_base(uint32_t bus);
uint32_t top_of_32bit_ram(void); // Top of 32bit usable memory

uint32_t pci_read_mmio_reg(int bus, uint32_t dev, uint32_t func, int offset);

void get_stack_busnos(uint32_t *bus);
void get_cpubusnos(uint32_t *bus0, uint32_t *bus1, uint32_t *bus2, uint32_t *bus3);
uint32_t get_socket_stack_busno(uint32_t socket, uint32_t stack);
void get_iiostack_info(struct iiostack_resource *info);

int get_threads_per_package(void);
int get_platform_thread_count(void);
void get_core_thread_bits(uint32_t *core_bits, uint32_t *thread_bits);

unsigned int get_srat_memory_entries(acpi_srat_mem_t *srat_mem);
void get_cpu_info_from_apicid(uint32_t apicid, uint32_t core_bits,
	uint32_t thread_bits, uint8_t *package, uint8_t *core, uint8_t *thread);

void unlock_pam_regions(void);
void xeonsp_init_cpu_config(void);
void set_bios_init_completion(void);
void config_reset_cpl3_csrs(void);

#endif /* _SOC_UTIL_H_ */
