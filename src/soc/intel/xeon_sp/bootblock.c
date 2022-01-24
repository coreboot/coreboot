/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <bootblock_common.h>
#include <device/pci.h>
#include <FsptUpd.h>
#include <intelblocks/fast_spi.h>
#include <intelblocks/tco.h>
#include <soc/iomap.h>
#include <console/console.h>
#include <cpu/x86/mtrr.h>
#include <intelblocks/lpc_lib.h>
#include <security/intel/cbnt/cbnt.h>
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

static uint64_t assembly_timestamp;
static uint64_t bootblock_timestamp;

asmlinkage void bootblock_c_entry(uint64_t base_timestamp)
{
	/*
	 * FSP-T does not respect its own API and trashes registers
	 * coreboot uses to store its initial timestamp.
	 */
	assembly_timestamp = base_timestamp;
	bootblock_timestamp = timestamp_get();
	fast_spi_cache_bios_region();

	bootblock_main_with_basetime(MIN(assembly_timestamp, bootblock_timestamp));
}

void bootblock_soc_early_init(void)
{
	fast_spi_early_init(SPI_BASE_ADDRESS);
	pch_enable_lpc();

	/* Set up P2SB BAR. This is needed for PCR to work */
	uint8_t p2sb_cmd = pci_s_read_config8(PCH_DEV_P2SB, PCI_COMMAND);
	pci_s_write_config8(PCH_DEV_P2SB, PCI_COMMAND, p2sb_cmd | PCI_COMMAND_MEMORY);
	pci_s_write_config32(PCH_DEV_P2SB, PCI_BASE_ADDRESS_0, CONFIG_PCR_BASE_ADDRESS);
}

void bootblock_soc_init(void)
{
	if (assembly_timestamp > bootblock_timestamp)
		printk(BIOS_WARNING, "Invalid initial timestamp detected\n");

	if (CONFIG(FSP_CAR))
		report_fspt_output();

	if (CONFIG(INTEL_CBNT_LOGGING))
		intel_cbnt_log_registers();

	bootblock_pch_init();

	/* Programming TCO_BASE_ADDRESS and TCO Timer Halt */
	tco_configure();
}
