/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include "me.h"

/* HFS1[3:0] Current Working State Values */
static const char *me_cws_values[] = {
	[ME_HFS_CWS_RESET]	= "Reset",
	[ME_HFS_CWS_INIT]	= "Initializing",
	[ME_HFS_CWS_REC]	= "Recovery",
	[ME_HFS_CWS_NORMAL]	= "Normal",
	[ME_HFS_CWS_WAIT]	= "Platform Disable Wait",
	[ME_HFS_CWS_TRANS]	= "OP State Transition",
	[ME_HFS_CWS_INVALID]	= "Invalid CPU Plugged In",
};

/* HFS1[8:6] Current Operation State Values */
static const char *me_opstate_values[] = {
	[ME_HFS_STATE_PREBOOT]	= "Preboot",
	[ME_HFS_STATE_M0_UMA]	= "M0 with UMA",
	[ME_HFS_STATE_M3]	= "M3 without UMA",
	[ME_HFS_STATE_M0]	= "M0 without UMA",
	[ME_HFS_STATE_BRINGUP]	= "Bring up",
	[ME_HFS_STATE_ERROR]	= "M0 without UMA but with error"
};

/* HFS[19:16] Current Operation Mode Values */
static const char *me_opmode_values[] = {
	[ME_HFS_MODE_NORMAL]	= "Normal",
	[ME_HFS_MODE_DEBUG]	= "Debug",
	[ME_HFS_MODE_DIS]	= "Soft Temporary Disable",
	[ME_HFS_MODE_OVER_JMPR]	= "Security Override via Jumper",
	[ME_HFS_MODE_OVER_MEI]	= "Security Override via MEI Message"
};

/* HFS[15:12] Error Code Values */
static const char *me_error_values[] = {
	[ME_HFS_ERROR_NONE]	= "No Error",
	[ME_HFS_ERROR_UNCAT]	= "Uncategorized Failure",
	[ME_HFS_ERROR_IMAGE]	= "Image Failure",
	[ME_HFS_ERROR_DEBUG]	= "Debug Failure"
};

/* HFS2[31:28] ME Progress Code */
static const char *me_progress_values[] = {
	[ME_HFS2_PHASE_ROM]		= "ROM Phase",
	[ME_HFS2_PHASE_BUP]		= "BUP Phase",
	[ME_HFS2_PHASE_UKERNEL]		= "uKernel Phase",
	[ME_HFS2_PHASE_POLICY]		= "Policy Module",
	[ME_HFS2_PHASE_MODULE_LOAD]	= "Module Loading",
	[ME_HFS2_PHASE_UNKNOWN]		= "Unknown",
	[ME_HFS2_PHASE_HOST_COMM]	= "Host Communication"
};

/* HFS2[27:24] Power Management Event */
static const char *me_pmevent_values[] = {
	[ME_HFS2_PMEVENT_CLEAN_MOFF_MX_WAKE] = "Clean Moff->Mx wake",
	[ME_HFS2_PMEVENT_MOFF_MX_WAKE_ERROR] = "Moff->Mx wake after an error",
	[ME_HFS2_PMEVENT_CLEAN_GLOBAL_RESET] = "Clean global reset",
	[ME_HFS2_PMEVENT_CLEAN_GLOBAL_RESET_ERROR] = "Global reset after an error",
	[ME_HFS2_PMEVENT_CLEAN_ME_RESET] = "Clean Intel ME reset",
	[ME_HFS2_PMEVENT_ME_RESET_EXCEPTION] = "Intel ME reset due to exception",
	[ME_HFS2_PMEVENT_PSEUDO_ME_RESET] = "Pseudo-global reset",
	[ME_HFS2_PMEVENT_S0MO_SXM3] = "S0/M0->Sx/M3",
	[ME_HFS2_PMEVENT_SXM3_S0M0] = "Sx/M3->S0/M0",
	[ME_HFS2_PMEVENT_NON_PWR_CYCLE_RESET] = "Non-power cycle reset",
	[ME_HFS2_PMEVENT_PWR_CYCLE_RESET_M3] = "Power cycle reset through M3",
	[ME_HFS2_PMEVENT_PWR_CYCLE_RESET_MOFF] = "Power cycle reset through Moff",
	[ME_HFS2_PMEVENT_SXMX_SXMOFF] = "Sx/Mx->Sx/Moff"
};

