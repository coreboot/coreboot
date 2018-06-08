/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 - 2017 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/cpu.h>
#include <bootblock_common.h>
#include <cpu/x86/mtrr.h>
#include <device/pci.h>
#include <FsptUpd.h>
#include <intelblocks/fast_spi.h>
#include <lib.h>
#include <soc/bootblock.h>
#include <soc/iomap.h>
#include <spi-generic.h>
#include <timestamp.h>
#include <console/console.h>

const FSPT_UPD temp_ram_init_params = {
	.FspUpdHeader = {
			.Signature = 0x545F445055564E44ULL,
			.Revision = 1,
			.Reserved = {0},
	},
	.FsptCoreUpd = {
			.MicrocodeRegionBase =
				(UINT32)CONFIG_CPU_MICROCODE_CBFS_LOC,
			.MicrocodeRegionLength =
				(UINT32)CONFIG_CPU_MICROCODE_CBFS_LEN,
			.CodeRegionBase =
				(UINT32)(0x100000000ULL - CONFIG_CBFS_SIZE),
			.CodeRegionLength = (UINT32)CONFIG_CBFS_SIZE,
			.Reserved1 = {0},
	},
	.FsptConfig = {
			.PcdFsptPort80RouteDisable = 0,
			.ReservedTempRamInitUpd = {0},
	},
	.UnusedUpdSpace0 = {0},
	.UpdTerminator = 0x55AA,
};

asmlinkage void bootblock_c_entry(uint64_t base_timestamp)
{
	/* Call lib/bootblock.c main */
	bootblock_main_with_timestamp(base_timestamp, NULL, 0);
};

void bootblock_soc_early_init(void)
{

#if (IS_ENABLED(CONFIG_CONSOLE_SERIAL))
	early_uart_init();
#endif
};

void bootblock_soc_init(void)
{
	if (IS_ENABLED(CONFIG_BOOTBLOCK_CONSOLE))
		printk(BIOS_DEBUG, "FSP TempRamInit successful...\n");
};
