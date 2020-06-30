/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef DELTALAKE_VPD_H
#define DELTALAKE_VPD_H

/* VPD variable for enabling/disabling FRB2 timer. */
#define FRB2_TIMER "frb2_timer"
/* VPD variable for setting FRB2 timer countdown value. */
#define FRB2_COUNTDOWN "frb2_countdown"
#define VPD_LEN 10
/* Default countdown is 15 minutes. */
#define DEFAULT_COUNTDOWN 9000

/* Define the VPD keys for UPD variables that can be overwritten */
#define FSP_LOG "fsp_log_enable" /* 1 or 0: enable or disable FSP SOL log */

#endif
