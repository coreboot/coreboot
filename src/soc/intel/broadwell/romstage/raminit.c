/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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

#include <arch/cbfs.h>
#include <arch/io.h>
#include <assert.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <device/pci_def.h>
#include <lib.h>
#include <string.h>
#if CONFIG_EC_GOOGLE_CHROMEEC
#include <ec/google/chromeec/ec.h>
#include <ec/google/chromeec/ec_commands.h>
#endif
#include <vendorcode/google/chromeos/chromeos.h>
#include <soc/intel/common/mrc_cache.h>
#include <soc/iomap.h>
#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>
#include <soc/pm.h>
#include <soc/reset.h>
#include <soc/romstage.h>
#include <soc/smm.h>
#include <soc/systemagent.h>

/*
 * Find PEI executable in coreboot filesystem and execute it.
 */
void raminit(struct pei_data *pei_data)
{
	struct region_device rdev;
	struct memory_info* mem_info;
	pei_wrapper_entry_t entry;
	int ret;

	broadwell_fill_pei_data(pei_data);

	if (vboot_recovery_mode_enabled()) {
		/* Recovery mode does not use MRC cache */
		printk(BIOS_DEBUG, "Recovery mode: not using MRC cache.\n");
	} else if (!mrc_cache_get_current(MRC_TRAINING_DATA, 0, &rdev)) {
		/* MRC cache found */
		pei_data->saved_data_size = region_device_sz(&rdev);
		pei_data->saved_data = rdev_mmap_full(&rdev);
		/* Assume boot device is memory mapped. */
		assert(IS_ENABLED(CONFIG_BOOT_DEVICE_MEMORY_MAPPED));
	} else if (pei_data->boot_mode == ACPI_S3) {
		/* Waking from S3 and no cache. */
		printk(BIOS_DEBUG, "No MRC cache found in S3 resume path.\n");
		post_code(POST_RESUME_FAILURE);
		reset_system();
	} else {
		printk(BIOS_DEBUG, "No MRC cache found.\n");
#if CONFIG_EC_GOOGLE_CHROMEEC
		if (pei_data->boot_mode == ACPI_S0) {
			/* Ensure EC is running RO firmware. */
			google_chromeec_check_ec_image(EC_IMAGE_RO);
		}
#endif
	}

	/*
	 * Do not use saved pei data.  Can be set by mainboard romstage
	 * to force a full train of memory on every boot.
	 */
	if (pei_data->disable_saved_data) {
		printk(BIOS_DEBUG, "Disabling PEI saved data by request\n");
		pei_data->saved_data = NULL;
		pei_data->saved_data_size = 0;
	}

	/* Determine if mrc.bin is in the cbfs. */
	entry = cbfs_boot_map_with_leak("mrc.bin", CBFS_TYPE_MRC, NULL);
	if (entry == NULL) {
		printk(BIOS_DEBUG, "Couldn't find mrc.bin\n");
		return;
	}

	printk(BIOS_DEBUG, "Starting Memory Reference Code\n");

	ret = entry(pei_data);
	if (ret < 0)
		die("pei_data version mismatch\n");

	/* Print the MRC version after executing the UEFI PEI stage. */
	u32 version = MCHBAR32(MCHBAR_PEI_VERSION);
	printk(BIOS_DEBUG, "MRC Version %d.%d.%d Build %d\n",
		version >> 24 , (version >> 16) & 0xff,
		(version >> 8) & 0xff, version & 0xff);

	report_memory_config();

	/* Basic memory sanity test */
	quick_ram_check();

	if (pei_data->boot_mode != ACPI_S3) {
		cbmem_initialize_empty();
	} else if (cbmem_initialize()) {
#if CONFIG_HAVE_ACPI_RESUME
		printk(BIOS_DEBUG, "Failed to recover CBMEM in S3 resume.\n");
		/* Failed S3 resume, reset to come up cleanly */
		reset_system();
#endif
	}

	printk(BIOS_DEBUG, "MRC data at %p %d bytes\n", pei_data->data_to_save,
	       pei_data->data_to_save_size);

	if (pei_data->data_to_save != NULL && pei_data->data_to_save_size > 0)
		mrc_cache_stash_data(MRC_TRAINING_DATA, 0,
					pei_data->data_to_save,
					pei_data->data_to_save_size);

	printk(BIOS_DEBUG, "create cbmem for dimm information\n");
	mem_info = cbmem_add(CBMEM_ID_MEMINFO, sizeof(struct memory_info));
	memcpy(mem_info, &pei_data->meminfo, sizeof(struct memory_info));

}
