/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _FSP_UPD_H_
#define _FSP_UPD_H_

/*
 * Intel FSPs of XEON server platforms define FSPX_CONFIG
 * instead of FSP_X_CONFIG, which is expected by coreboot.
 */
#define FSP_T_CONFIG FSPT_CONFIG
#define FSP_M_CONFIG FSPM_CONFIG
#define FSP_S_CONFIG FSPS_CONFIG

#endif
