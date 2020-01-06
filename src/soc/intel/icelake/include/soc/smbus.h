/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_ICELAKE_SMBUS_H_
#define _SOC_ICELAKE_SMBUS_H_

/* TCO registers and fields live behind TCOBASE I/O bar in SMBus device. */
#define TCO1_STS			0x04
#define  TCO_TIMEOUT			(1 << 3)
#define TCO2_STS			0x06
#define  TCO_STS_SECOND_TO		(1 << 1)
#define TCO1_CNT			0x08
#define  TCO_LOCK			(1 << 12)
#define  TCO_TMR_HLT			(1 << 11)

/*
 * Default slave address value for PCH. This value is set to match default
 * value set by hardware. It is useful since PCH is able to respond even
 * before CPU is up. This is reset by RSMRST# but not by PLTRST#.
 */
#define SMBUS_SLAVE_ADDR		0x44

#endif
