#include <smbus.h>
#include <arch/io.h>
#include <pci.h>
#include <southbridge/intel/82801.h>
#include "82801.h"


#define SMBUS_IO_BASE 0x1000

#define SMBHSTSTAT 0x0
#define SMBHSTCTL  0x2
#define SMBHSTCMD  0x3
#define SMBXMITADD 0x4
#define SMBHSTDAT0 0x5
#define SMBHSTDAT1 0x6
#define SMBBLKDAT  0x7
#define SMBTRNSADD 0x9
#define SMBSLVDATA 0xa
#define SMLINK_PIN_CTL 0xe
#define SMBUS_PIN_CTL  0xf 

void smbus_setup(void)
{
	pcibios_write_config_dword(SMBUS_BUS, SMBUS_DEVFN, 0x20, SMBUS_IO_BASE | 1);
	pcibios_write_config_byte(SMBUS_BUS, SMBUS_DEVFN, 0x40, 1);
	pcibios_write_config_word(SMBUS_BUS, SMBUS_DEVFN, 0x4, 1);

	/* Disable interrupt generation */
	outb(0, SMBUS_IO_BASE + SMBHSTCTL);
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
	} while((byte &1) == 1);
	while( (byte & ~((1<<6)|(1<<0))) == 0) {
		byte = inb(SMBUS_IO_BASE + SMBHSTSTAT);
	}
}

#if 0
static void smbus_print_error(unsigned char host_status_register)
{

	printk_debug("smbus_error: 0x%02x\n", host_status_register);
	if (host_status_register & (1 << 7)) {
		printk_debug("Byte Done Status\n");
	}
	if (host_status_register & (1 << 6)) {
		printk_debug("In Use Status\n");
	}
	if (host_status_register & (1 << 5)) {
		printk_debug("SMBus Alert Status\n");
	}
	if (host_status_register & (1 << 4)) {
		printk_debug("Interrup/SMI# was Failed Bus Transaction\n");
	}
	if (host_status_register & (1 << 3)) {
		printk_debug("Bus Error\n");
	}
	if (host_status_register & (1 << 2)) {
		printk_debug("Device Error\n");
	}
	if (host_status_register & (1 << 1)) {
		printk_debug("Interrupt/SMI# was Successful Completion\n");
	}
	if (host_status_register & (1 << 0)) {
		printk_debug("Host Busy\n");
	}
}
#endif

int smbus_read_byte(unsigned device, unsigned address, unsigned char *result)
{
	unsigned char host_status_register;
	unsigned char byte;

	smbus_wait_until_ready();

	/* setup transaction */
	/* disable interrupts */
	outb(inb(SMBUS_IO_BASE + SMBHSTCTL) & (~1), SMBUS_IO_BASE + SMBHSTCTL);
	/* set the device I'm talking too */
	outb(((device & 0x7f) << 1) | 1, SMBUS_IO_BASE + SMBXMITADD);
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

	/* Ignore the In Use Status... */
	host_status_register &= ~(1 << 6);

	/* read results of transaction */
	byte = inb(SMBUS_IO_BASE + SMBHSTDAT0);

	*result = byte;
	return host_status_register != 0x02;
}
