void land_test(void)
{
	int i;
	i = 1 && 2;
}
void lor_test(void)
{
	int i;
	i = 1 || 2;
}

void outb(unsigned char value, unsigned short port)
{
	__builtin_outb(value, port);
}

unsigned char inb(unsigned short port)
{
	return __builtin_inb(port);
}

static unsigned int config_cmd2(unsigned char bus, unsigned devfn, unsigned where)
{
	return 0x80000000 | (bus << 16) | (devfn << 8) | (where & ~3)  ;
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

void dummy(void)
{
	uart_tx_byte(5);
}

#define PIIX4_DEVFN 0x90
#define SMBUS_MEM_DEVICE_START 0x50
#define SMBUS_MEM_DEVICE_END 0x53
#define SMBUS_MEM_DEVICE_INC 1


#define PM_BUS 0
#define PM_DEVFN (PIIX4_DEVFN+3)

#define SMBUS_IO_BASE 0x1000
#define SMBHSTSTAT 0
#define SMBHSTCTL  2
#define SMBHSTCMD  3
#define SMBHSTADD  4
#define SMBHSTDAT0 5
#define SMBHSTDAT1 6
#define SMBBLKDAT  7

static void smbus_wait_until_done(void)
{
	unsigned char byte;
	do {
		byte = inb(SMBUS_IO_BASE + SMBHSTSTAT);
	}while((byte &1) == 1);
#if 1
	while( (byte & ~1) == 0) {
		byte = inb(SMBUS_IO_BASE + SMBHSTSTAT);
	}
#endif
}

#if 0
void ifthenelse(void)
{
	int i;
	if (5 > 2) {
		i = 1;
	}
	else {
		i = 2;
	}
	i = i + 3;
}
#endif
#if 0
static int add(int left, int right)
{
	{
		return left + right;
	}
}
#else
#if 0
static int add(int left, int right)
{
	return left + right;
}
#endif
#endif

#if 0
static void assign(void)
{
	int i, j;
	i = j = 1;
}
#endif

#if 0
static void and(void)
{
	int i, j, k;
	i = 1;
	j = 2;
	k = i && j;

}
static void and_test(void)
{
	and();
}
#endif
#if 0
#define INC_TEST 2
static void inc(void)
{
	int i;
	i = 5;
#if (INC_TEST == 1)
	i += 7;
#endif
#if (INC_TEST == 2)
	++i;
#endif
#if (INC_TEST == 3)
	i++;
#endif
}

#if 0
static void inc_test(void)
{
	inc();
}
#endif
#endif
#if 0
static void loop(void)
{
	int i;
	for(i = 0; i < 10; i++) {
		;
	} while(i < 10);
}

static void loop_test(void)
{
	loop();
}
#endif

#if 0
static void simple(void)
{
	add(1,2);
}
#endif

#if 0
static void fun(void)
{
	int bar;
	bar = add(1, 2);
}
#endif


#if 0
static void func(void)
{
	int bar, baz;
 	int i;

	baz = add(1, 2);
	baz = add(1, 2);
	bar = 1;
	baz = 2;
	for(i = 0; i < 10; i = i + 1) {
		baz = i;
	}
	bar = 1 + 2 * 3;
	bar = add(3, 4);
	bar = add(bar, baz);
}
#endif
