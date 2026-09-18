/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <acpi/acpi.h>
#include <assert.h>
#include <cbfs.h>
#include <cbmem.h>
#include <cf9_reset.h>
#include <console/console.h>
#include <device/dram/ddr3.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <device/smbus_host.h>
#include <lib.h>
#include <mrc_cache.h>
#include <soc/iomap.h>
#include <soc/iosf.h>
#include <soc/mrc_wrapper.h>
#include <soc/pci_devs.h>
#include <soc/romstage.h>

static void MRC_ABI_X86 send_to_console(unsigned char b)
{
	do_putchar(b);
}

static void populate_smbios_tables(struct memory_init_params *memory_init_params,
				   int speed, uint32_t channel_mask)
{
	for (size_t ch = 0; ch < NUM_CHANNELS; ++ch) {
		struct spd_cfg *cfg = &memory_init_params->spd_cfgs[ch];
		if (channel_mask & BIT(ch))
			spd_add_smbios17(ch, 0, speed, &cfg->dimm);
	}
}

static void print_dram_info(struct memory_init_params *memory_init_params)
{
	const int mrc_ver_reg = 0xf0;
	const uint32_t soc_dev = PCI_DEV(0, SOC_DEV, SOC_FUNC);
	uint32_t reg;
	uint32_t channel_mask;
	int speed;
	uint32_t ch0;
	uint32_t ch1;

	reg = pci_read_config32(soc_dev, mrc_ver_reg);

	printk(BIOS_INFO, "MRC v%u.%02u\n", (reg >> 8) & 0xff, reg & 0xff);

	/* Number of channels enabled and DDR3 type. Determine number of
	 * channels by keying of the rank enable bits [3:0]. * */
	ch0 = iosf_dunit_ch0_read(DRP);
	ch1 = iosf_dunit_ch1_read(DRP);
	channel_mask = 0;
	if (ch0 & DRP_RANK_MASK)
		channel_mask |= BIT(0);
	if (ch1 & DRP_RANK_MASK)
		channel_mask |= BIT(1);

	printk(BIOS_INFO, "%u channels of %sDDR3 @ ", popcnt(channel_mask),
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
	printk(BIOS_INFO, "%d MT/s\n", speed);

	populate_smbios_tables(memory_init_params, speed, channel_mask);
}

void raminit(struct memory_init_params *memory_init_params, int prev_sleep_state)
{
	bool s3resume = prev_sleep_state == ACPI_S3;
	struct mrc_params mrc_params = {};
	size_t mrc_cache_size;
	mrc_wrapper_entry_t mrc_entry;
	int mrc_ret;

	printk(BIOS_DEBUG, "MRC params at %p %zd bytes\n", &mrc_params, sizeof(mrc_params));

	/* Fill in default entries. */
	mrc_params.version = MRC_PARAMS_VER;
	mrc_params.console_out = &send_to_console;
	mrc_params.prev_sleep_state = prev_sleep_state;
	mrc_params.rmt_enabled = CONFIG(MRC_RMT);
	mrc_params.io_hole_mb = 2048;

	/* Transcribe params into MRC format. */
	switch (memory_init_params->dram_type) {
	case DRAM_TYPE_DDR3:
		mrc_params.mainboard.dram_type = MRC_DRAM_DDR3;
		break;
	case DRAM_TYPE_DDR3L:
		mrc_params.mainboard.dram_type = MRC_DRAM_DDR3L;
		break;
	case DRAM_TYPE_LPDDR3:
		mrc_params.mainboard.dram_type = MRC_DRAM_LPDDR3;
		break;
	}
	mrc_params.mainboard.dram_is_slotted = memory_init_params->dram_is_slotted;
	mrc_params.mainboard.weaker_odt_settings = memory_init_params->weaker_odt_settings;
	mrc_params.mainboard.cpu_odt_value = memory_init_params->cpu_odt_value;
	mrc_params.mainboard.dram_odt_value = memory_init_params->dram_odt_value;

	mrc_params.mainboard.dram_info_location = MRC_DRAM_INFO_SPD_MEM;
	for (size_t ch = 0; ch < NUM_CHANNELS; ch++) {
		if (memory_init_params->spd_cfgs[ch].src == SPD_SRC_SMBUS) {
			enable_smbus();
			break;
		}
	}
	for (size_t ch = 0; ch < NUM_CHANNELS; ch++) {
		struct spd_cfg *cfg = &memory_init_params->spd_cfgs[ch];

		/* Channel unused by board. */
		if (cfg->src == SPD_SRC_NONE)
			continue;

		/* Read SPD from SMBUS if requested. */
		if (cfg->src == SPD_SRC_SMBUS &&
		    i2c_eeprom_read(cfg->addr, 0, sizeof(spd_ddr3_raw_data), cfg->data) < 0) {
			printk(BIOS_INFO, "Channel %zu SPD read failed\n", ch);
			continue;
		}

		/* Decode into dimm_attr struct. */
		enum spd_status status = spd_decode_ddr3(&cfg->dimm, cfg->data);

		/* Some SPDs have bad CRCs, nothing we can do about it. */
		if (status == SPD_STATUS_OK || status == SPD_STATUS_CRC_ERROR) {
			printk(BIOS_INFO, "Channel %zu SPD\n", ch);
			dram_print_spd_ddr3(&cfg->dimm);
		} else {
			printk(BIOS_INFO, "Channel %zu SPD decode failed\n", ch);
			continue;
		}

		/* Force DIMM type to SO-DIMM and always mark DIMM
		 * to be operable at 1.35V.
		 * The Bay Trail MRC enforces form-factor and voltage
		 * values and panics if the above are not met. */
		cfg->data[3] = SPD_DDR3_DIMM_TYPE_SO_DIMM;
		cfg->data[6] |= (1 << 1);

		/* Provide pointer to channel SPD data to MRC. */
		mrc_params.mainboard.dram_data[ch] = cfg->data;
	}

	/* Retrieve saved MRC cache. */
	assert(CONFIG(BOOT_DEVICE_MEMORY_MAPPED)); /* Assume boot device is memory mapped. */

	mrc_params.saved_data = mrc_cache_current_mmap_leak(MRC_TRAINING_DATA, 0, &mrc_cache_size);
	if (mrc_params.saved_data) {
		mrc_params.saved_data_size = mrc_cache_size;
	} else if (s3resume) {
		/* If waking from S3 and no cache then. */
		printk(BIOS_DEBUG, "No MRC cache found in S3 resume path.\n");
		post_code(POSTCODE_RESUME_FAILURE);
		system_reset();
	} else {
		printk(BIOS_DEBUG, "No MRC cache found.\n");
	}

	/* Determine if mrc.bin is in the cbfs. */
	if (cbfs_map("mrc.bin", NULL) == NULL) {
		printk(BIOS_DEBUG, "Couldn't find mrc.bin\n");
		return;
	}

	/*
	 * The entry point is currently the first instruction. Handle the
	 * case of an ELF file being put in the cbfs by setting the entry
	 * to the CONFIG_MRC_BIN_ADDRESS.
	 */
	mrc_entry = (void *)(uintptr_t)CONFIG_MRC_BIN_ADDRESS;

	mrc_ret = mrc_entry(&mrc_params);

	bool cbmem_was_initted = !cbmem_recovery(s3resume);
	if (s3resume && !cbmem_was_initted) {
		/* Failed S3 resume, reset to come up cleanly */
		printk(BIOS_CRIT, "Failed to recover CBMEM in S3 resume.\n");
		system_reset();
	}

	print_dram_info(memory_init_params);

	printk(BIOS_DEBUG, "MRC Wrapper returned %d\n", mrc_ret);
	printk(BIOS_DEBUG, "MRC data at %p %d bytes\n", mrc_params.data_to_save,
	       mrc_params.data_to_save_size);

	if (mrc_params.data_to_save != NULL && mrc_params.data_to_save_size > 0)
		mrc_cache_stash_data(MRC_TRAINING_DATA, 0, mrc_params.data_to_save,
					mrc_params.data_to_save_size);
}
