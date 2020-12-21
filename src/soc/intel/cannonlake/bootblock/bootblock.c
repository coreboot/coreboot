/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <cpu/x86/mtrr.h>
#include <intelblocks/fast_spi.h>
#include <intelblocks/gpio.h>
#include <intelblocks/gspi.h>
#include <intelblocks/systemagent.h>
#include <intelblocks/tco.h>
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
		 *
		 * Note: CodeRegionSize must be smaller than or equal to 16MiB to not
		 * overlap with LAPIC or the CAR area at 0xfef00000.
		 */
		.MicrocodeRegionBase = 0,
		.MicrocodeRegionSize = 0,
		.CodeRegionBase = (uint32_t)0x100000000ULL - CACHE_ROM_SIZE,
		.CodeRegionSize = (uint32_t)CACHE_ROM_SIZE,
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
	fast_spi_cache_bios_region();
	pch_early_iorange_init();
	if (CONFIG(INTEL_LPSS_UART_FOR_CONSOLE))
		uart_bootblock_init();
}

void bootblock_soc_init(void)
{
	report_platform_info();
	bootblock_pch_init();

	/* Program TCO_BASE_ADDRESS and TCO Timer Halt */
	tco_configure();
}
