/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
 * Copyright (C) 2006,2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
 * Copyright (C) 2007 Silicon Integrated Systems Corp. (SiS)
 * Written by Morgan Tsai <my_tsai@sis.com> for SiS.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <device/smbus_def.h>

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

int smbus_wait_until_ready(unsigned smbus_io_base);
int smbus_wait_until_done(unsigned smbus_io_base);
int do_smbus_recv_byte(unsigned smbus_io_base, unsigned device);
int do_smbus_send_byte(unsigned smbus_io_base, unsigned device, unsigned char val);
int smbus_read_byte(unsigned device, unsigned address);
int smbus_write_byte(unsigned device, unsigned address, unsigned char val);

