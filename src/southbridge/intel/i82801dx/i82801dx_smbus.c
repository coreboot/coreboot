#include "i82801dx.h"
#include <smbus.h>
#include <pci.h>
#include <arch/io.h>

#define PM_BUS 0
#define PM_DEVFN PCI_DEVFN(0x1f,3)

#if 0
#define SMBUS_IO_BASE 0x1000
#define SMBHSTSTAT 0
#define SMBHSTCTL  2
#define SMBHSTCMD  3
#define SMBHSTADD  4
#define SMBHSTDAT0 5
#define SMBHSTDAT1 6
#define SMBBLKDAT  7
#endif

void smbus_enable(void)
{
	unsigned char byte;
	/* iobase addr */
	pcibios_write_config_dword(PM_BUS, PM_DEVFN, 0x20, SMBUS_IO_BASE | 1);
	/* smbus enable */
	pcibios_write_config_byte(PM_BUS, PM_DEVFN, 0x40,  1);
	/* iospace enable */
	pcibios_write_config_word(PM_BUS, PM_DEVFN, 0x4, 1);

        /* Disable interrupt generation */
        outb(0, SMBUS_IO_BASE + SMBHSTCTL);

}

void smbus_setup(void)
{
	outb(0, SMBUS_IO_BASE + SMBHSTSTAT);
}

static void smbus_wait_until_ready(void)
{
	while((inb(SMBUS_IO_BASE + SMBHSTSTAT) & 1) == 1) {
		/* nop */
	}
}

static void smbus_wait_until_done(void)
{
	unsigned char byte;
	do {
		byte = inb(SMBUS_IO_BASE + SMBHSTSTAT);
	}
	while((byte &1) == 1);
	while( (byte & ~1) == 0) {
		byte = inb(SMBUS_IO_BASE + SMBHSTSTAT);
	}
}

int smbus_read_byte(unsigned device, unsigned address, unsigned char *result)
{
	unsigned char host_status_register;
	unsigned char byte;

	smbus_wait_until_ready();

	/* setup transaction */
	/* disable interrupts */
	outb(inb(SMBUS_IO_BASE + SMBHSTCTL) & (~1), SMBUS_IO_BASE + SMBHSTCTL);
	/* set the device I'm talking too */
	outb(((device & 0x7f) << 1) | 1, SMBUS_IO_BASE + SMBHSTADD);
	/* set the command/address... */
	outb(address & 0xFF, SMBUS_IO_BASE + SMBHSTCMD);
	/* set up for a byte data read */
	outb((inb(SMBUS_IO_BASE + SMBHSTCTL) & 0xE3) | (0x2 << 2), SMBUS_IO_BASE + SMBHSTCTL);

	/* clear any lingering errors, so the transaction will run */
	outb(inb(SMBUS_IO_BASE + SMBHSTSTAT), SMBUS_IO_BASE + SMBHSTSTAT);

	/* clear the data byte...*/
	outb(0, SMBUS_IO_BASE + SMBHSTDAT0);

	/* start the command */
	outb((inb(SMBUS_IO_BASE + SMBHSTCTL) | 0x40), SMBUS_IO_BASE + SMBHSTCTL);

	/* poll for transaction completion */
	smbus_wait_until_done();

	host_status_register = inb(SMBUS_IO_BASE + SMBHSTSTAT);

	/* read results of transaction */
	byte = inb(SMBUS_IO_BASE + SMBHSTDAT0);

	*result = byte;
	return host_status_register != 0x02;
}
