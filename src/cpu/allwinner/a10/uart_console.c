/*
 * Glue to UART code to enable serial console
 *
 * Copyright 2013 Google Inc.
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include <types.h>
#include <console/uart.h>
#include <boot/coreboot_tables.h>

#include "memmap.h"

uintptr_t uart_platform_base(int idx)
{
	/* UART blocks are mapped 0x400 bytes apart */
	if (idx < 8)
		return A1X_UART0_BASE + 0x400 * idx;
	else
		return 0;
}

/* FIXME: We assume clock is 24MHz, which may not be the case. */
unsigned int uart_platform_refclk(void)
{
	return 24000000;
}

#ifndef __PRE_RAM__
void uart_fill_lb(void *data)
{
	struct lb_serial serial;
	serial.type = LB_SERIAL_TYPE_MEMORY_MAPPED;
	serial.baseaddr = uart_platform_base(CONFIG_UART_FOR_CONSOLE);
	serial.baud = default_baudrate();
	serial.regwidth = 1;
	lb_add_serial(&serial, data);

	lb_add_console(LB_TAG_CONSOLE_SERIAL8250MEM, data);
}
#endif
