#define SMBUS_IO_BASE 0x0f00

#define SMBGSTATUS 0xe0
#define SMBGCTL    0xe2
#define SMBHSTADDR 0xe4
#define SMBHSTDAT  0xe6
#define SMBHSTCMD  0xe8
#define SMBHSTFIFO 0xe9

#define SMBUS_TIMEOUT (100*1000*10)

static void enable_smbus(void)
{
	device_t dev;
	dev = pci_locate_device(PCI_ID(0x1022, 0x746b), 0);
	if (dev == PCI_DEV_INVALID) {
		die("SMBUS controller not found\r\n");
	}
	uint8_t enable;
	print_debug("SMBus controller enabled\r\n");
	pci_write_config32(dev, 0x58, SMBUS_IO_BASE | 1);
	enable = pci_read_config8(dev, 0x41);
	pci_write_config8(dev, 0x41, enable | (1 << 7));
}


static inline void smbus_delay(void)
{
	outb(0x80, 0x80);
}

static int smbus_wait_until_ready(void)
{
	unsigned long loops;
	loops = SMBUS_TIMEOUT;
	do {
		unsigned short val;
		smbus_delay();
		val = inw(SMBUS_IO_BASE + SMBGSTATUS);
		if ((val & 0x800) == 0) {
			break;
		}
	} while(--loops);
	return loops?0:-1;
}

static int smbus_wait_until_done(void)
{
	unsigned long loops;
	loops = SMBUS_TIMEOUT;
	do {
		unsigned short val;
		smbus_delay();
		
		val = inw(SMBUS_IO_BASE + SMBGSTATUS);
		if (((val & 0x8) == 0) | ((val & 0x437) != 0)) {
			break;
		}
	} while(--loops);
	return loops?0:-1;
}

static int smbus_read_byte(unsigned device, unsigned address)
{
	unsigned char global_control_register;
	unsigned char global_status_register;
	unsigned char byte;

	if (smbus_wait_until_ready() < 0) {
		return -1;
	}
	
	/* setup transaction */
	/* disable interrupts */
	outw(inw(SMBUS_IO_BASE + SMBGCTL) & ~((1<<10)|(1<<9)|(1<<8)|(1<<4)), SMBUS_IO_BASE + SMBGCTL);
	/* set the device I'm talking too */
	outw(((device & 0x7f) << 1) | 1, SMBUS_IO_BASE + SMBHSTADDR);
	/* set the command/address... */
	outb(address & 0xFF, SMBUS_IO_BASE + SMBHSTCMD);
	/* set up for a byte data read */
	outw((inw(SMBUS_IO_BASE + SMBGCTL) & ~7) | (0x2), SMBUS_IO_BASE + SMBGCTL);

	/* clear any lingering errors, so the transaction will run */
	/* Do I need to write the bits to a 1 to clear an error? */
	outw(inw(SMBUS_IO_BASE + SMBGSTATUS), SMBUS_IO_BASE + SMBGSTATUS);

	/* clear the data word...*/
	outw(0, SMBUS_IO_BASE + SMBHSTDAT);

	/* start the command */
	outw((inw(SMBUS_IO_BASE + SMBGCTL) | (1 << 3)), SMBUS_IO_BASE + SMBGCTL);


	/* poll for transaction completion */
	if (smbus_wait_until_done() < 0) {
		return -1;
	}

	global_status_register = inw(SMBUS_IO_BASE + SMBGSTATUS);

	/* read results of transaction */
	byte = inw(SMBUS_IO_BASE + SMBHSTDAT) & 0xff;

	if (global_status_register != (1 << 4)) {
		return -1;
	}
	return byte;
}
