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

#include <arch/io.h>
#include <bootblock_common.h>
#include <console/console.h>
#include <timestamp.h>
#include <console/uart.h>
#include <soc/addressmap.h>
#include <soc/bdb.h>
#include <soc/gic.h>
#include <soc/load_validate.h>
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
	struct bdb_pointer bdb_info;

	write32((void *)MVMAP2315_BOOTBLOCK_CB1, 0);
	write32((void *)MVMAP2315_BOOTBLOCK_CB2, 0);

	set_bdb_pointers((u8 *)MVMAP2315_BDB_LCM_BASE, &bdb_info);

	printk(BIOS_DEBUG, "loading and validating APMU firmware.\n");
	load_and_validate(&bdb_info, APMU_FIRMWARE);

	printk(BIOS_DEBUG, "loading and validating MCU firmware.\n");
	load_and_validate(&bdb_info, MCU_FIRMWARE);

	/* initializing UART1 to free UART0 to be used by romstage */
	uart_num = 1;
	uart_init(uart_num);

	while (read32((void *)MVMAP2315_BOOTBLOCK_CB1) != 0x4)
		;

	printk(BIOS_DEBUG, "loading and validating AP_RW firmware.\n");
	load_and_validate(&bdb_info, AP_RW_FIRMWARE);

	write32((void *)MVMAP2315_BOOTBLOCK_CB2, 0x4)
		;

	while (1)
		;
}
