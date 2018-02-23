/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016-2017 Intel Corporation.
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

#include <arch/io.h>
#include <bootstate.h>
#include <commonlib/helpers.h>
#include <compiler.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/cse.h>
#include <soc/iomap.h>
#include <soc/me.h>
#include <soc/pci_devs.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static inline u32 me_read_config32(int offset)
{
	return pci_read_config32(PCH_DEV_CSE, offset);
}

/* HFSTS1[3:0] Current Working State Values */
static const char * const me_cws_values[] = {
	[ME_HFS_CWS_RESET]	= "Reset",
	[ME_HFS_CWS_INIT]	= "Initializing",
	[ME_HFS_CWS_REC]	= "Recovery",
	[3]			= "Unknown (3)",
	[4]			= "Unknown (4)",
	[ME_HFS_CWS_NORMAL]	= "Normal",
	[ME_HFS_CWS_WAIT]	= "Platform Disable Wait",
	[ME_HFS_CWS_TRANS]	= "OP State Transition",
	[ME_HFS_CWS_INVALID]	= "Invalid CPU Plugged In",
	[9]			= "Unknown (9)",
	[10]			= "Unknown (10)",
	[11]			= "Unknown (11)",
	[12]			= "Unknown (12)",
	[13]			= "Unknown (13)",
	[14]			= "Unknown (14)",
	[15]			= "Unknown (15)",
};

/* HFSTS1[8:6] Current Operation State Values */
static const char * const me_opstate_values[] = {
	[ME_HFS_STATE_PREBOOT]	= "Preboot",
	[ME_HFS_STATE_M0_UMA]	= "M0 with UMA",
	[ME_HFS_STATE_M3]	= "M3 without UMA",
	[ME_HFS_STATE_M0]	= "M0 without UMA",
	[ME_HFS_STATE_BRINGUP]	= "Bring up",
	[ME_HFS_STATE_ERROR]	= "M0 without UMA but with error"
};

/* HFSTS1[19:16] Current Operation Mode Values */
static const char * const me_opmode_values[] = {
	[ME_HFS_MODE_NORMAL]	= "Normal",
	[ME_HFS_MODE_DEBUG]	= "Debug",
	[ME_HFS_MODE_DIS]	= "Soft Temporary Disable",
	[ME_HFS_MODE_OVER_JMPR]	= "Security Override via Jumper",
	[ME_HFS_MODE_OVER_MEI]	= "Security Override via MEI Message"
};

/* HFSTS1[15:12] Error Code Values */
static const char * const me_error_values[] = {
	[ME_HFS_ERROR_NONE]	= "No Error",
	[ME_HFS_ERROR_UNCAT]	= "Uncategorized Failure",
	[ME_HFS_ERROR_IMAGE]	= "Image Failure",
	[ME_HFS_ERROR_DEBUG]	= "Debug Failure"
};

/* HFSTS2[31:28] ME Progress Code */
static const char * const me_progress_values[] = {
	[ME_HFS2_PHASE_ROM]		= "ROM Phase",
	[1]				= "Unknown (1)",
	[ME_HFS2_PHASE_UKERNEL]		= "uKernel Phase",
	[ME_HFS2_PHASE_BUP]		= "BUP Phase",
	[4]				= "Unknown (4)",
	[5]				= "Unknown (5)",
	[ME_HFS2_PHASE_HOST_COMM]	= "Host Communication",
	[7]				= "Unknown (7)",
	[8]				= "Unknown (8)"
};

