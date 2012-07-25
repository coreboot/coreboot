/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011  Alexandru Gagniuc <mr.nuke.me@gmail.com>
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
 * @file post_codes.h
 *
 * This file defines the prototypes for several common SMBUS functions
 * These functions are prefixed with __smbus_ so that they do not conflict with
 * the dozens of similar (duplicated) implementations in many southbridges.
 *
 * As a last parameter, the SMBUS functions take a u16 value __smbus_io_base,
 * which represents the base IO port for smbus transactions
 */

#include <arch/io.h>

/**
 * \brief SMBUS IO ports in relation to the base IO port
 */
#define SMBHSTSTAT		__smbus_io_base + 0x0
#define SMBSLVSTAT		__smbus_io_base + 0x1
#define SMBHSTCTL		__smbus_io_base + 0x2
#define SMBHSTCMD		__smbus_io_base + 0x3
#define SMBXMITADD		__smbus_io_base + 0x4
#define SMBHSTDAT0		__smbus_io_base + 0x5
#define SMBHSTDAT1		__smbus_io_base + 0x6
#define SMBBLKDAT		__smbus_io_base + 0x7
#define SMBSLVCTL		__smbus_io_base + 0x8
#define SMBTRNSADD		__smbus_io_base + 0x9
#define SMBSLVDATA 		__smbus_io_base + 0xa

#define SMBUS_TIMEOUT		(100*1000*10)

/**
 * \brief printk macro for SMBUS debugging
 */
#if defined(CONFIG_DEBUG_SMBUS_SETUP) && (CONFIG_DEBUG_SMBUS_SETUP)
#define printsmbus(x, ...) printk(BIOS_DEBUG, x, ##__VA_ARGS__)
#else
#define printsmbus(x, ...)
#endif

void __smbus_reset(u16 __smbus_io_base);
int __smbus_print_error(u8 host_status, int loops, u16 __smbus_io_base);
int __smbus_is_busy(u16 __smbus_io_base);
int __smbus_wait_until_ready(u16 __smbus_io_base);
u8 __smbus_read_byte(u8 dimm, u8 offset, u16 __smbus_io_base);

void __smbus_delay(void);

#if defined(SMBUS_IO_BASE) && (SMBUS_IO_BASE != 0)

__attribute__((always_inline, unused))
static void smbus_reset(void)
{
	__smbus_reset(SMBUS_IO_BASE);
}

__attribute__((always_inline, unused))
static int smbus_is_busy(void)
{
	return __smbus_is_busy(SMBUS_IO_BASE);
}

__attribute__((always_inline, unused))
static int smbus_wait_until_ready(void)
{
	return __smbus_wait_until_ready(SMBUS_IO_BASE);
}

__attribute__((always_inline, unused))
static int smbus_print_error(u8 host_status, int loops)
{
	return __smbus_print_error(host_status, loops, SMBUS_IO_BASE);
}

__attribute__((always_inline, unused))
static u8 smbus_read_byte(u8 dimm, u8 offset)
{
	return __smbus_read_byte(dimm, offset, SMBUS_IO_BASE);
}

#endif