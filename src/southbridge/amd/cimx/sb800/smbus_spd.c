/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_def.h>
#include <device/device.h>
#include <stddef.h>
#include <OEM.h>		/* SMBUS0_BASE_ADDRESS */

/* warning: Porting.h includes an open #pragma pack(1) */
#include <vendorcode/amd/include/Porting.h>
#include <AGESA.h>
#include "chip.h"
#include "smbus_spd.h"

#include <northbridge/amd/agesa/dimmSpd.h>

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

int smbus_readSpd(int spdAddress, char *buf, size_t len)
{
	int ioBase = SMBUS0_BASE_ADDRESS;
	setupFch(ioBase);
	return readspd(ioBase, spdAddress, buf, len);
}
