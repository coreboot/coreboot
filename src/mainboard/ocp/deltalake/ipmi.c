/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <drivers/ipmi/ipmi_kcs.h>
#include <drivers/ipmi/ipmi_ops.h>

#include "ipmi.h"

enum cb_err ipmi_set_ppin(struct ppin_req *req)
{
	int ret;
	struct ipmi_rsp rsp;

	ret = ipmi_kcs_message(CONFIG_BMC_KCS_BASE, IPMI_NETFN_OEM, 0x0, IPMI_OEM_SET_PPIN,
			(const unsigned char *) req, sizeof(*req),
			(unsigned char *) &rsp, sizeof(rsp));

	if (ret < sizeof(struct ipmi_rsp) || rsp.completion_code) {
		printk(BIOS_ERR, "IPMI: %s command failed (ret=%d resp=0x%x)\n",
			__func__, ret, rsp.completion_code);
		return CB_ERR;
	}

	return CB_SUCCESS;
}

enum cb_err ipmi_get_pcie_config(uint8_t *pcie_config)
{
	int ret;
	struct ipmi_config_rsp {
		struct ipmi_rsp resp;
		uint8_t config;
	} __packed;
	struct ipmi_config_rsp rsp;

	ret = ipmi_kcs_message(CONFIG_BMC_KCS_BASE, IPMI_NETFN_OEM, 0x0,
			IPMI_OEM_GET_PCIE_CONFIG, NULL, 0, (unsigned char *) &rsp,
			sizeof(rsp));

	if (ret < sizeof(struct ipmi_rsp) || rsp.resp.completion_code) {
		printk(BIOS_ERR, "IPMI: %s command failed (ret=%d resp=0x%x)\n",
			__func__, ret, rsp.resp.completion_code);
		return CB_ERR;
	}
	*pcie_config = rsp.config;

	return CB_SUCCESS;
}

enum cb_err ipmi_get_slot_id(uint8_t *slot_id)
{
	int ret;
	struct ipmi_config_rsp {
		struct ipmi_rsp resp;
		uint8_t board_sku_id;
		uint8_t board_rev_id;
		uint8_t slot_id;
		uint8_t slot_config_id;
	} __packed;
	struct ipmi_config_rsp rsp;

	ret = ipmi_kcs_message(CONFIG_BMC_KCS_BASE, IPMI_NETFN_OEM, 0x0, IPMI_OEM_GET_BOARD_ID,
			NULL, 0, (unsigned char *) &rsp, sizeof(rsp));

	if (ret < sizeof(struct ipmi_rsp) || rsp.resp.completion_code) {
		printk(BIOS_ERR, "IPMI: %s command failed (ret=%d resp=0x%x)\n",
			__func__, ret, rsp.resp.completion_code);
		return CB_ERR;
	}
	*slot_id = rsp.slot_id;

	return CB_SUCCESS;
}
