/*
 * Copyright 2013 Google Inc.
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

#include <libpayload.h>
#include <stdint.h>

struct mmio32_uart {
	union {
		uint32_t thr; // Transmit holding register.
		uint32_t rbr; // Receive buffer register.
		uint32_t dll; // Divisor latch lsb.
	};
	union {
		uint32_t ier; // Interrupt enable register.
		uint32_t dlm; // Divisor latch msb.
	};
	union {
		uint32_t iir; // Interrupt identification register.
		uint32_t fcr; // FIFO control register.
	};
	uint32_t lcr; // Line control register.
	uint32_t mcr; // Modem control register.
	uint32_t lsr; // Line status register.
	uint32_t msr; // Modem status register.
} __attribute__ ((packed));

enum {
	LSR_DR = 0x1 << 0, // Data ready.
	LSR_OE = 0x1 << 1, // Overrun.
	LSR_PE = 0x1 << 2, // Parity error.
	LSR_FE = 0x1 << 3, // Framing error.
	LSR_BI = 0x1 << 4, // Break.
	LSR_THRE = 0x1 << 5, // Xmit holding register empty.
	LSR_TEMT = 0x1 << 6, // Xmitter empty.
	LSR_ERR = 0x1 << 7 // Error.
};

static struct mmio32_uart *uart = NULL;

void serial_putchar(unsigned int c)
{
	while (!(readl(&uart->lsr) & LSR_THRE))
		/* wait for transmit register to clear */;

	writel((char)c, &uart->thr);
	if (c == '\n')
		serial_putchar('\r');
}

int serial_havechar(void)
{
	uint8_t lsr = readl(&uart->lsr);
	return (lsr & LSR_DR) == LSR_DR;
}

int serial_getchar(void)
{
	while (!serial_havechar())
		/* wait for character */;

	return readl(&uart->rbr);
}

static struct console_output_driver mmio32_serial_output = {
	.putchar = &serial_putchar
};

static struct console_input_driver mmio32_serial_input = {
	.havekey = &serial_havechar,
	.getchar = &serial_getchar
};

void serial_init(void)
{
	if (!lib_sysinfo.serial || !lib_sysinfo.serial->baseaddr)
		return;

	uart = (struct mmio32_uart *)(uintptr_t)lib_sysinfo.serial->baseaddr;
}

void serial_console_init(void)
{
	serial_init();

	if (uart) {
		console_add_output_driver(&mmio32_serial_output);
		console_add_input_driver(&mmio32_serial_input);
	}
}
