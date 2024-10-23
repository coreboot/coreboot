/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_UTIL_H_
#define _SOC_UTIL_H_

#include <hob_iiouds.h>
#include <hob_memmap.h>

#define xSTACK_RES STACK_RES
#define xIIO_RESOURCE_INSTANCE IIO_RESOURCE_INSTANCE

/*
 * Address of the MRC status byte in CMOS. Should be reserved
 * in mainboards' cmos.layout and not covered by checksum.
 */
#define CMOS_OFFSET_MRC_STATUS 0x47

#if CONFIG(USE_OPTION_TABLE)
#include "option_table.h"
#if CMOS_VSTART_mrc_status != CMOS_OFFSET_MRC_STATUS * 8
#error "CMOS start for CPX-SP MRC status byte is not correct, check your cmos.layout"
#endif
#if CMOS_VLEN_mrc_status != 8
#error "CMOS length for CPX-SP MRC status byte is not correct, check your cmos.layout"
#endif
#endif

struct iiostack_resource {
	uint8_t     no_of_stacks;
	STACK_RES   res[CONFIG_MAX_SOCKET * MAX_IIO_STACK];
};

void get_iiostack_info(struct iiostack_resource *info);

const struct SystemMemoryMapHob *get_system_memory_map(void);

uint8_t get_cxl_node_count(void);

void soc_set_mrc_cold_boot_flag(bool cold_boot_required);

#endif /* _SOC_UTIL_H_ */
