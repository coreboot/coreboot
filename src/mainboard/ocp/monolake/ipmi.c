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
#include <stdint.h>
#include <drivers/ipmi/ipmi_kcs.h>
#include <console/console.h>
#include "ipmi.h"

int is_ipmi_clear_cmos_set(ipmi_oem_rsp_t *rsp)
{
	int ret;
	ipmi_oem_req_t req;

	if (rsp == NULL) {
		printk(BIOS_ERR, "%s failed, null pointer parameter\n",
			 __func__);
		return 0;
	}
	/* IPMI OEM get bios boot order command to check if the valid bit and
	   the CMOS clear bit are both set from the response BootMode byte. */
	ret = ipmi_kcs_message(BMC_KCS_BASE, IPMI_NETFN_OEM, 0x0,
			IPMI_OEM_GET_BIOS_BOOT_ORDER,
			(const unsigned char *) &req, sizeof(ipmi_oem_req_t),
			(unsigned char *) rsp, sizeof(ipmi_oem_rsp_t));

	if (ret < sizeof(struct ipmi_rsp) || rsp->CompletionCode) {
		printk(BIOS_ERR, "IPMI: %s command failed (ret=%d resp=0x%x)\n",
		__func__, ret, rsp->CompletionCode);
		return 0;
	}

	if (GET_VALID_BIT(rsp->Data.BootMode) && GET_CMOS_BIT(rsp->Data.BootMode)) {
		printk(BIOS_INFO, "IPMI CMOS clear requested\n");
		return 1;
	}

	printk(BIOS_DEBUG, "IPMI CMOS clear is not set\n");
	return 0;
}

void clear_ipmi_flags(ipmi_oem_rsp_t *rsp_get)
{
	int ret;
	ipmi_oem_req_t req;
	struct ipmi_rsp rsp;

	if (rsp_get == NULL) {
		printk(BIOS_ERR, "%s failed, null pointer parameter\n",
			__func__);
		return;
	}

	req = rsp_get->Data;
	CLEAR_CMOS_AND_VALID_BIT(req.BootMode);
	ret = ipmi_kcs_message(BMC_KCS_BASE, IPMI_NETFN_OEM, 0x0,
			IPMI_OEM_SET_BIOS_BOOT_ORDER,
			(const unsigned char *) &req, sizeof(ipmi_oem_req_t),
			(unsigned char *) &rsp, sizeof(rsp));

	if (ret < sizeof(struct ipmi_rsp) || rsp.completion_code) {
		printk(BIOS_ERR, "IPMI: %s command failed (ret=%d resp=0x%x)\n",
			__func__, ret, rsp.completion_code);
		return;
	}

	printk(BIOS_INFO, "clear IPMI flags done\n");
}
