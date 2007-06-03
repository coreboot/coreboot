/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef SOUTHBRIDGE_INTEL_I82371EB_H
#define SOUTHBRIDGE_INTEL_I82371EB_H

#ifndef __ROMCC__
#include "chip.h"
void i82371eb_enable(device_t dev);
#endif

#define XBCS		0x4e	/* X-Bus Chip Select register */

/* SMBus */
#define SMBBA		0x90	/* SMBus Base Address */
#define SMBHSTCFG	0xd2	/* SMBus Host Configuration */

/* IDE */
#define IDETIM_PRI	0x40	/* IDE timing register, primary channel */
#define IDETIM_SEC	0x42	/* IDE timing register, secondary channel */

/* Bit definitions */
#define	IOSE			(1 << 0)	/* I/O Space Enable */
#define SMB_HST_EN		(1 << 0)	/* Host Interface Enable */
#define IDE_DECODE_ENABLE	(1 << 15)	/* IDE Decode Enable */

#endif				/* SOUTHBRIDGE_INTEL_I82371EB_H */
