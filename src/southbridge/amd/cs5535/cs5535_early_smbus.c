#include "cs5535_smbus.h"

#define SMBUS_IO_BASE 0x6000

/* initialization for SMBus Controller */
static int enable_smbus(void)
{
	unsigned char val;

	/* FixME: move to early_iobase.c */
	/* setup LBAR for SMBus controller */
	__builtin_wrmsr(0x5140000B, 0x00006000, 0x0000f001);
	/* setup LBAR for GPIO at 0x6100 */
	__builtin_wrmsr(0x5140000C, 0x00006100, 0x0000f001);


	/* setup GPIO pins for SDA/SCL */

	/* Setup SMBus host controller address  to 0xEF */
	val = inb(SMBUS_IO_BASE + SMB_ADD);
	val |= (0xEF | SMB_ADD_SAEN);
	outb(val, SMBUS_IO_BASE + SMB_ADD); 

	/* Set SCL freq and enable SMB controller */
	outb(0x00, SMBUS_IO_BASE + SMB_CTRL2);
	val = inb(SMBUS_IO_BASE + SMB_CTRL2);
	val |= (0x7F < 1) | SMB_CTRL2_ENABLE;
	outb(val, SMBUS_IO_BASE + SMB_CTRL2);

	/* Is SDA pulled high ? */
	val = inb(SMBUS_IO_BASE + SMB_CTRL_STS);
	if (val & SMB_CSTS_TSDA)
		return SMBUS_ERROR;

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
