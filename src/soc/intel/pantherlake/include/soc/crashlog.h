/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_PANTHERLAKE_CRASHLOG_H_
#define _SOC_PANTHERLAKE_CRASHLOG_H_

#include <types.h>

/* DVSEC capability Registers */
#define TEL_DVSEC_OFFSET			0x100
#define TEL_DVSEC_PCIE_CAP_ID			0x0
#define TEL_DVSEV_ID				0x8
#define TEL_DVSEV_DISCOVERY_TABLE_OFFSET	0xC
#define TELEMETRY_EXTENDED_CAP_ID		0x23
#define CRASHLOG_DVSEC_ID			0x04
#define TEL_DVSEC_TBIR_BAR0			0
#define TEL_DVSEC_TBIR_BAR1			1

typedef union {
	struct {
		u32 reserved1		:27;
		u32 set_storage_off	:1;
		u32 set_re_arm		:1;
		u32 reserved2		:1;
		u32 set_clr		:1;
		u32 reserved3		:1;
	} fields;
	u32 data;
} __packed cl_punit_control_interface_t;

#endif /* _SOC_PANTHERLAKE_CRASHLOG_H_ */
