/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on Intel Alder Lake Processor PCH Datasheet
 * Document number: 621483
 * Chapter number: 6
 */

#ifndef _SOC_ALDERLAKE_SMBUS_H_
#define _SOC_ALDERLAKE_SMBUS_H_

/* IO and MMIO registers under primary BAR */

/* TCO registers and fields live behind TCOBASE I/O bar in SMBus device. */
#define TCO1_STS			0x04
#define  TCO_TIMEOUT			(1 << 3)
#define TCO2_STS			0x06
#define  TCO_STS_SECOND_TO		(1 << 1)
#define  TCO_INTRD_DET			(1 << 0)
#define TCO1_CNT			0x08
#define  TCO_LOCK			(1 << 12)
#define  TCO_TMR_HLT			(1 << 11)
#define TCO2_CNT			0x0A
#define  TCO_INTRD_SEL_MASK		(3 << 1)
#define  TCO_INTRD_SEL_SMI		(1 << 2)
#define  TCO_INTRD_SEL_INT		(1 << 1)

/*
 * Default slave address value for PCH. This value is set to match default
 * value set by hardware. It is useful since PCH is able to respond even
 * before CPU is up. This is reset by RSMRST# but not by PLTRST#.
 */
#define SMBUS_SLAVE_ADDR		0x44

#endif
