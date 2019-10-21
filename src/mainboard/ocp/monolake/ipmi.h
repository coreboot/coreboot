/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 Wiwynn Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef MONOLAKE_IPMI_H
#define MONOLAKE_IPMI_H
#include <types.h>

#define IPMI_NETFN_OEM 0x30
#define IPMI_OEM_SET_BIOS_BOOT_ORDER 0x52
#define IPMI_OEM_GET_BIOS_BOOT_ORDER 0x53
#define GET_CMOS_BIT(x) ((x) & (1 << 1))
#define GET_VALID_BIT(x) ((x) & (1 << 7))
#define CLEAR_CMOS_AND_VALID_BIT(x) ((x) &= 0x7d)
#define BMC_KCS_BASE 0xca2
typedef struct {
	u8 BootMode; /* Bit 1:CMOS clear, bit 7:valid bit. */
	u8 Boot0000;
	u8 Boot0001;
	u8 Boot0002;
	u8 Boot0003;
	u8 Boot0004;
} __packed ipmi_oem_req_t;

typedef struct {
	u16 KcsRsp;
	u8 CompletionCode;
	ipmi_oem_req_t Data;
} __packed ipmi_oem_rsp_t;

/*
 * IPMI get response to check if valid and CMOS clear bit
 * are both set and store the IPMI response data to the parameter.
 */
int is_ipmi_clear_cmos_set(ipmi_oem_rsp_t *rsp);
/*
 * Clear valid bit and CMOS clear bit from the parameter
 * and set it back via IPMI.
 */
void clear_ipmi_flags(ipmi_oem_rsp_t *rsp);

#endif
