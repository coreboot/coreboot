/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 * Copyright (C) 2013 Sage Electronic Engineering, LLC
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

#if CONFIG_DDR3_SOLDERED_DOWN
#include CONFIG_PATH_TO_DDR3_SPD
AGESA_STATUS calc_fake_spd_crc( UINT8 *SPDPtr, UINT16 *crc );
#endif

/* uncomment for source level debug - GDB gets really confused otherwise. */
//#pragma optimize ("", off)

#if !CONFIG_DDR3_SOLDERED_DOWN
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

#else // CONFIG_DDR3_SOLDERED_DOWN
/*
 * Get the SPD from the mainboard
 */
AGESA_STATUS agesa_ReadSPD(UINT32 unused1, UINT32 unused2, void *infoptr)
{
	UINT8 *spd_ptr;
	UINT16 index, crc;

	AGESA_READ_SPD_PARAMS *info = infoptr;
	ROMSTAGE_CONST struct device *dev = dev_find_slot(0, PCI_DEVFN(0x18, 2));

	if ((dev == 0) || (dev->chip_info == 0))
		return AGESA_ERROR;

	if (info->MemChannelId > CONFIG_DDR3_CHANNEL_MAX) return AGESA_ERROR;
	if (info->SocketId     != 0)  return AGESA_ERROR;
	if (info->DimmId       != 0)  return AGESA_ERROR;

	/* read the bytes from the table */
	spd_ptr = (UINT8 *)info->Buffer;
	for (index = 0; index < 128; index++)
		spd_ptr[index] = ddr3_fake_spd[index];

	/* If CRC bytes are zeroes, calculate and store the CRC of the fake table */
	if ((spd_ptr[126] == 0) && (spd_ptr[127] == 0)) {
		calc_fake_spd_crc( spd_ptr, &crc );
		spd_ptr[126] = (UINT8)(crc & 0xFF);
		spd_ptr[127] = (UINT8)(crc>>8);
	}

	/* print out the table */
	printk(BIOS_SPEW, "\nDump the fake SPD for Channel %d\n",info->MemChannelId);
	for (index = 0; index < 128; index++) {
		if((index&0x0F)==0x00) printk(BIOS_SPEW, "%02x:  ",index);
		printk(BIOS_SPEW, "%02x ", spd_ptr[index]);
		if((index&0x0F)==0x0F) printk(BIOS_SPEW, "\n");
	}
	return AGESA_SUCCESS;
}

AGESA_STATUS calc_fake_spd_crc( UINT8 *SPDPtr, UINT16 *crc )
{
	INT16 i;
	INT16 j;
	INT16 jmax;

	/* should the CRC be done on bytes 0-116 or 0-125 ? */
	if (SPDPtr[0] & 0x80)
		 jmax = 117;
	else jmax = 126;

	*crc = 0; /* zero out the CRC */

	for (j = 0; j < jmax; j++) {
		*crc = *crc ^ ((UINT16)SPDPtr[j] << 8);
		for (i = 0; i < 8; i++) {
			if (*crc & 0x8000) {
				*crc = (*crc << 1) ^ 0x1021;
			} else {
				*crc = (*crc << 1);
			}
		}
	}
	return TRUE;
}
#endif
