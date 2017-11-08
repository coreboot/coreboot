/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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

#include <device/pci_def.h>
#include <device/device.h>

/* warning: Porting.h includes an open #pragma pack(1) */
#include <Porting.h>
#include <AGESA.h>
#include <soc/southbridge.h>
#include <soc/smbus.h>
#include <dimmSpd.h>

/*
 * readspd - Read one or more SPD bytes from a DIMM.
 *           Start with offset zero and read sequentially.
 *           Optimization relies on autoincrement to avoid
 *           sending offset for every byte.
 *          Reads 128 bytes in 7-8 ms at 400 KHz.
 */
static int readspd(uint16_t iobase, int SmbusSlaveAddress,
			char *buffer, size_t count)
{
	u8 dev_addr;
	size_t index;
	int error;
	char *pbuf = buffer;

	printk(BIOS_SPEW, "-------------READING SPD-----------\n");
	printk(BIOS_SPEW, "iobase: 0x%08X, SmbusSlave: 0x%08X, count: %d\n",
					iobase, SmbusSlaveAddress, (int)count);

	/*
	 * Convert received device address to the format accepted by
	 * do_smbus_read_byte and do_smbus_recv_byte.
	 */
	dev_addr = (SmbusSlaveAddress >> 1);

	/* Read the first SPD byte */
	error = do_smbus_read_byte(iobase, dev_addr, 0);
	if (error < 0) {
		printk(BIOS_ERR, "-------------SPD READ ERROR-----------\n");
		return error;
	} else {
		*pbuf = (char) error;
		pbuf++;
	}

	/* Read the remaining SPD bytes using do_smbus_recv_byte for speed */
	for (index = 1 ; index < count ; index++) {
		error = do_smbus_recv_byte(iobase, dev_addr);
		if (error < 0) {
			printk(BIOS_ERR, "-------------SPD READ ERROR-----------\n");
			return error;
		} else {
			*pbuf = (char) error;
			pbuf++;
		}
	}
	printk(BIOS_SPEW, "\n");
	printk(BIOS_SPEW, "-------------FINISHED READING SPD-----------\n");

	return 0;
}

static void writePmReg(int reg, int data)
{
	outb(reg, PM_INDEX);
	outb(data, PM_DATA);
}

static void setupFch(uint16_t ioBase)
{
	writePmReg(SMB_ASF_IO_BASE, ioBase >> 8);
	outb(SMB_SPEED_400KHZ, ioBase + SMBTIMING);
	/* Clear all SMBUS status bits */
	outb(SMBHST_STAT_CLEAR, ioBase + SMBHSTSTAT);
	outb(SMBSLV_STAT_CLEAR, ioBase + SMBSLVSTAT);
}

int sb_readSpd(int spdAddress, char *buf, size_t len)
{
	uint16_t ioBase = SMB_BASE_ADDR;
	setupFch(ioBase);
	return readspd(ioBase, spdAddress, buf, len);
}
