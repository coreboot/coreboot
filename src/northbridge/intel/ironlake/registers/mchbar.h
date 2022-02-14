/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __IRONLAKE_REGISTERS_MCHBAR_H__
#define __IRONLAKE_REGISTERS_MCHBAR_H__

#define CxJEDEC(x)	(0x271 + 0x400 * (x))

/* Bits 3:1 of CxJEDEC, aka SMS (SDRAM Mode Select) */
enum {
	SMS_POST_RESET       = 0 << 1,
	SMS_NOP_COMMAND      = 1 << 1,
	SMS_ALL_BANKS_PCHG   = 2 << 1,
	SMS_MODE_REG_SET     = 3 << 1,
	SMS_EXT_MODE_REG_SET = 4 << 1,
	SMS_ZQ_CALIBRATION   = 5 << 1, /* Reserved in datasheet, but matches x4x and gm45 */
	SMS_CBR_REFRESH      = 6 << 1,
	SMS_NORMAL_MODE      = 7 << 1,
};

#define  CxJEDEC_MASK    (0x1f << 1)
#define  CxJEDEC_EMRS(x) ((x) << 4 | SMS_EXT_MODE_REG_SET)

#endif /* __IRONLAKE_REGISTERS_MCHBAR_H__ */
