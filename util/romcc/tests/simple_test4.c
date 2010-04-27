#define HAVE_STRING_SUPPORT          1
#define HAVE_CAST_SUPPORT            1
#define HAVE_STATIC_ARRAY_SUPPORT    1
#define HAVE_POINTER_SUPPORT         1
#define HAVE_CONSTANT_PROPOGATION    0
#define CALCULATE_DRB_REG            1

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


/* Base Address */
#ifndef CONFIG_TTYS0_BASE
#define CONFIG_TTYS0_BASE 0x3f8
#endif

#ifndef CONFIG_TTYS0_BAUD
#define CONFIG_TTYS0_BAUD 115200
#endif

#if ((115200%CONFIG_TTYS0_BAUD) != 0)
#error Bad ttys0 baud rate
#endif

#define CONFIG_TTYS0_DIV	(115200/CONFIG_TTYS0_BAUD)

/* Line Control Settings */
#ifndef CONFIG_TTYS0_LCS
/* Set 8bit, 1 stop bit, no parity */
#define CONFIG_TTYS0_LCS	0x3
#endif

#define UART_LCS	CONFIG_TTYS0_LCS

/* Data */
#define UART_RBR 0x00
#define UART_TBR 0x00

/* Control */
#define UART_IER 0x01
#define UART_IIR 0x02
#define UART_FCR 0x02
#define UART_LCR 0x03
#define UART_MCR 0x04
#define UART_DLL 0x00
#define UART_DLM 0x01

/* Status */
#define UART_LSR 0x05
#define UART_MSR 0x06
#define UART_SCR 0x07

int uart_can_tx_byte(void)
{
	return inb(CONFIG_TTYS0_BASE + UART_LSR) & 0x20;
}

void uart_wait_to_tx_byte(void)
{
	while(!uart_can_tx_byte())
		;
}

void uart_wait_until_sent(void)
{
	while(!(inb(CONFIG_TTYS0_BASE + UART_LSR) & 0x40))
		;
}

void uart_tx_byte(unsigned char data)
{
	uart_wait_to_tx_byte();
	outb(data, CONFIG_TTYS0_BASE + UART_TBR);
	/* Make certain the data clears the fifos */
	uart_wait_until_sent();
}

void uart_init(void)
{
	/* disable interrupts */
	outb(0x0, CONFIG_TTYS0_BASE + UART_IER);
	/* enable fifo's */
	outb(0x01, CONFIG_TTYS0_BASE + UART_FCR);
	/* Set Baud Rate Divisor to 12 ==> 115200 Baud */
	outb(0x80 | UART_LCS, CONFIG_TTYS0_BASE + UART_LCR);
	outb(CONFIG_TTYS0_DIV & 0xFF,   CONFIG_TTYS0_BASE + UART_DLL);
	outb((CONFIG_TTYS0_DIV >> 8) & 0xFF,    CONFIG_TTYS0_BASE + UART_DLM);
	outb(UART_LCS, CONFIG_TTYS0_BASE + UART_LCR);
}

void __console_tx_char(unsigned char byte)
{
	uart_tx_byte(byte);
}
void __console_tx_nibble(unsigned nibble)
{
	unsigned char digit;
	digit = nibble + '0';
	if (digit > '9') {
		digit += 39;
	}
	__console_tx_char(digit);
}
void __console_tx_hex8(unsigned char byte)
{
	__console_tx_nibble(byte >> 4);
	__console_tx_nibble(byte & 0x0f);
}

void __console_tx_hex32(unsigned char value)
{
	__console_tx_nibble((value >> 28) & 0x0f);
	__console_tx_nibble((value >> 24) & 0x0f);
	__console_tx_nibble((value >> 20) & 0x0f);
	__console_tx_nibble((value >> 16) & 0x0f);
	__console_tx_nibble((value >> 12) & 0x0f);
	__console_tx_nibble((value >>  8) & 0x0f);
	__console_tx_nibble((value >>  4) & 0x0f);
	__console_tx_nibble(value & 0x0f);
}

#if HAVE_STRING_SUPPORT
void __console_tx_string(char *str)
{
	unsigned char ch;
	while((ch = *str++) != '\0') {
		__console_tx_char(ch);
	}
}
#else
void __console_tx_string(char *str)
{
}
#endif


void print_emerg_char(unsigned char byte) { __console_tx_char(byte); }
void print_emerg_hex8(unsigned char value) { __console_tx_hex8(value); }
void print_emerg_hex32(unsigned int value) { __console_tx_hex32(value); }
void print_emerg(char *str) { __console_tx_string(str); }

void print_alert_char(unsigned char byte) { __console_tx_char(byte); }
void print_alert_hex8(unsigned char value) { __console_tx_hex8(value); }
void print_alert_hex32(unsigned int value) { __console_tx_hex32(value); }
void print_alert(char *str) { __console_tx_string(str); }

void print_crit_char(unsigned char byte) { __console_tx_char(byte); }
void print_crit_hex8(unsigned char value) { __console_tx_hex8(value); }
void print_crit_hex32(unsigned int value) { __console_tx_hex32(value); }
void print_crit(char *str) { __console_tx_string(str); }

void print_err_char(unsigned char byte) { __console_tx_char(byte); }
void print_err_hex8(unsigned char value) { __console_tx_hex8(value); }
void print_err_hex32(unsigned int value) { __console_tx_hex32(value); }
void print_err(char *str) { __console_tx_string(str); }

