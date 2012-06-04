/*
 * SerialICE 
 *
 * Copyright (C) 2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

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

/* SIO functions */

static void sio_init(void)
{
#if SIO_SPEED > 115200
	/* "high speed" serial requires special chip setup
	 * (to be done in superio_init), and special divisor
	 * values (implement superio_serial_divisor() for that).
	 * Maybe it requires even more, but so far that seems
	 * to be enough.
	 */
	int divisor = superio_serial_divisor(SIO_SPEED);
#else
	int divisor = 115200 / SIO_SPEED;
#endif
	int lcs = 3;
	outb(0x00, SIO_PORT + UART_IER);
	outb(0x01, SIO_PORT + UART_FCR);
	outb(0x03, SIO_PORT + UART_MCR);
	outb(0x80 | lcs, SIO_PORT + UART_LCR);
	outb(divisor & 0xff, SIO_PORT + UART_DLL);
	outb((divisor >> 8) & 0xff, SIO_PORT + UART_DLM);
	outb(lcs, SIO_PORT + UART_LCR);
}

static void sio_putc(u8 data)
{
	while (!(inb(SIO_PORT + UART_LSR) & 0x20)) ;
	outb(data, SIO_PORT + UART_TBR);
	while (!(inb(SIO_PORT + UART_LSR) & 0x40)) ;
}

static u8 sio_getc(void)
{
	u8 val;
	while (!(inb(SIO_PORT + UART_LSR) & 0x01)) ;

	val = inb(SIO_PORT + UART_RBR);

#if ECHO_MODE
	sio_putc(val);
#endif
	return val;
}

/* SIO helpers */

static void sio_putstring(const char *string)
{
	/* Very simple, no %d, %x etc. */
	while (*string) {
		if (*string == '\n')
			sio_putc('\r');
		sio_putc(*string);
		string++;
	}
}

#define sio_put_nibble(nibble)	\
	if (nibble > 9)		\
		nibble += ('a' - 10);	\
	else			\
		nibble += '0';	\
	sio_putc(nibble)

static void sio_put8(u8 data)
{
	u8 c;
		
	c = (data >> 4) & 0xf;
	sio_put_nibble(c);

	c = data & 0xf;
	sio_put_nibble(c);
}

static void sio_put16(u16 data)
{
	int i;
	for (i=12; i >= 0; i -= 4) {
		u8 c = (data >> i) & 0xf;
		sio_put_nibble(c);
	}
}

static void sio_put32(u32 data)
{
	int i;
	for (i=28; i >= 0; i -= 4) {
		u8 c = (data >> i) & 0xf;
		sio_put_nibble(c);
	}
}

static u8 sio_get_nibble(void)
{
	u8 ret = 0;
	u8 nibble = sio_getc();

	if (nibble >= '0' && nibble <= '9') {
		ret = (nibble - '0');
	} else if (nibble >= 'a' && nibble <= 'f') {
		ret = (nibble - 'a') + 0xa;
	} else if (nibble >= 'A' && nibble <= 'F') {
		ret = (nibble - 'A') + 0xa;
	} else {
		sio_putstring("ERROR: parsing number\n");
	}
	return ret;
}

static u8 sio_get8(void)
{
	u8 data;
	data = sio_get_nibble();
	data = data << 4;
	data |= sio_get_nibble();
	return data;
}

static u16 sio_get16(void)
{
	u16 data;

	data = sio_get_nibble();
	data = data << 4;
	data |= sio_get_nibble();
	data = data << 4;
	data |= sio_get_nibble();
	data = data << 4;
	data |= sio_get_nibble();

	return data;
}

static u32 sio_get32(void)
{
	u32 data;

	data = sio_get_nibble();
	data = data << 4;
	data |= sio_get_nibble();
	data = data << 4;
	data |= sio_get_nibble();
	data = data << 4;
	data |= sio_get_nibble();
	data = data << 4;
	data |= sio_get_nibble();
	data = data << 4;
	data |= sio_get_nibble();
	data = data << 4;
	data |= sio_get_nibble();
	data = data << 4;
	data |= sio_get_nibble();

	return data;
}


