/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_UTIL_H_
#define _SOC_UTIL_H_

#include <hob_iiouds.h>
#include <hob_memmap.h>
#include <hob_cxlnode.h>
#include <hob_systeminfo.h>
#include <hob_enhancedwarningloglib.h>

const struct SystemMemoryMapHob *get_system_memory_map(void);
const struct SystemMemoryMapElement *get_system_memory_map_elment(uint8_t *num);

uint32_t get_socket_stack_busno(uint32_t socket, uint32_t stack);

bool is_iio_cxl_stack_res(const STACK_RES *res);

void soc_display_iio_universal_data_hob(const IIO_UDS *hob);
void soc_display_memmap_hob(const struct SystemMemoryMapHob **hob_addr);

const CXL_NODE_SOCKET *get_cxl_node(void);
uint8_t get_cxl_node_count(void);

const SYSTEM_INFO_VAR *get_system_info_hob(void);

const EWL_PRIVATE_DATA *get_ewl_hob(void);

uint32_t get_ubox_busno(uint32_t socket, uint8_t offset);
uint32_t get_socket_ubox_busno(uint32_t socket);

#endif /* _SOC_UTIL_H_ */
