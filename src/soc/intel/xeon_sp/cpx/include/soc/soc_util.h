/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_UTIL_H_
#define _SOC_UTIL_H_

#include <hob_iiouds.h>
#include <hob_memmap.h>

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

const struct SystemMemoryMapHob *get_system_memory_map(void);

uint8_t socket0_get_ubox_busno(const uint8_t stack);
uint8_t get_cxl_node_count(void);

int soc_get_stack_for_port(int port);
void soc_set_mrc_cold_boot_flag(bool cold_boot_required);

#endif /* _SOC_UTIL_H_ */
