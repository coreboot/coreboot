/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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

#include <stddef.h>
#include <arch/acpi.h>
#include <assert.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <device/pci_def.h>
#include <halt.h>
#include <soc/gpio.h>
#include <soc/intel/common/mrc_cache.h>
#include <soc/iomap.h>
#include <soc/iosf.h>
#include <soc/pci_devs.h>
#include <soc/reset.h>
#include <soc/romstage.h>
#include <ec/google/chromeec/ec.h>
#include <ec/google/chromeec/ec_commands.h>
#include <vboot/vboot_common.h>

static void reset_system(void)
{
	warm_reset();
	halt();
}

static void enable_smbus(void)
{
	uint32_t reg;
	const uint32_t smbus_dev = PCI_DEV(0, SMBUS_DEV, SMBUS_FUNC);

	/* SMBus I/O BAR */
	reg = SMBUS_BASE_ADDRESS | 2;
	pci_write_config32(smbus_dev, PCI_BASE_ADDRESS_4, reg);
	/* Enable decode of I/O space. */
	reg = pci_read_config16(smbus_dev, PCI_COMMAND);
	reg |= 0x1;
	pci_write_config16(smbus_dev, PCI_COMMAND, reg);
	/* Enable Host Controller */
	reg = pci_read_config8(smbus_dev, 0x40);
	reg |= 1;
	pci_write_config8(smbus_dev, 0x40, reg);

	/* Configure pads to be used for SMBus */
	score_select_func(PCU_SMB_CLK_PAD, 1);
	score_select_func(PCU_SMB_DATA_PAD, 1);
}

static void ABI_X86 send_to_console(unsigned char b)
{
	do_putchar(b);
}

static void print_dram_info(void)
{
	const int mrc_ver_reg = 0xf0;
	const uint32_t soc_dev = PCI_DEV(0, SOC_DEV, SOC_FUNC);
	uint32_t reg;
	int num_channels;
	int speed;
	uint32_t ch0;
	uint32_t ch1;

	reg = pci_read_config32(soc_dev, mrc_ver_reg);

	printk(BIOS_INFO, "MRC v%d.%02d\n", (reg >> 8) & 0xff, reg & 0xff);

	/* Number of channels enabled and DDR3 type. Determine number of
	 * channels by keying of the rank enable bits [3:0]. * */
	ch0 = iosf_dunit_ch0_read(DRP);
	ch1 = iosf_dunit_ch1_read(DRP);
	num_channels = 0;
	if (ch0 & DRP_RANK_MASK)
		num_channels++;
	if (ch1 & DRP_RANK_MASK)
		num_channels++;

	printk(BIOS_INFO, "%d channels of %sDDR3 @ ", num_channels,
	       (reg & (1 << 22)) ? "LP" : "");

	/* DRAM frequency -- all channels run at same frequency. */
	reg = iosf_dunit_read(DTR0);
	switch (reg & 0x3) {
	case 0:
		speed = 800; break;
	case 1:
		speed = 1066; break;
	case 2:
		speed = 1333; break;
	case 3:
		speed = 1600; break;
	}
	printk(BIOS_INFO, "%dMHz\n", speed);
}

void raminit(struct mrc_params *mp, int prev_sleep_state)
{
	int ret;
	mrc_wrapper_entry_t mrc_entry;
	struct region_device rdev;

	/* Fill in default entries. */
	mp->version = MRC_PARAMS_VER;
	mp->console_out = &send_to_console;
	mp->prev_sleep_state = prev_sleep_state;
	mp->rmt_enabled = IS_ENABLED(CONFIG_MRC_RMT);

	/* Default to 2GiB IO hole. */
	if (!mp->io_hole_mb)
		mp->io_hole_mb = 2048;

	if (vboot_recovery_mode_enabled()) {
		printk(BIOS_DEBUG, "Recovery mode: not using MRC cache.\n");
	} else if (!mrc_cache_get_current(MRC_TRAINING_DATA, 0, &rdev)) {
		mp->saved_data_size = region_device_sz(&rdev);
		mp->saved_data = rdev_mmap_full(&rdev);
		/* Assume boot device is memory mapped. */
		assert(IS_ENABLED(CONFIG_BOOT_DEVICE_MEMORY_MAPPED));
	} else if (prev_sleep_state == ACPI_S3) {
		/* If waking from S3 and no cache then. */
		printk(BIOS_DEBUG, "No MRC cache found in S3 resume path.\n");
		post_code(POST_RESUME_FAILURE);
		reset_system();
	} else {
		printk(BIOS_DEBUG, "No MRC cache found.\n");
#if CONFIG_EC_GOOGLE_CHROMEEC
		if (prev_sleep_state == ACPI_S0) {
			/* Ensure EC is running RO firmware. */
			google_chromeec_check_ec_image(EC_IMAGE_RO);
		}
#endif
	}

	/* Determine if mrc.bin is in the cbfs. */
	if (cbfs_boot_map_with_leak("mrc.bin", CBFS_TYPE_MRC, NULL) == NULL) {
		printk(BIOS_DEBUG, "Couldn't find mrc.bin\n");
		return;
	}

	/*
	 * The entry point is currently the first instruction. Handle the
	 * case of an ELF file being put in the cbfs by setting the entry
	 * to the CONFIG_MRC_BIN_ADDRESS.
	 */
	mrc_entry = (void *)(uintptr_t)CONFIG_MRC_BIN_ADDRESS;

	if (mp->mainboard.dram_info_location == DRAM_INFO_SPD_SMBUS)
		enable_smbus();

	ret = mrc_entry(mp);

	print_dram_info();

	if (prev_sleep_state != ACPI_S3) {
		cbmem_initialize_empty();
	} else if (cbmem_initialize()) {
	#if CONFIG_HAVE_ACPI_RESUME
		printk(BIOS_DEBUG, "Failed to recover CBMEM in S3 resume.\n");
		/* Failed S3 resume, reset to come up cleanly */
		reset_system();
	#endif
	}

	printk(BIOS_DEBUG, "MRC Wrapper returned %d\n", ret);
	printk(BIOS_DEBUG, "MRC data at %p %d bytes\n", mp->data_to_save,
	       mp->data_to_save_size);

	if (mp->data_to_save != NULL && mp->data_to_save_size > 0)
		mrc_cache_stash_data(MRC_TRAINING_DATA, 0, mp->data_to_save,
					mp->data_to_save_size);
}
