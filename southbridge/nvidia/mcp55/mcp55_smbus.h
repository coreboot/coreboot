/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
 * Copyright (C) 2006,2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
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

#include <device/smbus_def.h>
#include <shared.h> /* We share symbols from stage 0 */


#define SMBHSTSTAT	0x1
#define SMBHSTPRTCL	0x0
#define SMBHSTCMD	0x3
#define SMBXMITADD	0x2
#define SMBHSTDAT0	0x4
#define SMBHSTDAT1	0x5

/* Between 1-10 seconds, We should never timeout normally
 * Longer than this is just painful when a timeout condition occurs.
 */
#define SMBUS_TIMEOUT	(100*1000*10)

static inline void smbus_delay(void)
{
	udelay(1);
}

int do_smbus_recv_byte(u16 smbus_io_base, u8 device);
int do_smbus_send_byte(u16 smbus_io_base, u8 device, u8 val);
int do_smbus_read_byte(u16 smbus_io_base, u8 device, u8 address);
int do_smbus_write_byte(u16 smbus_io_base, u8 device, u8 address, u8 val);
void enable_smbus(void);
