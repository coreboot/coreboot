/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
 * (Written by Andrey Petrov <andrey.petrov@intel.com> for Intel Corp.)
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

#include <assert.h>
#include <arch/cpu.h>
#include <arch/io.h>
#include <arch/symbols.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/x86/mtrr.h>
#include <device/pci_def.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <device/resource.h>
#include <string.h>
#include <soc/iomap.h>
#include <soc/intel/common/mrc_cache.h>
#include <soc/pci_devs.h>
#include <soc/northbridge.h>
#include <soc/romstage.h>
#include <soc/uart.h>

#define FIT_POINTER				(0x100000000ULL - 0x40)

/*
 * Enables several BARs and devices which are needed for memory init
 * - MCH_BASE_ADDR is needed in order to talk to the memory controller
 * - PMC_BAR0 and PMC_BAR1 are used by FSP (with the base address hardcoded)
 *   Once raminit is done, we can safely let the allocator re-assign them
 * - HPET is enabled because FSP wants to store a pointer to global data in the
 *   HPET comparator register
 */
static void soc_early_romstage_init(void)
{
	device_t pmc = PMC_DEV;

	/* Set MCH base address and enable bit */
	pci_write_config32(NB_DEV_ROOT, MCHBAR, MCH_BASE_ADDR | 1);

	/* Set PMC base addresses and enable decoding. */
	pci_write_config32(pmc, PCI_BASE_ADDRESS_0, PMC_BAR0);
	pci_write_config32(pmc, PCI_BASE_ADDRESS_1, 0);	/* 64-bit BAR */
	pci_write_config32(pmc, PCI_BASE_ADDRESS_2, PMC_BAR1);
	pci_write_config32(pmc, PCI_BASE_ADDRESS_3, 0);	/* 64-bit BAR */
	pci_write_config16(pmc, PCI_BASE_ADDRESS_4, ACPI_PMIO_BASE);
	pci_write_config32(pmc, PCI_COMMAND,
				PCI_COMMAND_IO | PCI_COMMAND_MEMORY |
				PCI_COMMAND_MASTER);

	/* Enable decoding for HPET. Needed for FSP global pointer storage */
	pci_write_config32(P2SB_DEV, 0x60, 1<<7);
}

static void disable_watchdog(void)
{
	uint32_t reg;

	/* Stop TCO timer */
	reg = inl(ACPI_PMIO_BASE + 0x68);
	reg |= 1 << 11;
	outl(reg, ACPI_PMIO_BASE + 0x68);
}

asmlinkage void car_stage_entry(void)
{
	void *hob_list_ptr, *mrc_data;
	struct range_entry fsp_mem, reg_car;
	struct postcar_frame pcf;
	size_t  mrc_data_size;
	uintptr_t top_of_ram;

	printk(BIOS_DEBUG, "Starting romstage...\n");

	soc_early_romstage_init();

	disable_watchdog();

	/* Make sure the blob does not override our data in CAR */
	range_entry_init(&reg_car, (uintptr_t)_car_relocatable_data_end,
			(uintptr_t)_car_region_end, 0);

	if (fsp_memory_init(&hob_list_ptr, &reg_car) != FSP_SUCCESS) {
		die("FSP memory init failed. Giving up.");
	}

	fsp_find_reserved_memory(&fsp_mem, hob_list_ptr);

	/* initialize cbmem by adding FSP reserved memory first thing */
	cbmem_initialize_empty_id_size(CBMEM_ID_FSP_RESERVED_MEMORY,
					range_entry_size(&fsp_mem));

	/* make sure FSP memory is reserved in cbmem */
	if (range_entry_base(&fsp_mem) !=
		(uintptr_t)cbmem_find(CBMEM_ID_FSP_RESERVED_MEMORY))
		die("Failed to accommodate FSP reserved memory request");

	/* Now that CBMEM is up, save the list so ramstage can use it */
	fsp_save_hob_list(hob_list_ptr);

	/* Save MRC Data to CBMEM */
	if (IS_ENABLED(CONFIG_CACHE_MRC_SETTINGS))
	{
		/* TODO: treat MRC data as const */
		mrc_data = (void*) fsp_find_nv_storage_data(&mrc_data_size);
		if (mrc_data && mrc_cache_stash_data(mrc_data, mrc_data_size) < 0)
			printk(BIOS_ERR, "Failed to stash MRC data\n");
	}

	if (postcar_frame_init(&pcf, 1*KiB))
		die("Unable to initialize postcar frame.\n");

	/*
	 * We need to make sure ramstage will be run cached. At this point exact
	 * location of ramstage in cbmem is not known. Instruct postcar to cache
	 * 16 megs under cbmem top which is a safe bet to cover ramstage.
	 */
	top_of_ram = (uintptr_t) cbmem_top();
	/* cbmem_top() needs to be at least 16 MiB aligned */
	assert(ALIGN_DOWN(top_of_ram, 16*MiB) == top_of_ram);
	postcar_frame_add_mtrr(&pcf, top_of_ram - 16*MiB, 16*MiB, MTRR_TYPE_WRBACK);

	run_postcar_phase(&pcf);
}

