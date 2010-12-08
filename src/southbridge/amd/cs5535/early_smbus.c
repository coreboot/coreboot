#include "smbus.h"

#define SMBUS_IO_BASE 0x6000

/* initialization for SMBus Controller */
static void cs5535_enable_smbus(void)
{
	unsigned char val;

	/* reset SMBUS controller */
	outb(0, SMBUS_IO_BASE + SMB_CTRL2);

	/* Set SCL freq and enable SMB controller */
	val = inb(SMBUS_IO_BASE + SMB_CTRL2);
	val |= ((0x20 << 1) | SMB_CTRL2_ENABLE);
	outb(val, SMBUS_IO_BASE + SMB_CTRL2);

	/* Setup SMBus host controller address to 0xEF */
	val = inb(SMBUS_IO_BASE + SMB_ADD);
	val |= (0xEF | SMB_ADD_SAEN);
	outb(val, SMBUS_IO_BASE + SMB_ADD);
}
