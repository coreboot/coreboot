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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <device/pci_def.h>
#include <device/device.h>

/* warning: Porting.h includes an open #pragma pack(1) */
#include "Porting.h"
#include "AGESA.h"
#include "amdlib.h"
#include "dimmSpd.h"
#include "chip.h"


#define DIMENSION(array)(sizeof (array)/ sizeof (array [0]))

/*-----------------------------------------------------------------------------
 *
 * readSmbusByteData - read a single SPD byte from any offset
 */

static int readSmbusByteData (int iobase, int address, char *buffer, int offset)
{
	unsigned int status;
	UINT64 limit;

	address |= 1; // set read bit

	__outbyte (iobase + 0, 0xFF);                // clear error status
	__outbyte (iobase + 1, 0x1F);                // clear error status
	__outbyte (iobase + 3, offset);              // offset in eeprom
	__outbyte (iobase + 4, address);             // slave address and read bit
	__outbyte (iobase + 2, 0x48);                // read byte command

	// time limit to avoid hanging for unexpected error status (should never happen)
	limit = __rdtsc () + 2000000000 / 10;
	for (;;)
	{
		status = __inbyte (iobase);
		if (__rdtsc () > limit) break;
		if ((status & 2) == 0) continue;               // SMBusInterrupt not set, keep waiting
		if ((status & 1) == 1) continue;               // HostBusy set, keep waiting
		break;
	}

	buffer [0] = __inbyte (iobase + 5);
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

	__outbyte (iobase + 0, 0xFF);                // clear error status
	__outbyte (iobase + 2, 0x44);                // read command

	// time limit to avoid hanging for unexpected error status
	limit = __rdtsc () + 2000000000 / 10;
	for (;;)
	{
		status = __inbyte (iobase);
		if (__rdtsc () > limit) break;
		if ((status & 2) == 0) continue;               // SMBusInterrupt not set, keep waiting
		if ((status & 1) == 1) continue;               // HostBusy set, keep waiting
		break;
	}

	buffer [0] = __inbyte (iobase + 5);
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
	if (error) return error;

	/* read the remaining bytes using auto-increment for speed */
	for (index = 1; index < count; index++)
	{
		error = readSmbusByte (iobase, SmbusSlaveAddress, &buffer [index]);
		if (error) return error;
	}

	return 0;
}

static void writePmReg (int reg, int data)
{
	__outbyte (0xCD6, reg);
	__outbyte (0xCD7, data);
}

static void setupFch (int ioBase)
{
	writePmReg (0x2D, ioBase >> 8);
	writePmReg (0x2C, ioBase | 1);
	__outbyte (ioBase + 0x0E, 66000000 / 400000 / 4); // set SMBus clock to 400 KHz
}

AGESA_STATUS AmdMemoryReadSPD (UINT32 unused1, UINT32 unused2, AGESA_READ_SPD_PARAMS *info)
{
	int spdAddress, ioBase;
	ROMSTAGE_CONST struct device *dev = dev_find_slot(0, PCI_DEVFN(0x18, 2));
	ROMSTAGE_CONST struct northbridge_amd_agesa_family16kb_config *config = dev->chip_info;

	if ((dev == 0) || (config == 0))
		return AGESA_ERROR;

	if (info->SocketId     >= DIMENSION(config->spdAddrLookup      ))
		return AGESA_ERROR;
	if (info->MemChannelId >= DIMENSION(config->spdAddrLookup[0]   ))
		return AGESA_ERROR;
	if (info->DimmId       >= DIMENSION(config->spdAddrLookup[0][0]))
		return AGESA_ERROR;

	spdAddress = config->spdAddrLookup
		[info->SocketId] [info->MemChannelId] [info->DimmId];

	if (spdAddress == 0) return AGESA_ERROR;
	ioBase = 0xB00;
	setupFch (ioBase);
	return readspd (ioBase, spdAddress, (void *) info->Buffer, 128);
}