static void fill_console_params(struct FSPM_UPD *mupd)
{
	if (IS_ENABLED(CONFIG_CONSOLE_SERIAL)) {
		mupd->FspmConfig.SerialDebugPortDevice = CONFIG_UART_FOR_CONSOLE;
		/* use MMIO port type */
		mupd->FspmConfig.SerialDebugPortType = 2;
		/* use 4 byte register stride */
		mupd->FspmConfig.SerialDebugPortStrideSize = 2;
		/* used only for port type set to external */
		mupd->FspmConfig.SerialDebugPortAddress = 0;
	} else {
		mupd->FspmConfig.SerialDebugPortType = 0;
	}
}

void platform_fsp_memory_init_params_cb(struct FSPM_UPD *mupd)
{
	const struct mrc_saved_data *mrc_cache;
	struct FSP_M_ARCH_UPD *arch_upd = &mupd->FspmArchUpd;

	fill_console_params(mupd);
	mainboard_memory_init_params(mupd);

	/* Do NOT let FSP do any GPIO pad configuration */
	mupd->FspmConfig.GpioPadInitTablePtr = NULL;
	/*
	 * At FIT_POINTER there is an address that points to FIT. Even though it
	 * is technically 64bit value we know only 32bit address is used.
	 */
	mupd->FspmConfig.FitTablePtr = read32((void*) FIT_POINTER);
	/* Reserve enough memory under TOLUD to save CBMEM header */
	mupd->FspmArchUpd.BootLoaderTolumSize = cbmem_overhead_size();
	/*
	 * FSPM_UPD passed here is populated with default values provided by
	 * the blob itself. We let FSPM use top of CAR region of the size it
	 * requests.
	 * TODO: add checks to avoid overlap/conflict of CAR usage.
	 */
	mupd->FspmArchUpd.StackBase = _car_region_end -
					mupd->FspmArchUpd.StackSize;

	arch_upd->Bootmode = FSP_BOOT_WITH_FULL_CONFIGURATION;

	if (IS_ENABLED(CONFIG_CACHE_MRC_SETTINGS)) {
		if (!mrc_cache_get_current_with_version(&mrc_cache, 0)) {
			/* MRC cache found */
			arch_upd->NvsBufferPtr = (void *)mrc_cache->data;
			arch_upd->Bootmode = FSP_BOOT_ASSUMING_NO_CONFIGURATION_CHANGES;
			printk(BIOS_DEBUG, "MRC cache found, size %x\n", mrc_cache->size);
		} else
			printk(BIOS_DEBUG, "MRC cache was not found\n");
	}
}

__attribute__ ((weak))
void mainboard_memory_init_params(struct FSPM_UPD *mupd)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}
