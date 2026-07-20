/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_QUALCOMM_CALYPSO_RPMH_CONFIG_H_
#define _SOC_QUALCOMM_CALYPSO_RPMH_CONFIG_H_

#include <types.h>

/*
 * CALYPSO RPMh RSC Configuration
 */
#define RPMH_RSC_DRV_ID			2

#define RPMH_TCS_OFFSET			0xd00
#define RPMH_TCS_DISTANCE		0x2a0

/* TCS Configuration */
#define RPMH_NUM_ACTIVE_TCS		2
#define RPMH_NUM_SLEEP_TCS		3
#define RPMH_NUM_WAKE_TCS		3
#define RPMH_NUM_CONTROL_TCS		0
#define RPMH_NUM_FAST_PATH_TCS		0

#define RPMH_NUM_CHANNELS		1

#define RPMH_HW_SOLVER_SUPPORTED	true
#define RPMH_HW_CHANNEL_MODE		false

#define RPMH_RSC_NAME			"apps_rsc"

#define RPMH_REGULATOR_LEVEL_MIN_MM0	1
#define RPMH_REGULATOR_LEVEL_TURBO_MM0	6

#define RPMH_REGULATOR_LEVEL_NOM_L1	5
#define BCM_MM0_VOTE_VALUE		0x60004001

#define AOP_BOOT_COOKIE			0xA0C00C1E
#define AOP_BOOT_TIMEOUT_US		200000

int rpmh_rsc_init(void);

#endif /* _SOC_QUALCOMM_CALYPSO_RPMH_CONFIG_H_ */
