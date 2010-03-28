/*
 * This file is part of the coreboot project.
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

#ifndef SOUTHBRIDGE_INTEL_I82371EB_I82371EB_H
#define SOUTHBRIDGE_INTEL_I82371EB_I82371EB_H

#if !defined(__PRE_RAM__)
#include "chip.h"
void i82371eb_enable(device_t dev);
void i82371eb_hard_reset(void);
#endif

/* If 'cond' is true this macro sets the bit(s) specified by 'bits' in the
 * 'reg' variable, otherwise it clears those bits.
 *
 * Examples:
 * reg16 = ONOFF(conf->ide0_enable, reg16, (1 << 5));
 * reg16 = ONOFF(conf->ide0_enable, reg16, (FOO | BAR));
 */
/* TODO: Move into some global header file? */
#define ONOFF(cond,reg,bits) ((cond) ? ((reg) | (bits)) : ((reg) & ~(bits)))

#define XBCS		0x4e	/* X-Bus chip select register */
#define GENCFG		0xb0	/* General configuration register */
#define RC		0xcf9	/* Reset control register */

/* IDE */
#define IDETIM_PRI	0x40	/* IDE timing register, primary channel */
#define IDETIM_SEC	0x42	/* IDE timing register, secondary channel */
#define UDMACTL		0x48	/* Ultra DMA/33 control register */
#define UDMATIM		0x4a	/* Ultra DMA/33 timing register */

/* SMBus */
#define SMBBA		0x90	/* SMBus base address */
#define SMBHSTCFG	0xd2	/* SMBus host configuration */

/* Power management (ACPI) */
#define PMBA		0x40	/* Power management base address */
#define PMREGMISC	0x80	/* Miscellaneous power management */

/* Bit definitions */
#define EXT_BIOS_ENABLE_1MB	(1 << 9)  /* 1-Meg Extended BIOS Enable */
#define EXT_BIOS_ENABLE		(1 << 7)  /* Extended BIOS Enable */
#define LOWER_BIOS_ENABLE	(1 << 6)  /* Lower BIOS Enable */
#define WRITE_PROTECT_ENABLE	(1 << 2)  /* Write Protect Enable */
#define SRST			(1 << 1)  /* System Reset */
#define RCPU			(1 << 2)  /* Reset CPU */
#define SMB_HST_EN		(1 << 0)  /* Host Interface Enable */
#define IDE_DECODE_ENABLE	(1 << 15) /* IDE Decode Enable */
#define DTE0			(1 << 3)  /* DMA Timing Enable Only, drive 0 */
#define DTE1			(1 << 7)  /* DMA Timing Enable Only, drive 1 */
#define PSDE0			(1 << 0)  /* Primary Drive 0 UDMA/33 */
#define PSDE1			(1 << 1)  /* Primary Drive 1 UDMA/33 */
#define SSDE0			(1 << 2)  /* Secondary Drive 0 UDMA/33 */
#define SSDE1			(1 << 3)  /* Secondary Drive 1 UDMA/33 */
#define ISA			(1 << 0)  /* Select ISA */
#define EIO			(0 << 0)  /* Select EIO */

#endif /* SOUTHBRIDGE_INTEL_I82371EB_I82371EB_H */
