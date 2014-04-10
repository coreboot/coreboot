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

struct s5p_uart
{
	uint32_t ulcon;		// line control
	uint32_t ucon;		// control
	uint32_t ufcon;		// FIFO control
	uint32_t umcon;		// modem control
	uint32_t utrstat;	// Tx/Rx status
	uint32_t uerstat;	// Rx error status
	uint32_t ufstat;	// FIFO status
	uint32_t umstat;	// modem status
	uint32_t utxh;		// transmit buffer
	uint32_t urxh;		// receive buffer
	uint32_t ubrdiv;	// baud rate divisor
	uint32_t ufracval;	// divisor fractional value
	uint32_t uintp;		// interrupt pending
	uint32_t uints;		// interrupt source
	uint32_t uintm;		// interrupt mask
};

static struct s5p_uart *uart_regs;

void serial_putchar(unsigned int c)
{
	const uint32_t TxFifoFullBit = (0x1 << 24);

	while (readl(&uart_regs->ufstat) & TxFifoFullBit)
	{;}

	writeb(c, &uart_regs->utxh);
	if (c == '\n')
		serial_putchar('\r');
}

int serial_havechar(void)
{
	const uint32_t DataReadyMask = (0xf << 0) | (0x1 << 8);

	return (readl(&uart_regs->ufstat) & DataReadyMask) != 0;
}

int serial_getchar(void)
{
	while (!serial_havechar())
	{;}

	return readb(&uart_regs->urxh);
}

static struct console_output_driver s5p_serial_output =
{
	.putchar = &serial_putchar
};

static struct console_input_driver s5p_serial_input =
{
	.havekey = &serial_havechar,
	.getchar = &serial_getchar
};

void serial_init(void)
{
	if (!lib_sysinfo.serial || !lib_sysinfo.serial->baseaddr)
		return;

	uart_regs = (struct s5p_uart *)lib_sysinfo.serial->baseaddr;
}

void serial_console_init(void)
{
	serial_init();

	if (uart_regs) {
		console_add_output_driver(&s5p_serial_output);
		console_add_input_driver(&s5p_serial_input);
	}
}
