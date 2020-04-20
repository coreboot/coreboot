/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <drivers/ipmi/ipmi_kcs.h>

#include "ipmi.h"

void ipmi_set_ppin(struct ppin_req *req)
{
	int ret;
	struct ipmi_rsp rsp;

	ret = ipmi_kcs_message(CONFIG_BMC_KCS_BASE, IPMI_NETFN_OEM, 0x0, IPMI_OEM_SET_PPIN,
			(const unsigned char *) req, sizeof(*req),
			(unsigned char *) &rsp, sizeof(rsp));

	if (ret < sizeof(struct ipmi_rsp) || rsp.completion_code) {
		printk(BIOS_ERR, "IPMI: %s command failed (ret=%d resp=0x%x)\n",
			__func__, ret, rsp.completion_code);
		return;
	}
	printk(BIOS_DEBUG, "IPMI Set PPIN to BMC done.\n");
}