void print_warning_char(unsigned char byte) { __console_tx_char(byte); }
void print_warning_hex8(unsigned char value) { __console_tx_hex8(value); }
void print_warning_hex32(unsigned int value) { __console_tx_hex32(value); }
void print_warning(char *str) { __console_tx_string(str); }

void print_notice_char(unsigned char byte) { __console_tx_char(byte); }
void print_notice_hex8(unsigned char value) { __console_tx_hex8(value); }
void print_notice_hex32(unsigned int value) { __console_tx_hex32(value); }
void print_notice(char *str) { __console_tx_string(str); }

void print_info_char(unsigned char byte) { __console_tx_char(byte); }
void print_info_hex8(unsigned char value) { __console_tx_hex8(value); }
void print_info_hex32(unsigned int value) { __console_tx_hex32(value); }
void print_info(char *str) { __console_tx_string(str); }

void print_debug_char(unsigned char byte) { __console_tx_char(byte); }
void print_debug_hex8(unsigned char value) { __console_tx_hex8(value); }
void print_debug_hex32(unsigned int value) { __console_tx_hex32(value); }
void print_debug(char *str) { __console_tx_string(str); }

void print_spew_char(unsigned char byte) { __console_tx_char(byte); }
void print_spew_hex8(unsigned char value) { __console_tx_hex8(value); }
void print_spew_hex32(unsigned int value) { __console_tx_hex32(value); }
void print_spew(char *str) { __console_tx_string(str); }

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
	while((inb(SMBUS_IO_HSTSTAT) & 1) == 1) {
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

	smbus_wait_until_ready();

	/* setup transaction */
	/* disable interrupts */
	outb(inb(SMBUS_IO_HSTCTL) & (~1), SMBUS_IO_HSTCTL);
	/* set the device I'm talking too */
	outb(((device & 0x7f) << 1) | 1, SMBUS_IO_HSTADD);
	/* set the command/address... */
	outb(address & 0xFF, SMBUS_IO_HSTCMD);
	/* set up for a byte data read */
	outb((inb(SMBUS_IO_HSTCTL) & 0xE3) | 8, SMBUS_IO_HSTCTL);

	/* clear any lingering errors, so the transaction will run */
	outb(inb(SMBUS_IO_HSTSTAT), SMBUS_IO_HSTSTAT);

	/* clear the data byte...*/
	outb(0, SMBUS_IO_HSTDAT0);

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

#define I440GX_BUS 0
#define I440GX_DEVFN ((0x00 << 3) + 0)


static void spd_set_drb(void)
{
	/*
	 * Effects:	Uses serial presence detect to set the
	 *              DRB registers which holds the ending memory address assigned
	 *              to each DIMM.
	 */
	unsigned end_of_memory;
	unsigned char device;
	unsigned char drb_reg;

	end_of_memory = 0; /* in multiples of 8MiB */
	device = SMBUS_MEM_DEVICE_START;
#if !CALCULATE_DRB_REG
	drb_reg = 0x60;
#endif
	while (device <= SMBUS_MEM_DEVICE_END) {
		unsigned side1_bits, side2_bits;
		int byte, byte2;

		side1_bits = side2_bits = -1;

		/* rows */
		byte = smbus_read_byte(device, 3);
		if (byte >= 0) {
			side1_bits += byte & 0xf;

			/* columns */
			byte = smbus_read_byte(device, 4);
			side1_bits += byte & 0xf;

			/* banks */
			byte = smbus_read_byte(device, 17);
			side1_bits += log2(byte);

			/* Get the module data width and convert it to a power of two */
			/* low byte */
			byte = smbus_read_byte(device, 6);

			/* high byte */
			byte2 = smbus_read_byte(device, 7);
#if HAVE_CAST_SUPPORT
			side1_bits += log2((((unsigned long)byte2 << 8)| byte));
#else
			side1_bits += log2((((byte2 << 8) | byte));
#endif

			/* now I have the ram size in bits as a power of two (less 1) */
			/* Make it mulitples of 8MB */
			side1_bits -= 25;

			/* side two */

			/* number of physical banks */
			byte = smbus_read_byte(device, 5);
			if (byte > 1) {
				/* for now only handle the symmetrical case */
				side2_bits = side1_bits;
			}
		}

		/* Compute the end address for the DRB register */
		/* Only process dimms < 2GB (2^8 * 8MB) */
		if (side1_bits < 8) {
			end_of_memory += (1 << side1_bits);
		}
#if CALCULATE_DRB_REG
		drb_reg = ((device - SMBUS_MEM_DEVICE_START) << 1) + 0x60;
#endif

#if HAVE_STRING_SUPPORT
		print_debug("end_of_memory: "); print_debug_hex32(end_of_memory); print_debug("\n");
#endif
		pcibios_write_config_byte(I440GX_BUS, I440GX_DEVFN, drb_reg, end_of_memory);

		if (side2_bits < 8 ) {
			end_of_memory += (1 << side2_bits);
		}
#if HAVE_STRING_SUPPORT
		print_debug("end_of_memory: "); print_debug_hex32(end_of_memory); print_debug("\n");
#endif
		pcibios_write_config_byte(I440GX_BUS, I440GX_DEVFN, drb_reg +1, end_of_memory);

#if !CALCULATE_DRB_REG
		drb_reg += 2;
#endif
		device += SMBUS_MEM_DEVICE_INC;
	}
}
