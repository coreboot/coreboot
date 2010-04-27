#define HAVE_CONSTANT_PROPOGATION    1

void outb(unsigned char value, unsigned short port)
{
	__builtin_outb(value, port);
}

void outw(unsigned short value, unsigned short port)
{
	__builtin_outw(value, port);
}

void outl(unsigned int value, unsigned short port)
{
	__builtin_outl(value, port);
}

unsigned char inb(unsigned short port)
{
	return __builtin_inb(port);
}

unsigned char inw(unsigned short port)
{
	return __builtin_inw(port);
}

unsigned char inl(unsigned short port)
{
	return __builtin_inl(port);
}

static unsigned int config_cmd(unsigned char bus, unsigned devfn, unsigned where)
{
	return 0x80000000 | (bus << 16) | (devfn << 8) | (where & ~3);
}

static unsigned char pcibios_read_config_byte(
	unsigned char bus, unsigned devfn, unsigned where)
{
	outl(config_cmd(bus, devfn, where), 0xCF8);
	return inb(0xCFC + (where & 3));
}

static unsigned short pcibios_read_config_word(
	unsigned char bus, unsigned devfn, unsigned where)
{
	outl(config_cmd(bus, devfn, where), 0xCF8);
	return inw(0xCFC + (where & 2));
}

static unsigned int pcibios_read_config_dword(
	unsigned char bus, unsigned devfn, unsigned where)
{
	outl(config_cmd(bus, devfn, where), 0xCF8);
	return inl(0xCFC);
}


static void pcibios_write_config_byte(
	unsigned char bus, unsigned devfn, unsigned where, unsigned char value)
{
	outl(config_cmd(bus, devfn, where), 0xCF8);
	outb(value, 0xCFC + (where & 3));
}

static void pcibios_write_config_word(
	unsigned char bus, unsigned devfn, unsigned where, unsigned short value)
{
	outl(config_cmd(bus, devfn, where), 0xCF8);
	outw(value, 0xCFC + (where & 2));
}

static void pcibios_write_config_dword(
	unsigned char bus, unsigned devfn, unsigned where, unsigned int value)
{
	outl(config_cmd(bus, devfn, where), 0xCF8);
	outl(value, 0xCFC);
}

int log2(int value)
{
	/* __builtin_bsr is a exactly equivalent to the x86 machine
	 * instruction with the exception that it returns -1
	 * when the value presented to it is zero.
	 * Otherwise __builtin_bsr returns the zero based index of
	 * the highest bit set.
	 */
	return __builtin_bsr(value);
}

#define PIIX4_DEVFN 0x90
#define SMBUS_MEM_DEVICE_START 0x50
#define SMBUS_MEM_DEVICE_END 0x53
#define SMBUS_MEM_DEVICE_INC 1


#define PM_BUS 0
#define PM_DEVFN (PIIX4_DEVFN+3)

#if HAVE_CONSTANT_PROPOGATION
#define SMBUS_IO_BASE 0x1000
#define SMBHSTSTAT 0
#define SMBHSTCTL  2
#define SMBHSTCMD  3
#define SMBHSTADD  4
#define SMBHSTDAT0 5
#define SMBHSTDAT1 6
#define SMBBLKDAT  7

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
	}while((byte &1) == 1);
	while( (byte & ~1) == 0) {
		byte = inb(SMBUS_IO_BASE + SMBHSTSTAT);
	}
}

int smbus_read_byte(unsigned device, unsigned address)
{
	unsigned char host_status_register;
	unsigned char byte;
	int result;

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

	result = byte;
	if (host_status_register != 0x02) {
		result = -1;
	}
	return result;
}

#else /* !HAVE_CONSTANT_PROPOGATION */

#define SMBUS_IO_HSTSTAT   0x1000
#define SMBUS_IO_HSTCTL    0x1002
#define SMBUS_IO_HSTCMD    0x1003
#define SMBUS_IO_HSTADD    0x1004
#define SMBUS_IO_HSTDAT0   0x1005
#define SMBUS_IO_HSTDAT1   0x1006
#define SMBUS_IO_HSTBLKDAT 0x1007


static void smbus_wait_until_ready(void)
{
	while((inb(SMBUS_IO_HSTSTAT) & '\1') == '\1') {
		/* nop */
	}
}

static void smbus_wait_until_done(void)
{
	unsigned char byte;
	do {
		byte = inb(SMBUS_IO_HSTSTAT);
	}while((byte &1) == 1);
	while( (byte & ~1) == 0) {
		byte = inb(SMBUS_IO_HSTSTAT);
	}
}

short smbus_read_byte(unsigned char device, unsigned char address)
{
	unsigned char host_status_register;
	short result;

	smbus_wait_until_ready(); /* 2 */

	/* setup transaction */
	/* disable interrupts */
	outb(inb(SMBUS_IO_HSTCTL) & (~1), SMBUS_IO_HSTCTL); /* 3 */
	/* set the device I'm talking too */
	outb(((device & 0x7f) << 1) | 1, SMBUS_IO_HSTADD); /* 1 + 3 */
	/* set the command/address... */
	outb(address & 0xFF, SMBUS_IO_HSTCMD); /* 1 + 3 */
	/* set up for a byte data read */
	outb((inb(SMBUS_IO_HSTCTL) & 0xE3) | 8, SMBUS_IO_HSTCTL); /* 3 */

	/* clear any lingering errors, so the transaction will run */
	outb(inb(SMBUS_IO_HSTSTAT), SMBUS_IO_HSTSTAT); /* 3 */

	/* clear the data byte...*/
	outb(0, SMBUS_IO_HSTDAT0); /* 3 */

	/* start the command */
	outb((inb(SMBUS_IO_HSTCTL) | 0x40), SMBUS_IO_HSTCTL);

	/* poll for transaction completion */
	smbus_wait_until_done();

	host_status_register = inb(SMBUS_IO_HSTSTAT);

	/* read results of transaction */
	result = inb(SMBUS_IO_HSTDAT0);

	if (host_status_register != 0x02) {
		result = -1;
	}
	return result;
}
#endif /* HAVE_CONSTANT_PROPOGATION */


static void test(void)
{
	short byte;
	unsigned device;
	unsigned char i, j, k, l, m, n, o;
	i = 1;
	j = 2;
	k = 3;
	l = 4;
	m = 5;
	n = 6;
	o = 7;
	device = inb(SMBUS_MEM_DEVICE_START);
	byte = smbus_read_byte(device, 3);
	outb(i, 0xab);
	outb(j, 0xab);
	outb(k, 0xab);
	outb(l, 0x1234);
#if 1
	outb(m, 0xab);
#endif
#if 1
	outb(n, 0xab);
#endif
#if 0
	outb(o, 0xab);
#endif
}
