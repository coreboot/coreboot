/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Marvell, Inc.
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

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <bootblock_common.h>
#include <console/console.h>
#include <timestamp.h>
#include <console/uart.h>
#include <soc/gic.h>
#include <soc/uart.h>

void bootblock_soc_early_init(void)
{
	timestamp_add_now(TS_START_BOOTBLOCK);

	asm volatile ("bl fiq_stack_init" : : : "r0", "r1");

	enable_bcm_gic();

	uart_num = 0;
}

void bootblock_soc_init(void)
{
	/* initializing UART1 to free UART0 to be used by romstage */
	uart_num = 1;
	uart_init(uart_num);

	while (1)
		;
}
