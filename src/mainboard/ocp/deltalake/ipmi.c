/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <drivers/ipmi/ipmi_if.h>
#include <drivers/ipmi/ipmi_ops.h>
#include <drivers/ipmi/ocp/ipmi_ocp.h>
#include <drivers/vpd/vpd.h>
#include <types.h>

#include "ipmi.h"
#include "vpd.h"

enum cb_err ipmi_get_pcie_config(uint8_t *pcie_config)
{
	int ret;
	struct ipmi_config_rsp {
		struct ipmi_rsp resp;
		uint8_t config;
	} __packed;
	struct ipmi_config_rsp rsp;

	ret = ipmi_message(CONFIG_BMC_KCS_BASE, IPMI_NETFN_OEM, 0x0,
			IPMI_OEM_GET_PCIE_CONFIG, NULL, 0, (unsigned char *)&rsp,
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

	ret = ipmi_message(CONFIG_BMC_KCS_BASE, IPMI_NETFN_OEM, 0x0, IPMI_OEM_GET_BOARD_ID,
			NULL, 0, (unsigned char *)&rsp, sizeof(rsp));

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
	uint8_t enable;
	int action, countdown;

	if (vpd_get_bool(FRB2_TIMER, VPD_RW_THEN_RO, &enable)) {
		printk(BIOS_DEBUG, "Got VPD %s value: %d\n", FRB2_TIMER, enable);
	} else {
		printk(BIOS_INFO, "Not able to get VPD %s, default set to %d\n", FRB2_TIMER,
			FRB2_TIMER_DEFAULT);
		enable = FRB2_TIMER_DEFAULT;
	}

	if (enable) {
		if (vpd_get_int(FRB2_COUNTDOWN, VPD_RW_THEN_RO, &countdown)) {
			printk(BIOS_DEBUG, "FRB2 timer countdown set to: %d ms\n",
				countdown * 100);
		} else {
			printk(BIOS_DEBUG, "FRB2 timer use default value: %d ms\n",
				FRB2_COUNTDOWN_DEFAULT * 100);
			countdown = FRB2_COUNTDOWN_DEFAULT;
		}

		if (vpd_get_int(FRB2_ACTION, VPD_RW_THEN_RO, &action)) {
			printk(BIOS_DEBUG, "FRB2 timer action set to: %d\n", action);
		} else {
			printk(BIOS_DEBUG, "FRB2 timer action use default value: %d\n",
				FRB2_ACTION_DEFAULT);
			action = FRB2_ACTION_DEFAULT;
		}
		ipmi_init_and_start_bmc_wdt(CONFIG_BMC_KCS_BASE, (uint16_t)countdown,
			(uint8_t)action);
	} else {
		printk(BIOS_DEBUG, "Disable FRB2 timer\n");
		ipmi_stop_bmc_wdt(CONFIG_BMC_KCS_BASE);
	}
}