/* HFSTS2[27:24] Power Management Event */
static const char * const me_pmevent_values[] = {
	[ME_HFS2_PMEVENT_CLEAN_MOFF_MX_WAKE] =
	"Clean Moff->Mx wake",
	[ME_HFS2_PMEVENT_MOFF_MX_WAKE_ERROR] =
	"Moff->Mx wake after an error",
	[ME_HFS2_PMEVENT_CLEAN_GLOBAL_RESET] =
	"Clean global reset",
	[ME_HFS2_PMEVENT_CLEAN_GLOBAL_RESET_ERROR] =
	"Global reset after an error",
	[ME_HFS2_PMEVENT_CLEAN_ME_RESET] =
	"Clean Intel ME reset",
	[ME_HFS2_PMEVENT_ME_RESET_EXCEPTION] =
	"Intel ME reset due to exception",
	[ME_HFS2_PMEVENT_PSEUDO_ME_RESET] =
	"Pseudo-global reset",
	[ME_HFS2_PMEVENT_CM0_CM3] =
	"CM0->CM3",
	[ME_HFS2_PMEVENT_CM3_CM0] =
	"CM3->CM0",
	[ME_HFS2_PMEVENT_NON_PWR_CYCLE_RESET] =
	"Non-power cycle reset",
	[ME_HFS2_PMEVENT_PWR_CYCLE_RESET_M3] =
	"Power cycle reset through M3",
	[ME_HFS2_PMEVENT_PWR_CYCLE_RESET_MOFF] =
	"Power cycle reset through Moff",
	[ME_HFS2_PMEVENT_CMX_CMOFF] =
	"Cx/Mx->Cx/Moff",
	[ME_HFS2_PMEVENT_CM0_CM0PG] =
	"CM0->CM0PG",
	[ME_HFS2_PMEVENT_CM3_CM3PG] =
	"CM3->CM3PG",
	[ME_HFS2_PMEVENT_CM0PG_CM0] =
	"CM0PG->CM0"

};

/* Progress Code 0 states */
static const char * const me_progress_rom_values[] = {
	[ME_HFS2_STATE_ROM_BEGIN]	= "BEGIN",
	[ME_HFS2_STATE_ROM_DISABLE]	= "DISABLE"
};

/* Progress Code 1 states */
static const char * const me_progress_bup_values[] = {
	[ME_HFS2_STATE_BUP_INIT] =
	"Initialization starts",
	[ME_HFS2_STATE_BUP_DIS_HOST_WAKE] =
	"Disable the host wake event",
	[ME_HFS2_STATE_BUP_CG_ENABLE] =
	"Enabling CG for cset",
	[ME_HFS2_STATE_BUP_PM_HND_EN] =
	"Enabling PM handshaking",
	[ME_HFS2_STATE_BUP_FLOW_DET] =
	"Flow determination start process",
	[ME_HFS2_STATE_BUP_PMC_PATCHING] =
	"PMC Patching process",
	[ME_HFS2_STATE_BUP_GET_FLASH_VSCC] =
	"Get VSCC params",
	[ME_HFS2_STATE_BUP_SET_FLASH_VSCC] =
	"Set VSCC params",
	[ME_HFS2_STATE_BUP_VSCC_ERR] =
	"Error reading/matching the VSCC table in the descriptor",
	[ME_HFS2_STATE_BUP_EFSS_INIT] =
	"Initialize EFFS",
	[ME_HFS2_STATE_BUP_CHECK_STRAP] =
	"Check to see if straps say ME DISABLED",
	[ME_HFS2_STATE_BUP_PWR_OK_TIMEOUT] =
	"Timeout waiting for PWROK",
	[ME_HFS2_STATE_BUP_STRAP_DIS] =
	"EFFS says ME disabled",
	[ME_HFS2_STATE_BUP_MANUF_OVRD_STRAP] =
	"Possibly handle BUP manufacturing override strap",
	[ME_HFS2_STATE_BUP_M3] =
	"Bringup in M3",
	[ME_HFS2_STATE_BUP_M0] =
	"Bringup in M0",
	[ME_HFS2_STATE_BUP_FLOW_DET_ERR] =
	"Flow detection error",
	[ME_HFS2_STATE_BUP_M3_CLK_ERR] =
	"M3 clock switching error",
	[ME_HFS2_STATE_BUP_CPU_RESET_DID_TIMEOUT_MEM_MISSING] =
	"Host error - CPU reset timeout, DID timeout, memory missing",
	[ME_HFS2_STATE_BUP_M3_KERN_LOAD] =
	"M3 kernel load",
	[ME_HFS2_STATE_BUP_T32_MISSING] =
	"T34 missing - cannot program ICC",
	[ME_HFS2_STATE_BUP_WAIT_DID] =
	"Waiting for DID BIOS message",
	[ME_HFS2_STATE_BUP_WAIT_DID_FAIL] =
	"Waiting for DID BIOS message failure",
	[ME_HFS2_STATE_BUP_DID_NO_FAIL] =
	"DID reported no error",
	[ME_HFS2_STATE_BUP_ENABLE_UMA] =
	"Enabling UMA",
	[ME_HFS2_STATE_BUP_ENABLE_UMA_ERR] =
	"Enabling UMA error",
	[ME_HFS2_STATE_BUP_SEND_DID_ACK] =
	"Sending DID Ack to BIOS",
	[ME_HFS2_STATE_BUP_SEND_DID_ACK_ERR] =
	"Sending DID Ack to BIOS error",
	[ME_HFS2_STATE_BUP_M0_CLK] =
	"Switching clocks in M0",
	[ME_HFS2_STATE_BUP_M0_CLK_ERR] =
	"Switching clocks in M0 error",
	[ME_HFS2_STATE_BUP_TEMP_DIS] =
	"ME in temp disable",
	[ME_HFS2_STATE_BUP_M0_KERN_LOAD] =
	"M0 kernel load",
};

