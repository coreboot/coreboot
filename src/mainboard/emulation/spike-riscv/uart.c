/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <types.h>
#include <console/uart.h>
#include <arch/io.h>
#include <boot/coreboot_tables.h>
#include <spike_util.h>

static uint8_t *buf = (void *)0x3f8;
uintptr_t uart_platform_base(int idx)
{
	return (uintptr_t) buf;
}

void uart_init(int idx)
{
}

unsigned char uart_rx_byte(int idx)
{
	return *buf; // this does not work on spike, requires more implementation details
}

void uart_tx_byte(int idx, unsigned char data)
{
	mcall_console_putchar(data);
}

void uart_tx_flush(int idx)
{
}

#ifndef __PRE_RAM__
void uart_fill_lb(void *data)
{
	struct lb_serial serial;
	serial.type = LB_SERIAL_TYPE_MEMORY_MAPPED;
	serial.baseaddr = 0x3f8;
	serial.baud = 115200;
	serial.regwidth = 1;
	lb_add_serial(&serial, data);
        lb_add_console(LB_TAG_CONSOLE_SERIAL8250MEM, data);
}
#endif
