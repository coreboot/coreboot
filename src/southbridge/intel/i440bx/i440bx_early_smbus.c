#include "i440bx_smbus.h"

#define SMBUS_IO_BASE 0x0f00

static void enable_smbus(void)
{
	device_t dev;
	dev = pci_locate_device(PCI_ID(0x8086, 0x7113), 0);
	if (dev == PCI_DEV_INVALID) {
		die("SMBUS controller not found\r\n");
	}
	uint8_t enable;
	print_spew("SMBus controller enabled\r\n");
	pci_write_config32(dev, 0x90, SMBUS_IO_BASE );
	// Enable and set SMBBus 
	// 0x01 Interrupt to SMI# 
	// (0x4<<1)|1 set interrupt to IRQ9
	pci_write_config8(dev, 0xd2, (0x4<<1)|1);
	
	// Enable the IO space
	pci_write_config16(dev, 0x04, 1);
	
	/* clear any lingering errors, so the transaction will run */
	outb(0x1e, SMBUS_IO_BASE + SMBGSTATUS);
}

static int smbus_recv_byte(unsigned device)
{
	return do_smbus_recv_byte(SMBUS_IO_BASE, device);
}

static int smbus_send_byte(unsigned device, unsigned char val)
{
	return do_smbus_send_byte(SMBUS_IO_BASE, device, val);
}

static int smbus_read_byte(unsigned device, unsigned address)
{
	return do_smbus_read_byte(SMBUS_IO_BASE, device, address);
}

static int smbus_write_byte(unsigned device, unsigned address, unsigned char val)
{
	return do_smbus_write_byte(SMBUS_IO_BASE, device, address, val);
}