static void print_me_version(void *unused)
{
	struct mkhi_hdr {
		uint8_t group_id;
		uint8_t command:7;
		uint8_t is_resp:1;
		uint8_t rsvd;
		uint8_t result;
	} __packed;

	struct version {
		uint16_t minor;
		uint16_t major;
		uint16_t build;
		uint16_t hotfix;
	} __packed;

	struct fw_ver_resp {
		struct mkhi_hdr hdr;
		struct version code;
		struct version rec;
		struct version fitc;
	} __packed;

	const struct mkhi_hdr fw_ver_msg = {
		.group_id = MKHI_GEN_GROUP_ID,
		.command = MKHI_GET_FW_VERSION,
	};

	struct fw_ver_resp resp;
	size_t resp_size = sizeof(resp);
	union me_hfs hfs;

	/*
	 * Print ME version only if UART debugging is enabled. Else, it takes ~1
	 * second to talk to ME and get this information.
	 */
	if (!IS_ENABLED(CONFIG_UART_DEBUG))
		return;

	hfs.data = me_read_config32(PCI_ME_HFSTS1);
	/*
	 * This command can be run only if:
	 * - Working state is normal and
	 * - Operation mode is normal.
	 */
	if ((hfs.fields.working_state != ME_HFS_CWS_NORMAL) ||
	    (hfs.fields.operation_mode != ME_HFS_MODE_NORMAL))
		goto failed;

	if (!heci_send(&fw_ver_msg, sizeof(fw_ver_msg), BIOS_HOST_ADD,
		       HECI_MKHI_ADD))
		goto failed;

	if (!heci_receive(&resp, &resp_size))
		goto failed;

	if (resp.hdr.result)
		goto failed;

	printk(BIOS_DEBUG, "ME: Version : %d.%d.%d.%d\n", resp.code.major,
	       resp.code.minor, resp.code.hotfix, resp.code.build);
	return;

failed:
	printk(BIOS_DEBUG, "ME: Version : Unavailable\n");
}
/*
 * This can't be put in intel_me_status because by the time control
 * reaches there, ME doesn't respond to GET_FW_VERSION command.
 */
BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_EXIT, print_me_version, NULL);

