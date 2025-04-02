/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_QCOM_QUP_SE_H__
#define __SOC_QCOM_QUP_SE_H__

#include <console/console.h>
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
	QUPV3_0_SE6,
	QUPV3_0_SE7,
	QUPV3_1_SE0,
	QUPV3_1_SE1,
	QUPV3_1_SE2,
	QUPV3_1_SE3,
	QUPV3_1_SE4,
	QUPV3_1_SE5,
	QUPV3_1_SE6,
	QUPV3_1_SE7,
	QUPV3_2_SE0,
	QUPV3_2_SE1,
	QUPV3_2_SE2,
	QUPV3_2_SE3,
	QUPV3_2_SE4,
	QUPV3_2_SE5,
	QUPV3_2_SE6,
	QUPV3_2_SE7,
	QUPV3_SE_MAX,
};

struct qup {
	struct qup_regs *regs;
	gpio_t pin[4];
	u8 func[4];
};

extern struct qup qup[QUPV3_SE_MAX];

/* TODO: update MAX_OFFSET_CFG_REG as per datasheet */
#define MAX_OFFSET_CFG_REG		0x000001c4

#endif /* __SOC_QCOM_QUP_SE_H__ */
