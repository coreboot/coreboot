/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <drivers/ipmi/ipmi_kcs.h>
#include <drivers/ipmi/ipmi_ops.h>
#include <drivers/vpd/vpd.h>
#include <string.h>

#include "ipmi.h"
#include "vpd.h"

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

void init_frb2_wdt(void)
{

	char val[VPD_LEN];
	/* Enable FRB2 timer by default. */
	u8 enable = 1;
	uint16_t countdown;

	if (vpd_get_bool(FRB2_TIMER, VPD_RW_THEN_RO, &enable)) {
		if (!enable) {
			printk(BIOS_DEBUG, "Disable FRB2 timer\n");
			ipmi_stop_bmc_wdt(CONFIG_BMC_KCS_BASE);
			return;
		}
	}
	if (enable) {
		if (vpd_gets(FRB2_COUNTDOWN, val, VPD_LEN, VPD_RW_THEN_RO)) {
			countdown = (uint16_t)atol(val);
			printk(BIOS_DEBUG, "FRB2 timer countdown set to: %d ms\n",
				countdown * 100);
		} else {
			printk(BIOS_DEBUG, "FRB2 timer use default value: %d ms\n",
				DEFAULT_COUNTDOWN * 100);
			countdown = DEFAULT_COUNTDOWN;
		}
		ipmi_init_and_start_bmc_wdt(CONFIG_BMC_KCS_BASE, countdown,
			TIMEOUT_HARD_RESET);
	}
}
