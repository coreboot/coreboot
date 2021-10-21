/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <drivers/ipmi/ipmi_if.h>

#include "ipmi_ocp.h"

enum cb_err ipmi_set_post_start(const int port)
{
	int ret;
	struct ipmi_rsp rsp;

	ret = ipmi_message(port, IPMI_NETFN_OEM, 0x0,
			   IPMI_BMC_SET_POST_START, NULL, 0, (u8 *) &rsp,
			   sizeof(rsp));

	if (ret < sizeof(struct ipmi_rsp) || rsp.completion_code) {
		printk(BIOS_ERR, "IPMI: %s command failed (ret=%d rsp=0x%x)\n",
		       __func__, ret, rsp.completion_code);
		return CB_ERR;
	}
	if (ret != sizeof(rsp)) {
		printk(BIOS_ERR, "IPMI: %s response truncated\n", __func__);
		return CB_ERR;
	}

	printk(BIOS_DEBUG, "IPMI BMC POST is started\n");
	return CB_SUCCESS;
}

enum cb_err ipmi_set_cmos_clear(void)
{
	int ret;

	struct ipmi_oem_rsp {
		struct ipmi_rsp resp;
		struct boot_order data;
	} __packed;

	struct ipmi_oem_rsp rsp;
	struct boot_order req;

	/* IPMI OEM get bios boot order command to check if the valid bit and
	   the CMOS clear bit are both set from the response BootMode byte. */
	ret = ipmi_message(CONFIG_BMC_KCS_BASE, IPMI_NETFN_OEM, 0x0,
			   IPMI_OEM_GET_BIOS_BOOT_ORDER,
			   NULL, 0,
			   (unsigned char *) &rsp, sizeof(rsp));

	if (ret < sizeof(struct ipmi_rsp) || rsp.resp.completion_code) {
		printk(BIOS_ERR, "IPMI: %s command failed (read ret=%d resp=0x%x)\n",
			__func__, ret, rsp.resp.completion_code);
		return CB_ERR;
	}

	if (!IS_CMOS_AND_VALID_BIT(rsp.data.boot_mode)) {
		req = rsp.data;
		SET_CMOS_AND_VALID_BIT(req.boot_mode);
		ret = ipmi_message(CONFIG_BMC_KCS_BASE, IPMI_NETFN_OEM, 0x0,
				   IPMI_OEM_SET_BIOS_BOOT_ORDER,
				   (const unsigned char *) &req, sizeof(req),
				   (unsigned char *) &rsp, sizeof(rsp));

		if (ret < sizeof(struct ipmi_rsp) || rsp.resp.completion_code) {
			printk(BIOS_ERR, "IPMI: %s command failed (sent ret=%d resp=0x%x)\n",
				__func__, ret, rsp.resp.completion_code);
			return CB_ERR;
		}

		printk(BIOS_INFO, "IPMI CMOS clear requested because CMOS data is invalid.\n");

		return CB_SUCCESS;
	}

	return CB_SUCCESS;
}
