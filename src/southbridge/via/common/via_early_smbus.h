/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file via_early_sambus.h
 *
 * This file contains generic definitions used in VIA SMBus controllers.
 *
 * Functions defined in device/early/smbus.h are each implemented in a separate
 * early_smbus_[func_name].c file. This makes it possible to override any of
 * these functions by not including them in yout build, via Makefile.c. This is
 * useful when there is a need to work around chipset bugs.
 *
 * These implementations work with most via chipsets. Any VIA port should try
 * to use these. Makefile.inc needs to be adapted to link against the files
 * providing SMBus functionality:
 * @code
 *	romstage-y += ./../../../southbridge/via/common/early_smbus_func.c
 * @endcode
 */

/**
 * \brief SMBus IO ports in relation to the base IO port
 */
#define SMBHSTSTAT(base)		((u16)base + 0x0)
#define SMBSLVSTAT(base)		((u16)base + 0x1)
#define SMBHSTCTL(base)			((u16)base + 0x2)
#define SMBHSTCMD(base)			((u16)base + 0x3)
#define SMBXMITADD(base)		((u16)base + 0x4)
#define SMBHSTDAT0(base)		((u16)base + 0x5)
#define SMBHSTDAT1(base)		((u16)base + 0x6)
#define SMBBLKDAT(base)			((u16)base + 0x7)
#define SMBSLVCTL(base)			((u16)base + 0x8)
#define SMBTRNSADD(base)		((u16)base + 0x9)
#define SMBSLVDATA (base)		((u16)base + 0xa)

#define SMBUS_TIMEOUT			(100*1000*10)

#include <device/early_smbus.h>
#include <arch/io.h>
