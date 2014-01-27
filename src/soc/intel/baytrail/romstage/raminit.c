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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stddef.h>
#include <arch/hlt.h>
#include <arch/io.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <device/pci_def.h>
#include <baytrail/gpio.h>
#include <baytrail/mrc_cache.h>
#include <baytrail/iomap.h>
#include <baytrail/iosf.h>
#include <baytrail/pci_devs.h>
#include <baytrail/reset.h>
#include <baytrail/romstage.h>

#if CONFIG_CHROMEOS
#include <vendorcode/google/chromeos/chromeos.h>
#else
#define recovery_mode_enabled(x) 0
#endif

static void reset_system(void)
{
	warm_reset();
	while(1) { hlt(); }
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
	console_tx_byte(b);
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
	const struct mrc_saved_data *cache;

	/* Fill in default entries. */
	mp->version = MRC_PARAMS_VER;
	mp->console_out = &send_to_console;
	mp->prev_sleep_state = prev_sleep_state;
#if CONFIG_MRC_RMT
	mp->rmt_enabled = 1;
#endif
	if (recovery_mode_enabled()) {
		printk(BIOS_DEBUG, "Recovery mode: not using MRC cache.\n");
	} else if (!mrc_cache_get_current(&cache)) {
		mp->saved_data_size = cache->size;
		mp->saved_data = &cache->data[0];
	} else if (prev_sleep_state == 3) {
		/* If waking from S3 and no cache then. */
		printk(BIOS_DEBUG, "No MRC cache found in S3 resume path.\n");
		post_code(POST_RESUME_FAILURE);
		reset_system();
	} else {
		printk(BIOS_DEBUG, "No MRC cache found.\n");
	}

	mrc_entry = cbfs_get_file_content(CBFS_DEFAULT_MEDIA, "mrc.bin", 0xab,
					  NULL);

	if (mrc_entry == NULL) {
		printk(BIOS_DEBUG, "Couldn't find mrc.bin\n");
		return;
	}
	if (mp->mainboard.dram_info_location == DRAM_INFO_SPD_SMBUS)
		enable_smbus();

	ret = mrc_entry(mp);

	print_dram_info();

	if (prev_sleep_state != 3) {
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
		mrc_cache_stash_data(mp->data_to_save, mp->data_to_save_size);
}
