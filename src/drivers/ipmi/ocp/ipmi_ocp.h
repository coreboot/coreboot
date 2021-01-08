/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __IPMI_OCP_H
#define __IPMI_OCP_H

#include <cpu/x86/msr.h>
#include <cpu/x86/name.h>
#include "drivers/ipmi/ipmi_kcs.h"

#define IPMI_NETFN_OEM				0x30
#define  IPMI_OEM_SET_PPIN			0x77
#define  IPMI_BMC_SET_POST_START		0x73
#define  IPMI_OEM_SET_BIOS_BOOT_ORDER		0x52
#define  IPMI_OEM_GET_BIOS_BOOT_ORDER		0x53

#define IPMI_NETFN_OEM_COMMON			0x36
#define  IPMI_BMC_SET_PROCESSOR_INFORMATION     0x10
#define  IPMI_BMC_GET_PROCESSOR_INFORMATION     0x11

#define MSR_CORE_THREAD_COUNT	0x35
#define MSR_PLATFORM_INFO	0xce

#define CMOS_BIT  (1 << 1)
#define VALID_BIT (1 << 7)
#define CLEAR_CMOS_AND_VALID_BIT(x) ((x) &= ~(CMOS_BIT | VALID_BIT))
#define SET_CMOS_AND_VALID_BIT(x)   ((x) |= (CMOS_BIT | VALID_BIT))
#define IS_CMOS_AND_VALID_BIT(x)    ((x)&CMOS_BIT && (x)&VALID_BIT)

struct ipmi_processor_info_req {
	uint8_t manufacturer_id[3];
	uint8_t index;
	uint8_t parameter_selector;
} __packed;

struct ipmi_processor_info_param1_req {
	struct ipmi_processor_info_req data;
	char product_name[48];
} __packed;

struct ipmi_processor_info_param2_req {
	struct ipmi_processor_info_req data;
	uint8_t core_number;
	uint16_t thread_number;
	uint16_t processor_freq;
	char revision[2];
} __packed;

struct ppin_req {
	uint32_t cpu0_lo;
	uint32_t cpu0_hi;
	uint32_t cpu1_lo;
	uint32_t cpu1_hi;
} __packed;

struct boot_order {
	uint8_t boot_mode;
	uint8_t boot_dev0;
	uint8_t boot_dev1;
	uint8_t boot_dev2;
	uint8_t boot_dev3;
	uint8_t boot_dev4;
} __packed;

enum cb_err ipmi_set_post_start(const int port);
enum cb_err ipmi_set_cmos_clear(void);
#endif