/* Progress Code 0 states */
static const char *me_progress_rom_values[] = {
	[ME_HFS2_STATE_ROM_BEGIN] = "BEGIN",
	[ME_HFS2_STATE_ROM_DISABLE] = "DISABLE"
};

/* Progress Code 1 states */
static const char *me_progress_bup_values[] = {
	[ME_HFS2_STATE_BUP_INIT] = "Initialization starts",
	[ME_HFS2_STATE_BUP_DIS_HOST_WAKE] = "Disable the host wake event",
	[ME_HFS2_STATE_BUP_FLOW_DET] = "Flow determination start process",
	[ME_HFS2_STATE_BUP_VSCC_ERR] = "Error reading/matching the VSCC table in the descriptor",
	[ME_HFS2_STATE_BUP_CHECK_STRAP] = "Check to see if straps say ME DISABLED",
	[ME_HFS2_STATE_BUP_PWR_OK_TIMEOUT] = "Timeout waiting for PWROK",
	[ME_HFS2_STATE_BUP_MANUF_OVRD_STRAP] = "Possibly handle BUP manufacturing override strap",
	[ME_HFS2_STATE_BUP_M3] = "Bringup in M3",
	[ME_HFS2_STATE_BUP_M0] = "Bringup in M0",
	[ME_HFS2_STATE_BUP_FLOW_DET_ERR] = "Flow detection error",
	[ME_HFS2_STATE_BUP_M3_CLK_ERR] = "M3 clock switching error",
	[ME_HFS2_STATE_BUP_CPU_RESET_DID_TIMEOUT_MEM_MISSING] = "Host error - CPU reset timeout, DID timeout, memory missing",
	[ME_HFS2_STATE_BUP_M3_KERN_LOAD] = "M3 kernel load",
	[ME_HFS2_STATE_BUP_T32_MISSING] = "T34 missing - cannot program ICC",
	[ME_HFS2_STATE_BUP_WAIT_DID] = "Waiting for DID BIOS message",
	[ME_HFS2_STATE_BUP_WAIT_DID_FAIL] = "Waiting for DID BIOS message failure",
	[ME_HFS2_STATE_BUP_DID_NO_FAIL] = "DID reported no error",
	[ME_HFS2_STATE_BUP_ENABLE_UMA] = "Enabling UMA",
	[ME_HFS2_STATE_BUP_ENABLE_UMA_ERR] = "Enabling UMA error",
	[ME_HFS2_STATE_BUP_SEND_DID_ACK] = "Sending DID Ack to BIOS",
	[ME_HFS2_STATE_BUP_SEND_DID_ACK_ERR] = "Sending DID Ack to BIOS error",
	[ME_HFS2_STATE_BUP_M0_CLK] = "Switching clocks in M0",
	[ME_HFS2_STATE_BUP_M0_CLK_ERR] = "Switching clocks in M0 error",
	[ME_HFS2_STATE_BUP_TEMP_DIS] = "ME in temp disable",
	[ME_HFS2_STATE_BUP_M0_KERN_LOAD] = "M0 kernel load",
};

/* Progress Code 3 states */
static const char *me_progress_policy_values[] = {
	[ME_HFS2_STATE_POLICY_ENTRY] = "Entry into Policy Module",
	[ME_HFS2_STATE_POLICY_RCVD_S3] = "Received S3 entry",
	[ME_HFS2_STATE_POLICY_RCVD_S4] = "Received S4 entry",
	[ME_HFS2_STATE_POLICY_RCVD_S5] = "Received S5 entry",
	[ME_HFS2_STATE_POLICY_RCVD_UPD] = "Received UPD entry",
	[ME_HFS2_STATE_POLICY_RCVD_PCR] = "Received PCR entry",
	[ME_HFS2_STATE_POLICY_RCVD_NPCR] = "Received NPCR entry",
	[ME_HFS2_STATE_POLICY_RCVD_HOST_WAKE] = "Received host wake",
	[ME_HFS2_STATE_POLICY_RCVD_AC_DC] = "Received AC<>DC switch",
	[ME_HFS2_STATE_POLICY_RCVD_DID] = "Received DRAM Init Done",
	[ME_HFS2_STATE_POLICY_VSCC_NOT_FOUND] = "VSCC Data not found for flash device",
	[ME_HFS2_STATE_POLICY_VSCC_INVALID] = "VSCC Table is not valid",
	[ME_HFS2_STATE_POLICY_FPB_ERR] = "Flash Partition Boundary is outside address space",
	[ME_HFS2_STATE_POLICY_DESCRIPTOR_ERR] = "ME cannot access the chipset descriptor region",
	[ME_HFS2_STATE_POLICY_VSCC_NO_MATCH] = "Required VSCC values for flash parts do not match",
};

