/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <arch/symbols.h>
#include <console/console.h>
#include <cbmem.h>
#include "../chip.h"
#include <fsp/util.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/romstage.h>
#include <soc/reg_access.h>
#include <soc/storage_test.h>

void mainboard_romstage_entry(void)
{
	bool s3wake;

	post_code(0x20);
	console_init();

	if (CONFIG(STORAGE_TEST)) {
		uint32_t bar;
		pci_devfn_t dev;
		uint32_t previous_bar;
		uint16_t previous_command;

		/* Enable the SD/MMC controller and run the test.  Restore
		 * the BAR and command registers upon completion.
		 */
		dev = PCI_DEV(0, SD_MMC_DEV, SD_MMC_FUNC);
		bar = storage_test_init(dev, &previous_bar, &previous_command);
		storage_test(bar, 1);
		storage_test_complete(dev, previous_bar, previous_command);
	}

	/* Initialize DRAM */
	s3wake = fill_power_state() == ACPI_S3;
	fsp_memory_init(s3wake);

	/* Disable the ROM shadow 0x000e0000 - 0x000fffff */
	disable_rom_shadow();

	/* Initialize the PCIe bridges */
	pcie_init();
}

static struct chipset_power_state power_state;

struct chipset_power_state *get_power_state(void)
{
	return &power_state;
}

int fill_power_state(void)
{
	power_state.prev_sleep_state = 0;
	printk(BIOS_SPEW, "prev_sleep_state %d\n",
	       power_state.prev_sleep_state);
	return power_state.prev_sleep_state;
}

void platform_fsp_memory_init_params_cb(FSPM_UPD *fspm_upd, uint32_t version)
{
	FSPM_ARCH_UPD *aupd;
	const struct soc_intel_quark_config *config;
	void *rmu_data;
	size_t rmu_data_len;
	FSP_M_CONFIG *upd;

	/* Clear SMI and wake events */
	clear_smi_and_wake_events();

	/* Locate the RMU data file in flash */
	rmu_data = locate_rmu_file(&rmu_data_len);
	if (!rmu_data)
		die_with_post_code(POST_INVALID_CBFS,
			"Microcode file (rmu.bin) not found.");

	/* Locate the configuration data from devicetree.cb */
	config = config_of_soc();

	/* Update the architectural UPD values. */
	aupd = &fspm_upd->FspmArchUpd;
	aupd->BootLoaderTolumSize = cbmem_overhead_size();
	aupd->StackBase = (uintptr_t)(CONFIG_FSP_ESRAM_LOC - aupd->StackSize);
	aupd->BootMode = FSP_BOOT_WITH_FULL_CONFIGURATION;

	/* Display the ESRAM layout */
	if (CONFIG(DISPLAY_ESRAM_LAYOUT)) {
		printk(BIOS_SPEW, "\nESRAM Layout:\n\n");
		printk(BIOS_SPEW,
			"+-------------------+ 0x80080000 - ESRAM end\n");
		printk(BIOS_SPEW, "| FSP binary        |\n");
		printk(BIOS_SPEW,
			"+-------------------+ 0x%08x (CONFIG_FSP_ESRAM_LOC)\n",
			CONFIG_FSP_ESRAM_LOC);
		printk(BIOS_SPEW, "| FSP stack         |\n");
		printk(BIOS_SPEW, "+-------------------+ 0x%zx\n",
			(size_t)aupd->StackBase);
		printk(BIOS_SPEW, "|                   |\n");
		printk(BIOS_SPEW, "+-------------------+ %p\n",
			_car_unallocated_start);
		printk(BIOS_SPEW, "| coreboot data     |\n");
		printk(BIOS_SPEW, "+-------------------+ %p\n",
			_ecar_stack);
		printk(BIOS_SPEW, "| coreboot stack    |\n");
		printk(BIOS_SPEW,
			"+-------------------+ 0x80000000 - ESRAM start\n\n");
	}

	/* Update the UPD data for MemoryInit */
	upd = &fspm_upd->FspmConfig;
	upd->AddrMode = config->AddrMode;
	upd->ChanMask = config->ChanMask;
	upd->ChanWidth = config->ChanWidth;
	upd->DramDensity = config->DramDensity;
	upd->DramRonVal = config->DramRonVal;
	upd->DramRttNomVal = config->DramRttNomVal;
	upd->DramRttWrVal = config->DramRttWrVal;
	upd->DramSpeed = config->DramSpeed;
	upd->DramType = config->DramType;
	upd->DramWidth = config->DramWidth;
	upd->EccScrubBlkSize = config->EccScrubBlkSize;
	upd->EccScrubInterval = config->EccScrubInterval;
	upd->Flags = config->Flags;
	upd->FspReservedMemoryLength = config->FspReservedMemoryLength;
	upd->RankMask = config->RankMask;
	upd->RmuBaseAddress = (uintptr_t)rmu_data;
	upd->RmuLength = rmu_data_len;
	upd->SerialPortWriteChar = !!console_log_level(BIOS_SPEW)
		? (uintptr_t)fsp_write_line : 0;
	upd->SmmTsegSize = CONFIG(HAVE_SMI_HANDLER) ?
		config->SmmTsegSize : 0;
	upd->SocRdOdtVal = config->SocRdOdtVal;
	upd->SocWrRonVal = config->SocWrRonVal;
	upd->SocWrSlewRate = config->SocWrSlewRate;
	upd->SrInt = config->SrInt;
	upd->SrTemp = config->SrTemp;
	upd->tCL = config->tCL;
	upd->tFAW = config->tFAW;
	upd->tRAS = config->tRAS;
	upd->tRRD = config->tRRD;
	upd->tWTR = config->tWTR;
}
