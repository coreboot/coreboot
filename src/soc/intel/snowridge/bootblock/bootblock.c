/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <console/console.h>
#include <fsp/util.h>
#include <intelblocks/fast_spi.h>
#include <intelblocks/tco.h>
#include <soc/iomap.h>

#include <FsptUpd.h>

#include "bootblock.h"

const FSPT_UPD temp_ram_init_params = {
	.FspUpdHeader = {
		.Signature = 0x545F445055434F53ULL,
		.Revision = 1,
		.Reserved = {0},
	},
	.FsptCoreUpd = {
		.MicrocodeRegionBase = 0,
		.MicrocodeRegionLength = 0,
		.CodeRegionBase = 0xffe00000,
		.CodeRegionLength = 0x200000,
		.Reserved1 = {0},
	},
	.ReservedTempRamInitUpd = {0},
	.UnusedUpdSpace0 = {0},
	.UpdTerminator = 0x55AA,
};

asmlinkage void bootblock_c_entry(uint64_t base_timestamp)
{
	bootblock_main_with_basetime(base_timestamp);
}

void bootblock_soc_early_init(void)
{
	if (CONFIG(DRIVERS_UART))
		early_uart_init();
}

void bootblock_soc_init(void)
{
	if (CONFIG(BOOTBLOCK_CONSOLE))
		printk(BIOS_DEBUG, "FSP TempRamInit successful...\n");

	if (CONFIG(FSP_CAR))
		report_fspt_output();

	tco_configure();
	tco_reset_status();

	fast_spi_early_init(SPI_BASE_ADDRESS);
}
