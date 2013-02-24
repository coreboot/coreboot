/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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
#include <stdlib.h>
#include "OEM.h"		/* SMBUS0_BASE_ADDRESS */

/* warning: Porting.h includes an open #pragma pack(1) */
#include "Porting.h"
#include "AGESA.h"
#include "amdlib.h"
#include "dimmSpd.h"
#include "chip.h"

/* uncomment for source level debug - GDB gets really confused otherwise. */
//#pragma optimize ("", off)

/**
 *	Read a single SPD byte.  If the first byte is being read, set up the
 *	address and offset. Following bytes auto increment.
 */
static UINT8 readSmbusByte(UINT16 iobase, UINT8 address, char *buffer,
						   int offset, int initial_offset)
{
	unsigned int status = -1;
	UINT64 time_limit;

	/* clear status register */
	__outbyte(iobase + SMBUS_STATUS_REG, 0x1E);

	if (offset == initial_offset) {
		/* Clear slave status, set offset, set slave address and start reading */
		__outbyte(iobase + SMBUS_SLAVE_STATUS_REG, 0x3E);
		__outbyte(iobase + SMBUS_CONTROL_REG, offset);
		__outbyte(iobase + SMBUS_HOST_CMD_REG, address | READ_BIT);
		__outbyte(iobase + SMBUS_COMMAND_REG, SMBUS_READ_BYTE_COMMAND);
	} else {
		/* Issue read command - auto increments to next byte */
		__outbyte(iobase + SMBUS_COMMAND_REG, SMBUS_READ_COMMAND);
	}
	/* time limit to avoid hanging for unexpected error status */
	time_limit = __rdtsc() + MAX_READ_TSC_COUNT;
	while (__rdtsc() <= time_limit) {
		status = __inbyte(iobase + SMBUS_STATUS_REG);
		if ((status & SMBUS_INTERRUPT_MASK) == 0)
			continue;	/* SMBusInterrupt not set, keep waiting */
		if ((status & HOSTBUSY_MASK) != 0)
			continue;	/* HostBusy set, keep waiting */
		break;
	}

	if (status != STATUS__COMPLETED_SUCCESSFULLY)
		return AGESA_ERROR;

	buffer[0] = __inbyte(iobase + SMBUS_DATA0_REG);
	return AGESA_SUCCESS;
}

static void writePmReg(UINT8 reg, UINT8 data)
{
	__outbyte(PMIO_INDEX_REG, reg);
	__outbyte(PMIO_DATA_REG, data);
}

static void setupFch(UINT16 ioBase)
{
	/* set up SMBUS - Set to SMBUS 0 & set base address */
	/* For SB800 & Hudson1 to SB900 & Hudson 2/3 */
	writePmReg(SMBUS_BAR_HIGH_BYTE, ioBase >> 8);
	writePmReg(SMBUS_BAR_LOW_BYTE, (ioBase & 0xe0) | 1);

	/* set SMBus clock to 400 KHz */
	__outbyte(ioBase + SMBUS_CLOCK_REG, SMBUS_FREQUENCY_CONST / 400000);
}

/**
 *	Read one or more SPD bytes from a DIMM.
 *	Start with offset zero and read sequentially.
 *	Reads 128 bytes in 7-8 ms at 400 KHz.
 */
static UINT8 readspd(UINT16 iobase, UINT8 SmbusSlaveAddress, char *buffer,
					 UINT16 count)
{
	UINT16 index;
	UINT8 status;
	UINT8 initial_offset = 0;

	setupFch(iobase);

	for (index = initial_offset; index < count; index++) {
		status = readSmbusByte(iobase, SmbusSlaveAddress, &buffer[index], index,
				initial_offset);
		if (status != AGESA_SUCCESS)
			return status;
	}

	return status;
}

/**
 * Gets the SMBUS address for an SPD from the array in devicetree.cb
 * then read the SPD into the supplied buffer.
 */
AGESA_STATUS agesa_ReadSPD(UINT32 unused1, UINT32 unused2, void *infoptr)
{
	UINT8 spdAddress;
	AGESA_READ_SPD_PARAMS *info = infoptr;
	ROMSTAGE_CONST struct device *dev = dev_find_slot(0, PCI_DEVFN(0x18, 2));
	ROMSTAGE_CONST struct northbridge_amd_agesa_family14_config *config = NULL;

	if ((dev == 0) || (dev->chip_info == 0))
		return AGESA_ERROR;

	config = dev->chip_info;
	if (info->SocketId >= ARRAY_SIZE(config->spdAddrLookup))
		return AGESA_ERROR;
	if (info->MemChannelId >= ARRAY_SIZE(config->spdAddrLookup[0]))
		return AGESA_ERROR;
	if (info->DimmId >= ARRAY_SIZE(config->spdAddrLookup[0][0]))
		return AGESA_ERROR;

	spdAddress = config->spdAddrLookup
		[info->SocketId][info->MemChannelId][info->DimmId];

	if (spdAddress == 0)
		return AGESA_ERROR;
	return readspd(SMBUS0_BASE_ADDRESS, spdAddress, (void *)info->Buffer, 128);
}
