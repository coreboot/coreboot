/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef TIOGAPASS_VPD_H
#define TIOGAPASS_VPD_H

/* VPD variable for enabling/disabling FRB2 timer. */
#define FRB2_TIMER "frb2_timer"
/* VPD variable for setting FRB2 timer countdown value. */
#define FRB2_COUNTDOWN "frb2_countdown"
#define VPD_LEN 10
/* Default countdown is 15 minutes. */
#define DEFAULT_COUNTDOWN 9000

#endif /* TIOGAPASS_VPD_H */
