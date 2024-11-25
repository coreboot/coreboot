/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_UTIL_H_
#define _SOC_UTIL_H_

#include <hob_iiouds.h>
#include <hob_memmap.h>

#define xSTACK_RES STACK_RES
#define xIIO_RESOURCE_INSTANCE IIO_RESOURCE_INSTANCE

const struct SystemMemoryMapHob *get_system_memory_map(void);

uint8_t get_cxl_node_count(void);

#endif /* _SOC_UTIL_H_ */