void intel_me_status(void)
{
	union me_hfs hfs;
	union me_hfs2 hfs2;
	union me_hfs3 hfs3;
	union me_hfs6 hfs6;

	hfs.data = me_read_config32(PCI_ME_HFSTS1);
	hfs2.data = me_read_config32(PCI_ME_HFSTS2);
	hfs3.data = me_read_config32(PCI_ME_HFSTS3);
	hfs6.data = me_read_config32(PCI_ME_HFSTS6);

	printk(BIOS_DEBUG, "ME: Host Firmware Status Register 1 : 0x%08X\n",
		hfs.data);
	printk(BIOS_DEBUG, "ME: Host Firmware Status Register 2 : 0x%08X\n",
		hfs2.data);
	printk(BIOS_DEBUG, "ME: Host Firmware Status Register 3 : 0x%08X\n",
		hfs3.data);
	printk(BIOS_DEBUG, "ME: Host Firmware Status Register 4 : 0x%08X\n",
		me_read_config32(PCI_ME_HFSTS4));
	printk(BIOS_DEBUG, "ME: Host Firmware Status Register 5 : 0x%08X\n",
		me_read_config32(PCI_ME_HFSTS5));
	printk(BIOS_DEBUG, "ME: Host Firmware Status Register 6 : 0x%08X\n",
		hfs6.data);
	/* Check Current States */
	printk(BIOS_DEBUG, "ME: FW Partition Table      : %s\n",
	       hfs.fields.fpt_bad ? "BAD" : "OK");
	printk(BIOS_DEBUG, "ME: Bringup Loader Failure  : %s\n",
	       hfs.fields.ft_bup_ld_flr ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: Firmware Init Complete  : %s\n",
	       hfs.fields.fw_init_complete ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: Manufacturing Mode      : %s\n",
	       hfs.fields.mfg_mode ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: Boot Options Present    : %s\n",
	       hfs.fields.boot_options_present ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: Update In Progress      : %s\n",
	       hfs.fields.update_in_progress ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: D3 Support              : %s\n",
	       hfs.fields.d3_support_valid ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: D0i3 Support            : %s\n",
	       hfs.fields.d0i3_support_valid ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: Low Power State Enabled : %s\n",
	       hfs2.fields.low_power_state ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: CPU Replaced            : %s\n",
	       hfs2.fields.cpu_replaced_sts  ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: CPU Replacement Valid   : %s\n",
	       hfs2.fields.cpu_replaced_valid ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: Current Working State   : %s\n",
	       me_cws_values[hfs.fields.working_state]);
	printk(BIOS_DEBUG, "ME: Current Operation State : %s\n",
	       me_opstate_values[hfs.fields.operation_state]);
	printk(BIOS_DEBUG, "ME: Current Operation Mode  : %s\n",
	       me_opmode_values[hfs.fields.operation_mode]);
	printk(BIOS_DEBUG, "ME: Error Code              : %s\n",
	       me_error_values[hfs.fields.error_code]);
	printk(BIOS_DEBUG, "ME: Progress Phase          : %s\n",
	       me_progress_values[hfs2.fields.progress_code]);
	printk(BIOS_DEBUG, "ME: Power Management Event  : %s\n",
	       me_pmevent_values[hfs2.fields.current_pmevent]);

	printk(BIOS_DEBUG, "ME: Progress Phase State    : ");
	switch (hfs2.fields.progress_code) {
	case ME_HFS2_PHASE_ROM:		/* ROM Phase */
		printk(BIOS_DEBUG, "%s",
		       me_progress_rom_values[hfs2.fields.current_state]);
		break;

	case ME_HFS2_PHASE_UKERNEL:	/* uKernel Phase */
		printk(BIOS_DEBUG, "0x%02x", hfs2.fields.current_state);
		break;

	case ME_HFS2_PHASE_BUP:		/* Bringup Phase */
		if (hfs2.fields.current_state
			< ARRAY_SIZE(me_progress_bup_values)
		    && me_progress_bup_values[hfs2.fields.current_state])
			printk(BIOS_DEBUG, "%s",
				me_progress_bup_values[
					hfs2.fields.current_state]);
		else
			printk(BIOS_DEBUG, "0x%02x", hfs2.fields.current_state);
		break;

	case ME_HFS2_PHASE_HOST_COMM:	/* Host Communication Phase */
		if (!hfs2.fields.current_state)
			printk(BIOS_DEBUG, "Host communication established");
		else
			printk(BIOS_DEBUG, "0x%02x", hfs2.fields.current_state);
		break;

	default:
		printk(BIOS_DEBUG, "Unknown phase: 0x%02x state: 0x%02x",
		       hfs2.fields.progress_code, hfs2.fields.current_state);
	}
	printk(BIOS_DEBUG, "\n");

	/* Power Down Mitigation Status */
	printk(BIOS_DEBUG, "ME: Power Down Mitigation   : %s\n",
	       hfs3.fields.power_down_mitigation ? "YES" : "NO");

	if (hfs3.fields.power_down_mitigation) {
		printk(BIOS_INFO, "ME: PD Mitigation State     : ");
		if (hfs3.fields.encrypt_key_override == 1 &&
		    hfs3.fields.encrypt_key_check == 0 &&
		    hfs3.fields.pch_config_change == 0)
			printk(BIOS_INFO, "Normal Operation");
		else if (hfs3.fields.encrypt_key_override == 1 &&
			 hfs3.fields.encrypt_key_check == 1 &&
			 hfs3.fields.pch_config_change == 0)
			printk(BIOS_INFO, "Issue Detected and Recovered");
		else
			printk(BIOS_INFO, "Issue Detected but not Recovered");
		printk(BIOS_INFO, "\n");

		printk(BIOS_DEBUG, "ME: Encryption Key Override : %s\n",
		       hfs3.fields.encrypt_key_override ? "Workaround Applied" :
		       "Unable to override");
		printk(BIOS_DEBUG, "ME: Encryption Key Check    : %s\n",
		       hfs3.fields.encrypt_key_check ? "FAIL" : "PASS");
		printk(BIOS_DEBUG, "ME: PCH Configuration Info  : %s\n",
		       hfs3.fields.pch_config_change ? "Changed" : "No Change");

		printk(BIOS_DEBUG, "ME: Firmware SKU            : ");
		switch (hfs3.fields.fw_sku) {
		case ME_HFS3_FW_SKU_CONSUMER:
			printk(BIOS_DEBUG, "Consumer\n");
			break;
		case ME_HFS3_FW_SKU_CORPORATE:
			printk(BIOS_DEBUG, "Corporate\n");
			break;
		default:
			printk(BIOS_DEBUG, "Unknown (0x%x)\n",
				hfs3.fields.fw_sku);
		}
	}

	printk(BIOS_DEBUG, "ME: FPF status               : ");
	switch (hfs6.fields.fpf_nvars) {
	case ME_HFS6_FPF_NOT_COMMITTED:
		printk(BIOS_DEBUG, "unfused\n");
		break;
	case ME_HFS6_FPF_ERROR:
		printk(BIOS_DEBUG, "unknown\n");
		break;
	default:
		printk(BIOS_DEBUG, "fused\n");
	}
}

