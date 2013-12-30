/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Eric Biederman
 * Copyright (C) 2006-2010 coresystems GmbH
 * Subject to the GNU GPL version 2.
 */

#include <uart8250.h>
#if CONFIG_USE_OPTION_TABLE
#include <pc80/mc146818rtc.h>
#include "option_table.h"
#endif

u32 uart_mem_init(void)
{
	unsigned uart_baud = CONFIG_TTYS0_BAUD;
	u32 uart_bar = 0;
	unsigned div;

	/* find out the correct baud rate */
#if !defined(__SMM__) && CONFIG_USE_OPTION_TABLE
	static const unsigned baud[8] = { 115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200 };
	unsigned b_index = 0;
#if defined(__PRE_RAM__) && CONFIG_USE_OPTION_TABLE
	b_index = read_option(baud_rate, 0);
	b_index &= 7;
	uart_baud = baud[b_index];
#else
	if (get_option(&b_index, "baud_rate") == CB_SUCCESS)
		uart_baud = baud[b_index];
#endif
#endif

	/* Now find the UART base address and calculate the divisor */

#if defined(MORE_TESTING) && !defined(__SIMPLE_DEVICE__)
	device_t dev = dev_find_device(0x1415, 0xc158, NULL);
	if (!dev)
		dev = dev_find_device(0x1415, 0xc11b, NULL);

	if (dev) {
		struct resource *res = find_resource(dev, 0x10);

		if (res) {
			uart_bar = res->base + 0x1000; // for 1st UART
			// uart_bar = res->base + 0x2000; // for 2nd UART
		}
	}

	if (!uart_bar)
#endif
	uart_bar = CONFIG_OXFORD_OXPCIE_BASE_ADDRESS + 0x1000; // 1st UART
	// uart_bar = CONFIG_OXFORD_OXPCIE_BASE_ADDRESS + 0x2000; // 2nd UART

	div = 4000000 / uart_baud;

	if (uart_bar)
		uart8250_mem_init(uart_bar, div);

	return uart_bar;
}
