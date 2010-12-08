#include "smbus.h"

#define SMBUS_IO_BASE 0x0f00

static void enable_smbus(void)
{
	device_t dev = PCI_DEV(0x0, 0x1f, 0x3);

	print_spew("SMBus controller enabled\n");
	pci_write_config32(dev, 0x20, SMBUS_IO_BASE | 1);
	pci_write_config8(dev, 0x40, 1);
	pci_write_config8(dev, 0x4, 1);
	/* SMBALERT_DIS */
	pci_write_config8(dev, 0x11, 4);

	/* Disable interrupt generation */
	outb(0, SMBUS_IO_BASE + SMBHSTCTL);
}

static int smbus_read_byte(unsigned device, unsigned address)
{
	return do_smbus_read_byte(SMBUS_IO_BASE, device, address);
}

#ifdef DEADCODE
static void smbus_write_byte(unsigned device, unsigned address, unsigned char val)
{
	if (smbus_wait_until_ready(SMBUS_IO_BASE) < 0) {
		return;
	}
	return;
}

static int smbus_write_block(unsigned device, unsigned length, unsigned cmd,
		 unsigned data1, unsigned data2)
{
	unsigned char byte;
	unsigned char stat;
	int i;

	/* chear the PM timeout flags, SECOND_TO_STS */
	outw(inw(0x0400 + 0x66), 0x0400 + 0x66);

	if (smbus_wait_until_ready(SMBUS_IO_BASE) < 0) {
		return -2;
	}

	/* setup transaction */
	/* Obtain ownership */
	outb(inb(SMBUS_IO_BASE + SMBHSTSTAT), SMBUS_IO_BASE + SMBHSTSTAT);
	for(stat=0;(stat&0x40)==0;) {
	stat = inb(SMBUS_IO_BASE + SMBHSTSTAT);
	}
	/* clear the done bit */
	outb(0x80, SMBUS_IO_BASE + SMBHSTSTAT);
	/* disable interrupts */
	outb(inb(SMBUS_IO_BASE + SMBHSTCTL) & (~1), SMBUS_IO_BASE + SMBHSTCTL);

	/* set the device I'm talking too */
	outb(((device & 0x7f) << 1), SMBUS_IO_BASE + SMBXMITADD);

	/* set the command address */
	outb(cmd & 0xFF, SMBUS_IO_BASE + SMBHSTCMD);

	/* set the block length */
	outb(length & 0xFF, SMBUS_IO_BASE + SMBHSTDAT0);

	/* try sending out the first byte of data here */
	byte=(data1>>(0))&0x0ff;
	outb(byte,SMBUS_IO_BASE + SMBBLKDAT);
	/* issue a block write command */
	outb((inb(SMBUS_IO_BASE + SMBHSTCTL) & 0xE3) | (0x5 << 2) | 0x40,
			SMBUS_IO_BASE + SMBHSTCTL);

	for(i=0;i<length;i++) {

		/* poll for transaction completion */
		if (smbus_wait_until_blk_done(SMBUS_IO_BASE) < 0) {
			return -3;
		}

		/* load the next byte */
		if(i>3)
			byte=(data2>>(i%4))&0x0ff;
		else
			byte=(data1>>(i))&0x0ff;
		outb(byte,SMBUS_IO_BASE + SMBBLKDAT);

		/* clear the done bit */
		outb(inb(SMBUS_IO_BASE + SMBHSTSTAT),
				SMBUS_IO_BASE + SMBHSTSTAT);
	}

	print_debug("SMBUS Block complete\n");
	return 0;
}
#endif

