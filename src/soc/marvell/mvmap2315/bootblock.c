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
#include <soc/a2bus.h>
#include <soc/addressmap.h>
#include <soc/apmu.h>
#include <soc/bdb.h>
#include <soc/gic.h>
#include <soc/load_validate.h>
#include <soc/mcu.h>
#include <soc/pmic.h>
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
	u32 boot_path;

	write32((void *)MVMAP2315_BOOTBLOCK_CB1, 0);
	write32((void *)MVMAP2315_BOOTBLOCK_CB2, 0);

	set_bdb_pointers((u8 *)MVMAP2315_BDB_LCM_BASE, &bdb_info);

	printk(BIOS_DEBUG, "loading and validating APMU firmware.\n");
	load_and_validate(&bdb_info, APMU_FIRMWARE);

	apmu_start();

	if (!(read32((void *)MVMAP2315_LOWPWR_REG) & MVMAP2315_LOWPWR_FLAG)) {
		printk(BIOS_DEBUG, "loading and validating MCU firmware.\n");
		load_and_validate(&bdb_info, MCU_FIRMWARE);
		mcu_start();
		boot_path = get_boot_path();
	} else {
		printk(BIOS_DEBUG, "Low power restart. Skip MCU code load.\n");

		sned_hash_msg(GET_HASH);
		receive_hash_msg_respond();

		printk(BIOS_DEBUG,
		       "MCU hash validation not currently implemented\n");

		boot_path = get_boot_path();
	}

	switch (boot_path) {
	case NO_BOOT:
		no_boot();
		break;
	case CHARGING_SCREEN:
		charging_screen();
		break;
	case FULL_BOOT:
		full_boot();
		break;
	}

	printk(BIOS_DEBUG, "initializing the A2 bus.\n");
	init_a2bus_config();

	printk(BIOS_DEBUG, "Powering up the AP core0.\n");
	ap_start((void *)MVMAP2315_ROMSTAGE_BASE);

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
