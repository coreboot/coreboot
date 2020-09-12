/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef DELTALAKE_VPD_H
#define DELTALAKE_VPD_H

/* VPD variable maximum length */
#define VPD_LEN 10

/* VPD variable for enabling/disabling FRB2 timer. 1/0: Enable/disable */
#define FRB2_TIMER "frb2_timer_enable"
#define FRB2_TIMER_DEFAULT 1 /* Default value when the VPD variable is not found */

/* VPD variable for setting FRB2 timer countdown value. */
#define FRB2_COUNTDOWN "frb2_countdown"
/* Default countdown is 15 minutes when the VPD variable is not found */
#define FRB2_COUNTDOWN_DEFAULT 9000

/* VPD variable for setting FRB2 timer action.
   0: No action, 1: hard reset, 2: power down, 3: power cycle */
#define FRB2_ACTION "frb2_action"
#define FRB2_ACTION_DEFAULT 0 /* Default no action when the VPD variable is not found */

/* Define the VPD keys for UPD variables that can be overwritten */
#define FSP_LOG "fsp_log_enable" /* 1 or 0: enable or disable FSP SOL log */
#define FSP_LOG_DEFAULT 1 /* Default value when the VPD variable is not found */

/* FSP debug print level: 1:Fatal, 2:Warning, 4:Summary, 8:Detail, 0x0F:All */
#define FSP_LOG_LEVEL "fsp_log_level"
#define FSP_LOG_LEVEL_DEFAULT 8 /* Default value when the VPD variable is not found */

/* DCI enable */
#define FSP_DCI "fsp_dci_enable" /* 1 or 0: enable or disable DCI */
#define FSP_DCI_DEFAULT 0 /* Default value when the VPD variable is not found */

/* coreboot log level */
#define COREBOOT_LOG_LEVEL "coreboot_log_level"
#define COREBOOT_LOG_LEVEL_DEFAULT 4

#endif
