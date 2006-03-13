#include "cs5535_smbus.h"

#define SMBUS_IO_BASE 0x6000

/* initialization for SMBus Controller */
static int cs5535_enable_smbus(void)
{
	unsigned char val;

	outb(0, SMBUS_IO_BASE + SMB_CTRL2);

	/* Set SCL freq and enable SMB controller */
	val = inb(SMBUS_IO_BASE + SMB_CTRL2);
	val |= ((0x20 << 1) | SMB_CTRL2_ENABLE);
	outb(val, SMBUS_IO_BASE + SMB_CTRL2);

	/* Setup SMBus host controller address to 0xEF */
	val = inb(SMBUS_IO_BASE + SMB_ADD);
	val |= (0xEF | SMB_ADD_SAEN);
	outb(val, SMBUS_IO_BASE + SMB_ADD); 

#if 0
	print_debug("SMBUS registers ");
	print_debug_hex8(inb(SMBUS_IO_BASE));
	print_debug(" ");
	print_debug_hex8(inb(SMBUS_IO_BASE + 1));
	print_debug(" ");
	print_debug_hex8(inb(SMBUS_IO_BASE + 2));
	print_debug(" ");
	print_debug_hex8(inb(SMBUS_IO_BASE + 3));
	print_debug(" ");
	print_debug_hex8(inb(SMBUS_IO_BASE + 4));
	print_debug(" ");
	print_debug_hex8(inb(SMBUS_IO_BASE + 5));
	print_debug(" ");
	print_debug_hex8(inb(SMBUS_IO_BASE + 6));
	print_debug("\n\r");
#endif
}

#if 0
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
#endif
