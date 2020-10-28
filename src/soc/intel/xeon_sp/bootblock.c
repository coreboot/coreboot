/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <bootblock_common.h>
#include <device/pci.h>
#include <FsptUpd.h>
#include <intelblocks/fast_spi.h>
#include <soc/iomap.h>
#include <console/console.h>
#include <cpu/x86/mtrr.h>
#include <intelblocks/lpc_lib.h>
#include <soc/pci_devs.h>
#include <soc/bootblock.h>
#include <fsp/util.h>

const FSPT_UPD temp_ram_init_params = {
	.FspUpdHeader = {
		.Signature = FSPT_UPD_SIGNATURE,
		.Revision = 1,
		.Reserved = {0},
	},
	.FsptCoreUpd = {
		.MicrocodeRegionBase = (UINT32)CONFIG_CPU_MICROCODE_CBFS_LOC,
		.MicrocodeRegionLength = (UINT32)CONFIG_CPU_MICROCODE_CBFS_LEN,
		.CodeRegionBase = (UINT32)CACHE_ROM_BASE,
		.CodeRegionLength = (UINT32)CACHE_ROM_SIZE,
		.Reserved1 = {0},
	},
	.FsptConfig = {
		.FsptPort80RouteDisable = 0,
		.ReservedTempRamInitUpd = {0},
	},
	.UnusedUpdSpace0 = {0},
	.UpdTerminator = 0x55AA,
};

asmlinkage void bootblock_c_entry(uint64_t base_timestamp)
{
	fast_spi_cache_bios_region();

	bootblock_main_with_basetime(base_timestamp);
}

void bootblock_soc_early_init(void)
{
	fast_spi_early_init(SPI_BASE_ADDRESS);
	pch_enable_lpc();

	/* Set up P2SB BAR. This is needed for PCR to work */
	uint8_t p2sb_cmd = pci_mmio_read_config8(PCH_DEV_P2SB, PCI_COMMAND);
	pci_mmio_write_config8(PCH_DEV_P2SB, PCI_COMMAND, p2sb_cmd | PCI_COMMAND_MEMORY);
	pci_mmio_write_config32(PCH_DEV_P2SB, PCI_BASE_ADDRESS_0, CONFIG_PCR_BASE_ADDRESS);
}

void bootblock_soc_init(void)
{
	if (CONFIG(BOOTBLOCK_CONSOLE))
		printk(BIOS_DEBUG, "FSP TempRamInit successful...\n");
	if (CONFIG(FSP_CAR))
		report_fspt_output();
	bootblock_pch_init();
}
