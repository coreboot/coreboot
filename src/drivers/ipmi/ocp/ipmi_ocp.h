/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __IPMI_OCP_H
#define __IPMI_OCP_H

#include <cpu/x86/msr.h>
#include <cpu/x86/name.h>
#include "drivers/ipmi/ipmi_kcs.h"

#define IPMI_NETFN_OEM_COMMON			0x36
#define  IPMI_BMC_SET_PROCESSOR_INFORMATION     0x10
#define  IPMI_BMC_GET_PROCESSOR_INFORMATION     0x11

#define MSR_CORE_THREAD_COUNT	0x35
#define MSR_PLATFORM_INFO	0xce

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

#endif
