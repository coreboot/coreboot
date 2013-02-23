/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 - 2012 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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

#include "Porting.h"
#include "AGESA.h"
#include "amdlib.h"
#include <arch/io.h>
#include <arch/romcc_io.h>
#include <device/pci_ids.h>

AGESA_STATUS AmdMemoryReadSPD (UINT32 unused1, UINT32 unused2, AGESA_READ_SPD_PARAMS *info);
#define DIMENSION(array)(sizeof (array)/ sizeof (array [0]))

/* SP5100 GPIO 49-52 contoled by SMBUS PCI_Reg 0x52 */
#define SP5100_GPIO37_46_47_48	0xA6
#define SP5100_GPIO49_52	0x50

/**
 * TODO not support all GPIO yet
 * @param reg -GPIO Cntrl Register
 * @param out -GPIO bitmap
 * @param out -GPIO enable bitmap
 * @return  old setting
 */
static u8 sp5100_set_gpio(u8 reg, u8 out, u8 enable)
{
	u8 value, ret;
	device_t sm_dev = PCI_DEV(0, 0x14, 0); //SMBUS

	value = pci_read_config8(sm_dev, reg);
	ret = value;
	value &= ~(enable);
	value |= out;
	value &= ~(enable << 4);
	pci_write_config8(sm_dev, reg, value);

	return ret;
}

static void sp5100_restore_gpio(u8 reg, u8 value)
{
	device_t sm_dev = PCI_DEV(0, 0x14, 0);
	pci_write_config8(sm_dev, reg, value);
}

/*-----------------------------------------------------------------------------
 *
 * SPD address table - porting required
 */
static const UINT8 spdAddressLookup [2] [2] [3] = { // socket, channel, dimm
	/* socket 0 */
	{
		{0xA0, 0xA4, 0xA8},
		{0xA2, 0xA6, 0xAA},
	},
	/* socket 1 */
	{
		{0xA0, 0xA4, 0xA8},
		{0xA2, 0xA6, 0xAA},
	},
};

/*-----------------------------------------------------------------------------
 *
 * readSmbusByteData - read a single SPD byte from any offset
 */

static int readSmbusByteData (int iobase, int address, char *buffer, int offset)
{
	unsigned int status;
	UINT64 limit;

	address |= 1; // set read bit

	outb(0xFF, iobase + 0);                // clear error status
	outb(0x1F, iobase + 1);                // clear error status
	outb(offset, iobase + 3);              // offset in eeprom
	outb(address, iobase + 4);             // slave address and read bit
	outb(0x48, iobase + 2);                // read byte command

	// time limit to avoid hanging for unexpected error status (should never happen)
	limit = __rdtsc () + 2000000000 / 10;
	for (;;)
	{
		status = inb(iobase);
		if (__rdtsc () > limit) break;
		if ((status & 2) == 0) continue;               // SMBusInterrupt not set, keep waiting
		if ((status & 1) == 1) continue;               // HostBusy set, keep waiting
		break;
	}

	buffer [0] = inb(iobase + 5);
	if (status == 2) status = 0;                      // check for done with no errors
	return status;
}

/*-----------------------------------------------------------------------------
 *
 * readSmbusByte - read a single SPD byte from the default offset
 *                 this function is faster function readSmbusByteData
 */

static int readSmbusByte (int iobase, int address, char *buffer)
{
	unsigned int status;
	UINT64 limit;

	outb(0xFF, iobase + 0);                // clear error status
	outb(0x44, iobase + 2);                // read command

	// time limit to avoid hanging for unexpected error status
	limit = __rdtsc () + 2000000000 / 10;
	for (;;)
	{
		status = inb(iobase);
		if (__rdtsc () > limit) break;
		if ((status & 2) == 0) continue;               // SMBusInterrupt not set, keep waiting
		if ((status & 1) == 1) continue;               // HostBusy set, keep waiting
		break;
	}

	buffer [0] = inb(iobase + 5);
	if (status == 2) status = 0;                      // check for done with no errors
	return status;
}

/*---------------------------------------------------------------------------
 *
 * readspd - Read one or more SPD bytes from a DIMM.
 *           Start with offset zero and read sequentially.
 *           Optimization relies on autoincrement to avoid
 *           sending offset for every byte.
 *          Reads 128 bytes in 7-8 ms at 400 KHz.
 */

static int readspd (int iobase, int SmbusSlaveAddress, char *buffer, int count)
{
	int index, error;

	/* read the first byte using offset zero */
	error = readSmbusByteData (iobase, SmbusSlaveAddress, buffer, 0);
	if (error) {
		return error;
	}

	/* read the remaining bytes using auto-increment for speed */
	for (index = 1; index < count; index++)
	{
		error = readSmbusByte (iobase, SmbusSlaveAddress, buffer + index);
		if (error)
			return error;
	}

	return 0;
}

static void setupFch (int ioBase)
{
	outb(66000000 / 400000 / 4, ioBase + 0x0E); /* set SMBus clock to 400 KHz */
}

AGESA_STATUS AmdMemoryReadSPD (UINT32 unused1, UINT32 unused2, AGESA_READ_SPD_PARAMS *info)
{
	AGESA_STATUS status;
	int spdAddress, ioBase;
	u8 i2c_channel;
	u8 backup;
	device_t sm_dev;

	if (info->SocketId     >= DIMENSION (spdAddressLookup      )) return AGESA_ERROR;
	if (info->MemChannelId >= DIMENSION (spdAddressLookup[0]   )) return AGESA_ERROR;
	if (info->DimmId       >= DIMENSION (spdAddressLookup[0][0])) return AGESA_ERROR;
	i2c_channel = (UINT8) info->SocketId;

	/* set IDTQS3253 i2c channel
	 * GPIO49,48 control the S1,S0
	 *  S1 S0 true table
	 *   0  0   channel 0
	 *   0  1   channel 1
	 *   1  0   channel 2   -  Socket 0
	 *   1  1   channel 3   -  Socket 1
	 * Note: Above is abstracted from Schemetic. But actually it seems to be other way.
	 *   1  0   channel 2   -  Socket 1
	 *   1  1   channel 3   -  Socket 0
	 * Note: The DIMMs need to be plugged in from the farthest slot for each channel.
	 */
	backup = sp5100_set_gpio(SP5100_GPIO49_52, 0x1, 0x1);
	sp5100_set_gpio(SP5100_GPIO37_46_47_48, (~i2c_channel & 1) << 3, 0x1 << 3);

	spdAddress = spdAddressLookup [info->SocketId] [info->MemChannelId] [info->DimmId];
	if (spdAddress == 0)
		return AGESA_ERROR;

	/*
	 * SMBus Base Address was set during southbridge early setup.
	 * e.g. sb700 IO mapped SMBUS_IO_BASE 0x6000, CIMX using 0xB00 as default
	 */
	sm_dev = pci_locate_device(PCI_ID(PCI_VENDOR_ID_ATI, PCI_DEVICE_ID_ATI_SB700_SM), 0);
	ioBase = pci_read_config32(sm_dev, 0x90) & (0xFFFFFFF0);
	setupFch(ioBase);

	status = readspd(ioBase, spdAddress, (void *)info->Buffer, 256);
	sp5100_restore_gpio(SP5100_GPIO49_52, backup);

	return status;
}
