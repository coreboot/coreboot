/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <boot/coreboot_tables.h>
#include <console/uart.h>
#include <drivers/uart/pl011.h>

void uart_init(unsigned int idx)
{
}

void uart_tx_byte(unsigned int idx, unsigned char data)
{
	struct pl011_uart *regs = uart_platform_baseptr(idx);

	write8(&regs->dr, data);
	uart_tx_flush(idx);
}

void uart_tx_flush(unsigned int idx)
{
	struct pl011_uart *regs = uart_platform_baseptr(idx);

	/* FIXME: add a timeout */
	while (!(read32(&regs->fr) & PL011_UARTFR_TXFE))
		;
}

unsigned char uart_rx_byte(unsigned int idx)
{
	struct pl011_uart *regs = uart_platform_baseptr(idx);

	while (read32(&regs->fr) & PL011_UARTFR_RXFE)
		;
	return read8(&regs->dr);
}

enum cb_err fill_lb_serial(struct lb_serial *serial)
{
	serial->type = LB_SERIAL_TYPE_MEMORY_MAPPED;
	serial->baseaddr = uart_platform_base(CONFIG_UART_FOR_CONSOLE);
	serial->baud = get_uart_baudrate();
	serial->regwidth = 1;
	serial->input_hertz = uart_platform_refclk();

	return CB_SUCCESS;
}
