/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 * Copyright (C) 2008 Ulf Jordan <jordan@chalmers.se>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <libpayload-config.h>
#include <libpayload.h>

#define IOBASE lib_sysinfo.ser_ioport
#define MEMBASE (phys_to_virt(lib_sysinfo.ser_base))
#define DIVISOR(x) (115200 / x)

#ifdef CONFIG_SERIAL_SET_SPEED
static void serial_io_hardware_init(int port, int speed, int word_bits, int parity, int stop_bits)
{
	unsigned char reg;

	/* We will assume 8n1 for now. Does anyone use anything else these days? */

	/* Disable interrupts. */
	outb(0, port + 0x01);

	/* Assert RTS and DTR. */
	outb(3, port + 0x04);

	/* Set the divisor latch. */
	reg = inb(port + 0x03);
	outb(reg | 0x80, port + 0x03);

	/* Write the divisor. */
	outb(DIVISOR(speed) & 0xFF, port);
	outb(DIVISOR(speed) >> 8 & 0xFF, port + 1);

	/* Restore the previous value of the divisor. */
	outb(reg & ~0x80, port + 0x03);
}

static void serial_mem_hardware_init(int port, int speed, int word_bits, int parity, int stop_bits)
{
	unsigned char reg;

	/* We will assume 8n1 for now. Does anyone use anything else these days? */

	/* Disable interrupts. */
	writeb(0, MEMBASE + 0x01);

	/* Assert RTS and DTR. */
	writeb(3, MEMBASE + 0x04);

	/* Set the divisor latch. */
	reg = readb(MEMBASE + 0x03);
	writeb(reg | 0x80, MEMBASE + 0x03);

	/* Write the divisor. */
	writeb(DIVISOR(speed) & 0xFF, MEMBASE);
	writeb(DIVISOR(speed) >> 8 & 0xFF, MEMBASE + 1);

	/* Restore the previous value of the divisor. */
	writeb(reg & ~0x80, MEMBASE + 0x03);
}
#endif

static struct console_input_driver consin = {
	.havekey = serial_havechar,
	.getchar = serial_getchar
};

static struct console_output_driver consout = {
	.putchar = serial_putchar
};

void serial_init(void)
{
	pcidev_t oxpcie_dev;
	if (pci_find_device(0x1415, 0xc158, &oxpcie_dev)) {
		lib_sysinfo.ser_base = pci_read_resource(oxpcie_dev, 0) + 0x1000;
	} else {
		lib_sysinfo.ser_base = 0;
	}

#ifdef CONFIG_SERIAL_SET_SPEED
	if (lib_sysinfo.ser_base)
		serial_mem_hardware_init(IOBASE, CONFIG_SERIAL_BAUD_RATE, 8, 0, 1);
	else
		serial_io_hardware_init(IOBASE, CONFIG_SERIAL_BAUD_RATE, 8, 0, 1);
#endif
	console_add_input_driver(&consin);
	console_add_output_driver(&consout);
}

static void serial_io_putchar(unsigned int c)
{
	c &= 0xff;
	while ((inb(IOBASE + 0x05) & 0x20) == 0) ;
	outb(c, IOBASE);
}

static int serial_io_havechar(void)
{
	return inb(IOBASE + 0x05) & 0x01;
}

static int serial_io_getchar(void)
{
	while (!serial_io_havechar()) ;
	return (int)inb(IOBASE);
}

static void serial_mem_putchar(unsigned int c)
{
	c &= 0xff;
	while ((readb(MEMBASE + 0x05) & 0x20) == 0) ;
	writeb(c, MEMBASE);
}

static int serial_mem_havechar(void)
{
	return readb(MEMBASE + 0x05) & 0x01;
}

static int serial_mem_getchar(void)
{
	while (!serial_mem_havechar()) ;
	return (int)readb(MEMBASE);
}


void serial_putchar(unsigned int c)
{
	if (lib_sysinfo.ser_base)
		serial_mem_putchar(c);
	else
		serial_io_putchar(c);
}

int serial_havechar(void)
{
	if (lib_sysinfo.ser_base)
		return serial_mem_havechar();
	else
		return serial_io_havechar();
}

int serial_getchar(void)
{
	if (lib_sysinfo.ser_base)
		return serial_mem_getchar();
	else
		return serial_io_getchar();
}

/*  These are thinly veiled vt100 functions used by curses */

#define VT100_CLEAR       "\e[H\e[J"
/* These defines will fail if you use bold and reverse at the same time.
 * Switching off one of them will switch off both. tinycurses knows about
 * this and does the right thing.
 */
#define VT100_SBOLD       "\e[1m"
#define VT100_EBOLD       "\e[m"
#define VT100_SREVERSE    "\e[7m"
#define VT100_EREVERSE    "\e[m"
#define VT100_CURSOR_ADDR "\e[%d;%dH"
#define VT100_CURSOR_ON   "\e[?25l"
#define VT100_CURSOR_OFF  "\e[?25h"
/* The following smacs/rmacs are actually for xterm; a real vt100 has
   enacs=\E(B\E)0, smacs=^N, rmacs=^O.  */
#define VT100_SMACS       "\e(0"
#define VT100_RMACS       "\e(B"
/* A vt100 doesn't do color, setaf/setab below are from xterm-color. */
#define VT100_SET_COLOR   "\e[3%d;4%dm"

static void serial_putcmd(char *str)
{
	while(*str)
		serial_putchar(*(str++));
}

void serial_clear(void)
{
	serial_putcmd(VT100_CLEAR);
}

void serial_start_bold(void)
{
	serial_putcmd(VT100_SBOLD);
}

void serial_end_bold(void)
{
	serial_putcmd(VT100_EBOLD);
}

void serial_start_reverse(void)
{
	serial_putcmd(VT100_SREVERSE);
}

void serial_end_reverse(void)
{
	serial_putcmd(VT100_EREVERSE);
}

void serial_start_altcharset(void)
{
	serial_putcmd(VT100_SMACS);
}

void serial_end_altcharset(void)
{
	serial_putcmd(VT100_RMACS);
}

/**
 * Set the foreground and background colors on the serial console.
 *
 * @param fg Foreground color number.
 * @param bg Background color number.
 */
void serial_set_color(short fg, short bg)
{
	char buffer[32];
	snprintf(buffer, sizeof(buffer), VT100_SET_COLOR, fg, bg);
	serial_putcmd(buffer);
}

void serial_set_cursor(int y, int x)
{
	char buffer[32];
	snprintf(buffer, sizeof(buffer), VT100_CURSOR_ADDR, y + 1, x + 1);
	serial_putcmd(buffer);
}

void serial_cursor_enable(int state)
{
	if (state)
		serial_putcmd(VT100_CURSOR_ON);
	else
		serial_putcmd(VT100_CURSOR_OFF);
}