void intel_me_status(struct me_hfs *hfs, struct me_hfs2 *hfs2)
{
	if (CONFIG_DEFAULT_CONSOLE_LOGLEVEL < BIOS_DEBUG)
		return;

	/* Check Current States */
	printk(BIOS_DEBUG, "ME: FW Partition Table      : %s\n",
	       hfs->fpt_bad ? "BAD" : "OK");
	printk(BIOS_DEBUG, "ME: Bringup Loader Failure  : %s\n",
	       hfs->ft_bup_ld_flr ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: Firmware Init Complete  : %s\n",
	       hfs->fw_init_complete ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: Manufacturing Mode      : %s\n",
	       hfs->mfg_mode ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: Boot Options Present    : %s\n",
	       hfs->boot_options_present ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: Update In Progress      : %s\n",
	       hfs->update_in_progress ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: Current Working State   : %s\n",
	       me_cws_values[hfs->working_state]);
	printk(BIOS_DEBUG, "ME: Current Operation State : %s\n",
	       me_opstate_values[hfs->operation_state]);
	printk(BIOS_DEBUG, "ME: Current Operation Mode  : %s\n",
	       me_opmode_values[hfs->operation_mode]);
	printk(BIOS_DEBUG, "ME: Error Code              : %s\n",
	       me_error_values[hfs->error_code]);
	printk(BIOS_DEBUG, "ME: Progress Phase          : %s\n",
	       me_progress_values[hfs2->progress_code]);
	printk(BIOS_DEBUG, "ME: Power Management Event  : %s\n",
	       me_pmevent_values[hfs2->current_pmevent]);

	printk(BIOS_DEBUG, "ME: Progress Phase State    : ");
	switch (hfs2->progress_code) {
	case ME_HFS2_PHASE_ROM:		/* ROM Phase */
		printk(BIOS_DEBUG, "%s",
		       me_progress_rom_values[hfs2->current_state]);
		break;

	case ME_HFS2_PHASE_BUP:		/* Bringup Phase */
		if (hfs2->current_state < ARRAY_SIZE(me_progress_bup_values)
		    && me_progress_bup_values[hfs2->current_state])
			printk(BIOS_DEBUG, "%s",
			       me_progress_bup_values[hfs2->current_state]);
		else
			printk(BIOS_DEBUG, "0x%02x", hfs2->current_state);
		break;

	case ME_HFS2_PHASE_POLICY:	/* Policy Module Phase */
		if (hfs2->current_state < ARRAY_SIZE(me_progress_policy_values)
		    && me_progress_policy_values[hfs2->current_state])
			printk(BIOS_DEBUG, "%s",
			       me_progress_policy_values[hfs2->current_state]);
		else
			printk(BIOS_DEBUG, "0x%02x", hfs2->current_state);
		break;

	case ME_HFS2_PHASE_HOST_COMM:	/* Host Communication Phase */
		if (!hfs2->current_state)
			printk(BIOS_DEBUG, "Host communication established");
		else
			printk(BIOS_DEBUG, "0x%02x", hfs2->current_state);
		break;

	default:
		printk(BIOS_DEBUG, "Unknown phase: 0x%02x sate: 0x%02x",
		       hfs2->progress_code, hfs2->current_state);
	}
	printk(BIOS_DEBUG, "\n");
}
