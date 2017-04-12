/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 Yinghai Lu <yinghailu@gmail.com>
 * Copyright (C) 2009 coresystems GmbH
 * Copyright (C) 2013 Vladimir Serbinenko
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

#ifndef INTEL_COMMON_SMBUS_H
#define INTEL_COMMON_SMBUS_H

/* SMBus register offsets. */
#define SMBHSTSTAT		0x0
#define SMBHSTCTL		0x2
#define SMBHSTCMD		0x3
#define SMBXMITADD		0x4
#define SMBHSTDAT0		0x5
#define SMBHSTDAT1		0x6
#define SMBBLKDAT		0x7
#define SMBTRNSADD		0x9
#define SMBSLVDATA		0xa
#define SMLINK_PIN_CTL		0xe
#define SMBUS_PIN_CTL		0xf
#define SMBSLVCMD		0x11

int do_smbus_read_byte(unsigned int smbus_base, u8 device,
		unsigned int address);
int do_smbus_write_byte(unsigned int smbus_base, u8 device,
			unsigned int address, unsigned int data);
int do_smbus_block_read(unsigned int smbus_base, u8 device,
			u8 cmd, unsigned int bytes, u8 *buf);
int do_smbus_block_write(unsigned int smbus_base, u8 device,
			u8 cmd, unsigned int bytes, const u8 *buf);
/* Only since ICH5 */
int do_i2c_block_read(unsigned int smbus_base, u8 device,
		unsigned int offset, u32 bytes, u8 *buf);
#endif
