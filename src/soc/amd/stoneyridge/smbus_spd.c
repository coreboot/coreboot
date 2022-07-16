/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/agesawrapper.h>
#include <amdblocks/acpimmio.h>
#include <console/console.h>
#include <device/pci_def.h>
#include <device/device.h>
#include <device/smbus_host.h>
#include <soc/southbridge.h>
#include <amdblocks/dimm_spd.h>

/*
 * readspd - Read one or more SPD bytes from a DIMM.
 *           Start with offset zero and read sequentially.
 *           Optimization relies on autoincrement to avoid
 *           sending offset for every byte.
 *          Reads 128 bytes in 7-8 ms at 400 KHz.
 */
static int readspd(uint8_t SmbusSlaveAddress, char *buffer, size_t count)
{
	uint8_t dev_addr;
	size_t index;
	int error;
	char *pbuf = buffer;

	printk(BIOS_SPEW, "-------------READING SPD-----------\n");
	printk(BIOS_SPEW, "SmbusSlave: 0x%08X, count: %zd\n",
					SmbusSlaveAddress, count);

	/*
	 * Convert received device address to the format accepted by
	 * do_smbus_read_byte and do_smbus_recv_byte.
	 */
	dev_addr = (SmbusSlaveAddress >> 1);

	/* Read the first SPD byte */
	error = do_smbus_read_byte((uintptr_t)acpimmio_smbus, dev_addr, 0);
	if (error < 0) {
		printk(BIOS_ERR, "-------------SPD READ ERROR-----------\n");
		return error;
	}
	*pbuf = (char)error;
	pbuf++;

	/* Read the remaining SPD bytes using do_smbus_recv_byte for speed */
	for (index = 1 ; index < count ; index++) {
		error = do_smbus_recv_byte((uintptr_t)acpimmio_smbus, dev_addr);
		if (error < 0) {
			printk(BIOS_ERR, "-------------SPD READ ERROR-----------\n");
			return error;
		}
		*pbuf = (char)error;
		pbuf++;
	}
	printk(BIOS_SPEW, "\n");
	printk(BIOS_SPEW, "-------------FINISHED READING SPD-----------\n");

	return 0;
}

int sb_read_spd(uint8_t spdAddress, char *buf, size_t len)
{
	return readspd(spdAddress, buf, len);
}
