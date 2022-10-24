/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <console/uart.h>
#include <boot/coreboot_tables.h>

static uint8_t *buf = (void *)0;
uintptr_t uart_platform_base(unsigned int idx)
{
	return (uintptr_t) buf;
}

void uart_init(unsigned int idx)
{
}

unsigned char uart_rx_byte(unsigned int idx)
{
	return 0;
}

void uart_tx_byte(unsigned int idx, unsigned char data)
{

}

void uart_tx_flush(unsigned int idx)
{
}

enum cb_err fill_lb_serial(struct lb_serial *serial)
{
	serial->type = LB_SERIAL_TYPE_MEMORY_MAPPED;
	serial->baseaddr = 0;
	serial->baud = 115200;
	serial->regwidth = 1;
	serial->input_hertz = uart_platform_refclk();

	return CB_SUCCESS;
}
