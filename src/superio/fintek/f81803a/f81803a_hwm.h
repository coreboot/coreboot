/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef SUPERIO_FINTEK_F81803_HWM_H
#define SUPERIO_FINTEK_F81803_HWM_H

#define TP_SENSOR_TYPE				0x6b
#define   TP_SENSOR1_TYPE_SHIFT			1
#define   TP_SENSOR2_TYPE_SHIFT			2
#define   TP_SENSOR_TYPE_MASK			0x01
#define TP_DIODE_STATUS				0x6f
#define   TP_MMX_OPEN				0x40
#define   TP_PECI_OPEN				0x20
#define   TP_TSI_OPEN				0x10
#define   TP_EXTERNAL_SENSOR2_OPEN		0x04
#define   TP_EXTERNAL_SENSOR1_OPEN		0x02

#define FAN_TYPE_REG				0x94
#define   FAN_TYPE_SHIFT(fan)			((fan - 1) * 2)
#define   FAN_TYPE_MASK				0x03
#define FAN_MODE_REG				0x96
	/* FUNC_PROG_SEL = 0 */
#define   FAN_MODE_SHIFT(fan)			((fan - 1) * 4)
#define   FAN_MODE_MASK				0x07
	/* FUNC_PROG_SEL = 1 */
#define   FAN1_ADJ_SEL_SHIFT			0
#define   FAN1_ADJ_SEL_MASK			0x07
#define   FAN_FREQ_SEL_ADD_SHIFT(fan)		(fan + 2)
#define FAN_UP_RATE_REG				0x9a
#define   FAN_RATE_SHIFT(fan)			((fan - 1) * 2)
#define   FAN_RATE_MASK				0x03
#define FAN_DOWN_RATE_REG			0x9b
#define   FAN_DOWN_RATE_DIFF_FROM_UP_SHIFT	7	/* FUNC_PROG_SEL = 1 */
#define   FAN_DIRECT_LOAD_EN_SHIFT		6	/* FUNC_PROG_SEL = 1 */
#define FAN_FAULT_TIME_REG			0x9f
#define   FAN_FUNC_PROG_SEL_SHIFT		7

#define FAN_BOUND_TEMP				0xa6	/* 4 temperatures */
#define FAN_SECTION_SPEED			0xaa	/* 5 sections */
#define FAN_TMP_MAPPING				0xaf
#define  FAN_TEMP_SEL_HIGH_SHIFT		7
#define  FAN_PWM_FREQ_SEL_SHIFT			6
#define  FAN_INTERPOLATION_SHIFT		4
#define  FAN_JUMP_UP_SHIFT			3
#define  FAN_JUMP_DOWN_SHIFT			2
#define  FAN_TEMP_SEL_LOW_SHIFT			0
#define  FAN_TEMP_SEL_LOW_MASK			0x03
#define  FAN_BIT_MASK				0x01

#define FAN_ADJUST(fan, start)			(((fan - 1) * 0x10) + start)

#define STATUS_INVALID_VALUE			-1
#define STATUS_INVALID_ORDER			-2

#define FIRST_FAN				1
#define LAST_FAN				2
#define MAX_DUTY				100

#endif /* SUPERIO_FINTEK_F81803_HWM_H */
