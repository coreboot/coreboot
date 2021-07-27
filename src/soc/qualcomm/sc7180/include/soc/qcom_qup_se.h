/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_QCOM_QUP_SE_H__
#define __SOC_QCOM_QUP_SE_H__

#include <device/mmio.h>
#include <gpio.h>
#include <soc/addressmap.h>
#include <timer.h>
#include <types.h>

enum qup_se {
	QUPV3_0_SE0,
	QUPV3_0_SE1,
	QUPV3_0_SE2,
	QUPV3_0_SE3,
	QUPV3_0_SE4,
	QUPV3_0_SE5,
	QUPV3_1_SE0,
	QUPV3_1_SE1,
	QUPV3_1_SE2,
	QUPV3_1_SE3,
	QUPV3_1_SE4,
	QUPV3_1_SE5,
	QUPV3_SE_MAX,
};

struct qup {
	struct qup_regs *regs;
	gpio_t pin[6];
	u8 func[6];
};

extern struct qup qup[12];

#define MAX_OFFSET_CFG_REG	0x000001c0

#endif /* __SOC_QCOM_QUP_SE_H__ */
