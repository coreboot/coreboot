/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <cpu/cpu.h>
#include <intelblocks/fast_spi.h>
#include <intelblocks/systemagent.h>
#include <intelblocks/tco.h>
#include <intelblocks/uart.h>
#include <soc/bootblock.h>
#include <stdint.h>

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
	/*
	 * Perform early chipset initialization before fsp memory init
	 * example: pirq->irq programming, enabling smbus, set pmcbase
	 * and abase, i2c programming and print platform info
	 */
	report_platform_info();
	bootblock_pch_init();

	/* Program TCO_BASE_ADDRESS and TCO Timer Halt */
	tco_configure();
}
