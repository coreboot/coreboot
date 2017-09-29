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
 * readSmbusByteData - read a single SPD byte from any offset
 */
static int readSmbusByteData(int iobase, int address, char *buffer, int offset)
{
	unsigned int status;
	UINT64 limit;

	address |= 1; // set read bit

	__outbyte(iobase + SMBHSTSTAT, SMBHST_STAT_CLEAR);
	__outbyte(iobase + SMBSLVSTAT, SMBSLV_STAT_CLEAR);
	__outbyte(iobase + SMBHSTCMD, offset);		// offset in eeprom
	__outbyte(iobase + SMBHSTADDR, address);	// slave addr & read bit
	__outbyte(iobase + SMBHSTCTRL, SMBHST_CTRL_STRT + SMBHST_CTRL_BDT_RW);

	// time limit to avoid hanging for unexpected error status
	limit = __rdtsc() + 2000000000 / 10;
	for (;;) {
		status = __inbyte(iobase + SMBHSTSTAT);
		if (__rdtsc() > limit)
			break;
		if ((status & SMBHST_STAT_INTERRUPT) == 0)
			continue;	// SMBusInterrupt not set, keep waiting
		if ((status & SMBHST_STAT_BUSY) == SMBHST_STAT_BUSY)
			continue;	// HostBusy set, keep waiting
		break;
	}

	buffer[0] = __inbyte(iobase + SMBHSTDAT0);
	if (status == SMBHST_STAT_NOERROR)
		status = 0;		// done with no errors
	return status;
}

/*
 * readSmbusByte - read a single SPD byte from the default offset
 *                 this function is faster function readSmbusByteData
 */
static int readSmbusByte(int iobase, int address, char *buffer)
{
	unsigned int status;
	UINT64 limit;

	__outbyte(iobase + SMBHSTSTAT, SMBHST_STAT_CLEAR);
	__outbyte(iobase + SMBHSTCTRL, SMBHST_CTRL_STRT + SMBHST_CTRL_BTE_RW);

	// time limit to avoid hanging for unexpected error status
	limit = __rdtsc() + 2000000000 / 10;
	for (;;) {
		status = __inbyte(iobase + SMBHSTSTAT);
		if (__rdtsc() > limit)
			break;
		if ((status & SMBHST_STAT_INTERRUPT) == 0)
			continue;	// SMBusInterrupt not set, keep waiting
		if ((status & SMBHST_STAT_BUSY) == SMBHST_STAT_BUSY)
			continue;	// HostBusy set, keep waiting
		break;
	}

	buffer[0] = __inbyte(iobase + SMBHSTDAT0);
	if (status == SMBHST_STAT_NOERROR)
		status = 0;		// done with no errors
	return status;
}

/*
 * readspd - Read one or more SPD bytes from a DIMM.
 *           Start with offset zero and read sequentially.
 *           Optimization relies on autoincrement to avoid
 *           sending offset for every byte.
 *          Reads 128 bytes in 7-8 ms at 400 KHz.
 */
static int readspd(int iobase, int SmbusSlaveAddress, char *buffer, int count)
{
	int index, error;

	printk(BIOS_SPEW, "-------------READING SPD-----------\n");
	printk(BIOS_SPEW, "iobase: 0x%08X, SmbusSlave: 0x%08X, count: %d\n",
					iobase, SmbusSlaveAddress, count);

	/* read the first byte using offset zero */
	error = readSmbusByteData(iobase, SmbusSlaveAddress, buffer, 0);

	if (error) {
		printk(BIOS_ERR, "-------------SPD READ ERROR-----------\n");
		return error;
	}

	/* read the remaining bytes using auto-increment for speed */
	for (index = 1; index < count; index++) {
		error = readSmbusByte(iobase, SmbusSlaveAddress,
					&buffer[index]);
		if (error) {
			printk(BIOS_ERR, "-------------SPD READ ERROR-----------\n");
			return error;
		}
	}
	printk(BIOS_SPEW, "\n");
	printk(BIOS_SPEW, "-------------FINISHED READING SPD-----------\n");

	return 0;
}

static void writePmReg(int reg, int data)
{
	__outbyte(PM_INDEX, reg);
	__outbyte(PM_DATA, data);
}

static void setupFch(int ioBase)
{
	/* register 0x2c and 0x2d are not defined in public datasheet */
	writePmReg(0x2d, ioBase >> 8);
	writePmReg(0x2c, ioBase | 1);
	/* set SMBus clock to 400 KHz */
	__outbyte(ioBase + SMBTIMING, 66000000 / 400000 / 4);
}

int sb_readSpd(int spdAddress, char *buf, size_t len)
{
	int ioBase = SMB_BASE_ADDR;
	setupFch(ioBase);
	return readspd(ioBase, spdAddress, buf, len);
}
