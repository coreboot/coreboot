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
#include <arch/io.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <device/pci_def.h>
#include <baytrail/gpio.h>
#include <baytrail/mrc_cache.h>
#include <baytrail/iomap.h>
#include <baytrail/pci_devs.h>
#include <baytrail/romstage.h>


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

void raminit(struct mrc_params *mp, int prev_sleep_state)
{
	int ret;
	mrc_wrapper_entry_t mrc_entry;
	const struct mrc_saved_data *cache;

	/* Fill in default entries. */
	mp->version = MRC_PARAMS_VER;
	mp->console_out = &console_tx_byte;
	mp->prev_sleep_state = prev_sleep_state;

	if (!mrc_cache_get_current(&cache)) {
		mp->saved_data_size = cache->size;
		mp->saved_data = &cache->data[0];
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

	cbmem_initialize_empty();

	printk(BIOS_DEBUG, "MRC Wrapper returned %d\n", ret);
	printk(BIOS_DEBUG, "MRC data at %p %d bytes\n", mp->data_to_save,
	       mp->data_to_save_size);

	if (mp->data_to_save != NULL && mp->data_to_save_size > 0)
		mrc_cache_stash_data(mp->data_to_save, mp->data_to_save_size);
}
