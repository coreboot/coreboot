/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef OCP_VPD_H
#define OCP_VPD_H

/* VPD variable for enabling/disabling FRB2 timer. 1/0: Enable/disable */
#define FRB2_TIMER "frb2_timer_enable"
#define FRB2_TIMER_DEFAULT 1 /* Default value when the VPD variable is not found */

/* VPD variable for setting FRB2 timer countdown value (unit: 100ms). */
#define FRB2_COUNTDOWN "frb2_countdown"
/* Default countdown is 15 minutes when the VPD variable is not found */
#define FRB2_COUNTDOWN_DEFAULT 9000

/* VPD variable for setting FRB2 timer action.
   0: No action, 1: hard reset, 2: power down, 3: power cycle */
#define FRB2_ACTION "frb2_action"
#define FRB2_ACTION_DEFAULT 0 /* Default no action when the VPD variable is not found */

/* coreboot log level */
#define COREBOOT_LOG_LEVEL "coreboot_log_level"
#define COREBOOT_LOG_LEVEL_DEFAULT 4

/* Define the VPD keys for UPD variables that can be overwritten */
#define FSP_LOG "fsp_log_enable" /* 1 or 0: enable or disable FSP SOL log */
#define FSP_LOG_DEFAULT 1 /* Default value when the VPD variable is not found */

/* Select Memory Serial Debug Message Level.
   0:Disable, 1:Minimum, 2:Normal, 3:Maximum, 4:Auto */
#define FSP_MEM_LOG_LEVEL "fsp_mem_log_lvl"
#define FSP_MEM_LOG_LEVEL_DEFAULT 1

/* VPD variable for enabling/disabling MRC promote warning in FSP.
   System may upgrade memory training warning to fatal error when enabled.
   0: Disable, 1: Enable */
#define MRC_PROMOTE_WARNING "mrc_promote_warning"
#define MRC_PROMOTE_WARNING_DEFAULT 1

/* SMM log level */
#define SMM_LOG_LEVEL "smm_log_level"
#define SMM_LOG_LEVEL_DEFAULT 0 /* By default 0 would disable SMM log completely */

/* FSP Dfx PMIC secure mode.
   0:Disable Pmic Secure Mode, 1:Enable Pmic Secure Mode, 2:Auto Pmic Secure Mode */
#define FSP_PMIC_SECURE_MODE "fsp_pmic_mode"
#define FSP_PMIC_SECURE_MODE_DEFAULT 2

enum cxl_memory_mode {
	CXL_DISABLED		= 0,
	CXL_SYSTEM_MEMORY	= 1,
	CXL_SPM			= 2,
	CXL_MODE_MAX,
};

/* CXL mode. 0: Disable CXL, 1: configured as system memory, 2: configured as SPM. */
#define CXL_MODE "cxl_mode"
#define CXL_MODE_DEFAULT CXL_SYSTEM_MEMORY /* By default configured as system memory */

/* Skip TXT lockdown */
#define SKIP_INTEL_TXT_LOCKDOWN "skip_intel_txt_lockdown"
#define SKIP_INTEL_TXT_LOCKDOWN_DEFAULT 0

/* Disable boot drive PCIe root port for testing */
#define DISABLE_BOOTDRIVE "disable_bootdrive"
#define DISABLE_BOOTDRIVE_DEFAULT 0 /* By default don't disable */

/* Skip Global reset so that information in Previous Boot Error Hob won't be cleared */
#define SKIP_GLOBAL_RESET "skip_global_reset"
#define SKIP_GLOBAL_RESET_DEFAULT 1

/* Disable memory poison */
#define DISABLE_MEM_POISON "disable_mem_poison"
#define DISABLE_MEM_POISON_DEFAULT 0

/* Socket 0 core disable bitmask, set 1 to disable core */
#define CORE_DIS_BITMSK0 "core_disable_bitmask0"
/* Socket 1 core disable bitmask */
#define CORE_DIS_BITMSK1 "core_disable_bitmask1"

/* Get VPD key with provided fallback value and min/max ranges */
int get_int_from_vpd_range(const char *const key, const int fallback, const int min,
	const int max);
bool get_bool_from_vpd(const char *const key, const bool fallback);
int get_cxl_mode_from_vpd(void);
#endif
