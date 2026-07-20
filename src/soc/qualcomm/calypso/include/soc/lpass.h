/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_QUALCOMM_CALYPSO_LPASS_H_
#define _SOC_QUALCOMM_CALYPSO_LPASS_H_

#include <soc/addressmap.h>
#include <types.h>

#define LPASS_CORE_HM_READY			(1 << 0)
#define HW_CTL					(1 << 1)
#define GDSC_ENABLE_BIT				0
#define GDSC_RETAIN_FF_ENABLE			(1 << 11)

#define LPASS_CORE_HM_VOTE_POWER_ON		0x0
#define GDSC_PWR_ON				BIT(31)
#define LPASS_CORE_HM_VOTE_POWER_DOWN		0x1
#define BCM_LP0_VOTE_VALUE			0x60004001

#define GDSC_PWR_ON_DELAY 150000
#define LPASS_CORE_HM_READY_DELAY 1000000
enum cb_err lpass_bring_up(void);

#endif /* _SOC_QUALCOMM_CALYPSO_LPASS_H_ */
