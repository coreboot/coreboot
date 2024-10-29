/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_UTIL_H_
#define _SOC_UTIL_H_

#include <hob_iiouds.h>
#include <hob_memmap.h>
#include <hob_cxlnode.h>
#include <hob_systeminfo.h>
#include <hob_enhancedwarningloglib.h>

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
#error "CMOS start for SPR-SP MRC status byte is not correct, check your cmos.layout"
#endif
#if CMOS_VLEN_mrc_status != 8
#error "CMOS length for SPR-SP MRC status byte is not correct, check your cmos.layout"
#endif
#endif

/* Equals to MAX_IIO_PORTS_PER_SOCKET - 2 * 8, because IOU5 and IOU6 are not used per socket. */
#define IIO_PORT_SETTINGS (1 + 5 * 8)

const struct SystemMemoryMapHob *get_system_memory_map(void);
const struct SystemMemoryMapElement *get_system_memory_map_elment(uint8_t *num);

void soc_display_iio_universal_data_hob(const IIO_UDS *hob);
void soc_display_memmap_hob(const struct SystemMemoryMapHob **hob_addr);

const CXL_NODE_SOCKET *get_cxl_node(void);
uint8_t get_cxl_node_count(void);

const SYSTEM_INFO_VAR *get_system_info_hob(void);

const EWL_PRIVATE_DATA *get_ewl_hob(void);

void soc_set_mrc_cold_boot_flag(bool cold_boot_required);
void soc_config_iio(FSPM_UPD *mupd, const UPD_IIO_PCIE_PORT_CONFIG_ENTRY
	mb_iio_table[CONFIG_MAX_SOCKET][IIO_PORT_SETTINGS], const UINT8 mb_iio_bifur[CONFIG_MAX_SOCKET][5]);

#endif /* _SOC_UTIL_H_ */
