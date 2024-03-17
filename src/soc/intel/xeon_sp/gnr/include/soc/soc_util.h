/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_UTIL_H_
#define _SOC_UTIL_H_

#include <device/device.h>
#include <device/pci.h>

#include <fsp/util.h>
#include <CxlNodeHob.h>
#include <FspAcpiHobs.h>
#include <IioUniversalDataHob.h>
#include <MemoryMapDataHob.h>

#define xSTACK_RES UDS_STACK_RES
#define xIIO_RESOURCE_INSTANCE UDS_SOCKET_RES

#define FSP_HOB_IIO_UNIVERSAL_DATA_GUID { \
	0xa1, 0x96, 0xf3, 0x7f, 0x7d, 0xee, 0x1e, 0x43, \
	0xba, 0x53, 0x8f, 0xCa, 0x12, 0x7c, 0x44, 0xc0  \
}

const struct SystemMemoryMapHob *get_system_memory_map(void);
const struct SystemMemoryMapElement *get_system_memory_map_elment(uint8_t *num);

const CXL_NODE_SOCKET *get_cxl_node(void);
uint8_t get_cxl_node_count(void);

const char *pciroot_res_to_domain_type(const UDS_STACK_RES *sr, const UDS_PCIROOT_RES *rr);

#endif /* _SOC_UTIL_H_ */
