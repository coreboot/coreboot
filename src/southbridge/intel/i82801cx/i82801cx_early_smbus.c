#include <device/pci_ids.h>
#include "i82801cx.h"

static void enable_smbus(void)
{
	device_t dev = PCI_DEV(0x0, 0x1f, 0x3);

	print_debug("SMBus controller enabled\n");
	/* set smbus iobase */
	pci_write_config32(dev, SMB_BASE, SMBUS_IO_BASE | PCI_BASE_ADDRESS_SPACE_IO);
	/* Set smbus enable */
	pci_write_config8(dev, HOSTC, HST_EN);
	/* Set smbus iospace enable */
	pci_write_config16(dev, PCI_COMMAND, PCI_COMMAND_IO);
	/* Disable interrupt generation */
	outb(0, SMBUS_IO_BASE + SMBHSTCTL);
	/* clear any lingering errors, so the transaction will run */
	outb(inb(SMBUS_IO_BASE + SMBHSTSTAT), SMBUS_IO_BASE + SMBHSTSTAT);
}


static inline void smbus_delay(void)
{
	outb(0x80, 0x80);
}

// See http://www.coreboot.org/pipermail/linuxbios/2004-September/009077.html
// for a description of this function.
static int smbus_wait_until_active(void)
{
	unsigned long loops;
	loops = SMBUS_TIMEOUT;
	do {
		unsigned char val;
		smbus_delay();
		val = inb(SMBUS_IO_BASE + SMBHSTSTAT);
		if ((val & 1)) {
			break;
		}
	} while (--loops);
	return loops ? 0 : -4;
}

static int smbus_wait_until_ready(void)
{
	unsigned long loops;
	loops = SMBUS_TIMEOUT;
	do {
		unsigned char val;
		smbus_delay();
		val = inb(SMBUS_IO_BASE + SMBHSTSTAT);
		// !HOST_BUSY?
		if ((val & 1) == 0) {
			break;
		}
		if(loops == (SMBUS_TIMEOUT / 2)) {
			// Clear status flags
			outb(inb(SMBUS_IO_BASE + SMBHSTSTAT),
				SMBUS_IO_BASE + SMBHSTSTAT);
		}
	} while(--loops);
	return loops?0:-2;
}

static int smbus_wait_until_done(void)
{
	unsigned long loops;
	loops = SMBUS_TIMEOUT;
	do {
		unsigned char val;
		smbus_delay();

		val = inb(SMBUS_IO_BASE + SMBHSTSTAT);
		// !HOST_BUSY?
		if ( (val & 1) == 0) {
			break;
		}
		// BYTE_DONE or SUCCESS or error?
		if ((val & ~((1<<6)|(1<<0)) ) != 0 ) {
			break;
		}
	} while(--loops);
	return loops?0:-3;
}

static int smbus_read_byte(unsigned device, unsigned address)
{
	unsigned char global_control_register;
	unsigned char global_status_register;
	unsigned char byte;

	if (smbus_wait_until_ready() < 0) {
		return -2;
	}

	/* setup transaction */
	/* disable interrupts */
	outb(inb(SMBUS_IO_BASE + SMBHSTCTL) & 0xfe, SMBUS_IO_BASE + SMBHSTCTL);
	/* set to read from the specified device  */
	outb(((device & 0x7f) << 1) | 1, SMBUS_IO_BASE + SMBXMITADD);
	/* set the command/address... */
	outb(address & 0xFF, SMBUS_IO_BASE + SMBHSTCMD);
	/* set up for a byte data read */
	outb((inb(SMBUS_IO_BASE + SMBHSTCTL) & 0xe3) | (0x2<<2), SMBUS_IO_BASE + SMBHSTCTL);

	/* clear any lingering errors, so the transaction will run */
	outb(inb(SMBUS_IO_BASE + SMBHSTSTAT), SMBUS_IO_BASE + SMBHSTSTAT);

	/* clear the data byte...*/
	outb(0, SMBUS_IO_BASE + SMBHSTDAT0);

	/* start a byte read, with interrupts disabled */
	outb((inb(SMBUS_IO_BASE + SMBHSTCTL) | 0x40), SMBUS_IO_BASE + SMBHSTCTL);
	/* poll for it to start */
	if (smbus_wait_until_active() < 0) {
		return -4;
	}

	/* poll for transaction completion */
	if (smbus_wait_until_done() < 0) {
		return -3;
	}

	global_status_register = inb(SMBUS_IO_BASE + SMBHSTSTAT) & ~(1<<6); /* Ignore the In Use Status... */

	/* read results of transaction */
	byte = inb(SMBUS_IO_BASE + SMBHSTDAT0);

	// SUCCESS?
	if (global_status_register != 2) {
		return -1;
	}
	return byte;
}
