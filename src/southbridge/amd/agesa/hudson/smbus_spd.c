/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <device/pci_def.h>
#include <device/device.h>
#include <console/console.h>
#include <stddef.h>
/* warning: Porting.h includes an open #pragma pack(1) */
#include <Porting.h>
#include <AGESA.h>
#include <amdlib.h>
#include <northbridge/amd/agesa/dimmSpd.h>

/*-----------------------------------------------------------------------------
 *
 * readSmbusByteData - read a single SPD byte from any offset
 */

static int readSmbusByteData(int iobase, int address, char *buffer, int offset)
{
	unsigned int status;
	UINT64 limit;

	address |= 1; // set read bit

	__outbyte(iobase + 0, 0xFF);                // clear error status
	__outbyte(iobase + 1, 0x1F);                // clear error status
	__outbyte(iobase + 3, offset);              // offset in eeprom
	__outbyte(iobase + 4, address);             // slave address and read bit
	__outbyte(iobase + 2, 0x48);                // read byte command

	// time limit to avoid hanging for unexpected error status (should never happen)
	limit = __rdtsc() + 2000000000 / 10;
	for (;;)
	{
		status = __inbyte(iobase);
		if (__rdtsc() > limit) break;
		if ((status & 2) == 0) continue;               // SMBusInterrupt not set, keep waiting
		if ((status & 1) == 1) continue;               // HostBusy set, keep waiting
		break;
	}

	buffer [0] = __inbyte(iobase + 5);
	if (status == 2) status = 0;                      // check for done with no errors
	return status;
}

/*-----------------------------------------------------------------------------
 *
 * readSmbusByte - read a single SPD byte from the default offset
 *                 this function is faster function readSmbusByteData
 */

static int readSmbusByte(int iobase, int address, char *buffer)
{
	unsigned int status;
	UINT64 limit;

	__outbyte(iobase + 0, 0xFF);                // clear error status
	__outbyte(iobase + 2, 0x44);                // read command

	// time limit to avoid hanging for unexpected error status
	limit = __rdtsc() + 2000000000 / 10;
	for (;;)
	{
		status = __inbyte(iobase);
		if (__rdtsc() > limit) break;
		if ((status & 2) == 0) continue;               // SMBusInterrupt not set, keep waiting
		if ((status & 1) == 1) continue;               // HostBusy set, keep waiting
		break;
	}

	buffer [0] = __inbyte(iobase + 5);
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
	for (index = 1; index < count; index++)
	{
		error = readSmbusByte(iobase, SmbusSlaveAddress, &buffer [index]);
		if (error) {
			printk(BIOS_ERR, "-------------SPD READ ERROR-----------\n");
			return error;
		}
	}
	printk(BIOS_SPEW, "\n");
	printk(BIOS_SPEW, "-------------FINISHED READING SPD-----------\n");

	return 0;
}

static void setupFch(int ioBase)
{
	pm_write16(0x2c, ioBase | 1);
	__outbyte(ioBase + 0x0E, 66000000 / 400000 / 4); // set SMBus clock to 400 KHz
}

int hudson_readSpd(int spdAddress, char *buf, size_t len)
{
	int ioBase = 0xB00;
	setupFch(ioBase);
	return readspd(ioBase, spdAddress, buf, len);
}
