/*
 * This file is part of the coreboot project.
 *
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

#include <bootblock_common.h>
#include <intelblocks/gspi.h>
#include <intelblocks/uart.h>
#include <soc/bootblock.h>
#include <soc/iomap.h>
#include <soc/pch.h>

#if CONFIG(FSP_CAR)
#include <FsptUpd.h>

const FSPT_UPD temp_ram_init_params = {
	.FspUpdHeader = {
		.Signature = 0x545F4450554C4643ULL,	/* 'CFLUPD_T' */
		.Revision = 1,
		.Reserved = {0},
	},
	.FsptCoreUpd = {
		/*
		 * It is a requirement for firmware to have Firmware Interface Table
		 * (FIT), which contains pointers to each microcode update.
		 * The microcode update is loaded for all logical processors before
		 * cpu reset vector.
		 *
		 * All SoC since Gen-4 has above mechanism in place to load microcode
		 * even before hitting CPU reset vector. Hence skipping FSP-T loading
		 * microcode after CPU reset by passing '0' value to
		 * FSPT_UPD.MicrocodeRegionBase and FSPT_UPD.MicrocodeRegionSize.
		 */
		.MicrocodeRegionBase = 0,
		.MicrocodeRegionSize = 0,
		.CodeRegionBase =
			(uint32_t)(0x100000000ULL - CONFIG_ROM_SIZE),
		.CodeRegionSize = (uint32_t)CONFIG_ROM_SIZE,
	},
};
#endif

asmlinkage void bootblock_c_entry(uint64_t base_timestamp)
{
	/* Call lib/bootblock.c main */
	bootblock_main_with_basetime(base_timestamp);
}

void bootblock_soc_early_init(void)
{
	bootblock_systemagent_early_init();
	bootblock_pch_early_init();
	bootblock_cpu_init();
	pch_early_iorange_init();
	if (CONFIG(INTEL_LPSS_UART_FOR_CONSOLE))
		uart_bootblock_init();
}

void bootblock_soc_init(void)
{
	/*
	 * Clear the GPI interrupt status and enable registers. These
	 * registers do not get reset to default state when booting from S5.
	 */
	gpi_clear_int_cfg();
	report_platform_info();
	bootblock_pch_init();
}
