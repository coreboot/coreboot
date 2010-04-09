/*
 * Copyright 2004 Tyan Computer
 *  by yhlu@tyan.com
 */

#include "ck804_smbus.h"

#define SMBUS_IO_BASE 0x1000

static void enable_smbus(void)
{
	device_t dev;
	dev = pci_locate_device(PCI_ID(0x10de, 0x0052), 0);
	if (dev == PCI_DEV_INVALID)
		die("SMBus controller not found\n");

	print_debug("SMBus controller enabled\n");

	/* Set SMBus I/O base. */
	pci_write_config32(dev, 0x20, SMBUS_IO_BASE | 1);

	/* Set SMBus I/O space enable. */
	pci_write_config16(dev, 0x4, 0x01);

	/* Clear any lingering errors, so the transaction will run. */
	outb(inb(SMBUS_IO_BASE + SMBHSTSTAT), SMBUS_IO_BASE + SMBHSTSTAT);
}

static int smbus_read_byte(unsigned device, unsigned address)
{
	return do_smbus_read_byte(SMBUS_IO_BASE, device, address);
}

static inline int smbus_write_byte(unsigned device, unsigned address,
			    unsigned char val)
{
	return do_smbus_write_byte(SMBUS_IO_BASE, device, address, val);
}