static int send_heci_reset_message(void)
{
	int status;
	struct reset_reply {
		u8 group_id;
		u8 command;
		u8 reserved;
		u8 result;
	} __packed reply;
	struct reset_message {
		u8 group_id;
		u8 cmd;
		u8 reserved;
		u8 result;
		u8 req_origin;
		u8 reset_type;
	} __packed;
	struct reset_message msg = {
		.cmd = MKHI_GLOBAL_RESET,
		.req_origin = GR_ORIGIN_BIOS_POST,
		.reset_type = GLOBAL_RST_TYPE
	};
	size_t reply_size;

	heci_reset();

	status = heci_send(&msg, sizeof(msg), BIOS_HOST_ADD, HECI_MKHI_ADD);
	if (!status)
		return -1;

	reply_size = sizeof(reply);
	memset(&reply, 0, reply_size);
	status = heci_receive(&reply, &reply_size);
	if (!status)
		return -1;
	/* get reply result from HECI MSG  */
	if (reply.result) {
		printk(BIOS_DEBUG, "%s: Exit with Failure\n", __func__);
		return -1;
	}
	printk(BIOS_DEBUG, "%s: Exit with Success\n",  __func__);
	return 0;
}

int send_global_reset(void)
{
	int status = -1;
	union me_hfs hfs;

	/* Check ME operating mode */
	hfs.data = me_read_config32(PCI_ME_HFSTS1);
	if (hfs.fields.operation_mode)
		goto ret;

	/* ME should be in Normal Mode for this command */
	status = send_heci_reset_message();
ret:
	return status;
}
