/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_UTIL_H_
#define _SOC_UTIL_H_

#include <console/console.h>
#include <hob_iiouds.h>
#include <hob_memmap.h>
#include <stdint.h>

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
	STACK_RES   res[MAX_SOCKET * MAX_LOGIC_IIO_STACK];
};

uint8_t get_iiostack_info(struct iiostack_resource *info);

void xeonsp_init_cpu_config(void);
const IIO_UDS *get_iio_uds(void);
void get_core_thread_bits(uint32_t *core_bits, uint32_t *thread_bits);
void get_cpu_info_from_apicid(uint32_t apicid, uint32_t core_bits, uint32_t thread_bits,
	uint8_t *package, uint8_t *core, uint8_t *thread);
/* Return socket count, as obtained from FSP HOB */
unsigned int xeon_sp_get_socket_count(void);

int get_platform_thread_count(void);
int get_threads_per_package(void);
const struct SystemMemoryMapHob *get_system_memory_map(void);

void set_bios_init_completion(void);

#endif /* _SOC_UTIL_H_ */
