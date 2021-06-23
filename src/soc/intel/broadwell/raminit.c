/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <cbfs.h>
#include <cbmem.h>
#include <cf9_reset.h>
#include <console/console.h>
#include <device/pci_def.h>
#include <memory_info.h>
#include <mrc_cache.h>
#include <string.h>
#include <soc/iomap.h>
#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>
#include <soc/pm.h>
#include <soc/romstage.h>
#include <soc/systemagent.h>
#include <timestamp.h>

static void save_mrc_data(struct pei_data *pei_data)
{
	printk(BIOS_DEBUG, "MRC data at %p %d bytes\n", pei_data->data_to_save,
	       pei_data->data_to_save_size);

	if (pei_data->data_to_save != NULL && pei_data->data_to_save_size > 0)
		mrc_cache_stash_data(MRC_TRAINING_DATA, 0,
					pei_data->data_to_save,
					pei_data->data_to_save_size);
}

static const char *const ecc_decoder[] = {
	"inactive",
	"active on IO",
	"disabled on IO",
	"active",
};

/*
 * Dump in the log memory controller configuration as read from the memory
 * controller registers.
 */
static void report_memory_config(void)
{
	int i;

	const u32 addr_decoder_common = mchbar_read32(MAD_CHNL);

	printk(BIOS_DEBUG, "memcfg DDR3 clock %d MHz\n",
	       (mchbar_read32(MC_BIOS_DATA) * 13333 * 2 + 50) / 100);

	printk(BIOS_DEBUG, "memcfg channel assignment: A: %d, B % d, C % d\n",
	       (addr_decoder_common >> 0) & 3,
	       (addr_decoder_common >> 2) & 3,
	       (addr_decoder_common >> 4) & 3);

	for (i = 0; i < NUM_CHANNELS; i++) {
		const u32 ch_conf = mchbar_read32(MAD_DIMM(i));

		printk(BIOS_DEBUG, "memcfg channel[%d] config (%8.8x):\n", i, ch_conf);
		printk(BIOS_DEBUG, "   ECC %s\n", ecc_decoder[(ch_conf >> 24) & 3]);
		printk(BIOS_DEBUG, "   enhanced interleave mode %s\n",
		       ((ch_conf >> 22) & 1) ? "on" : "off");

		printk(BIOS_DEBUG, "   rank interleave %s\n",
		       ((ch_conf >> 21) & 1) ? "on" : "off");

		printk(BIOS_DEBUG, "   DIMMA %d MB width %s %s rank%s\n",
		       ((ch_conf >> 0) & 0xff) * 256,
		       ((ch_conf >> 19) & 1) ? "x16" : "x8 or x32",
		       ((ch_conf >> 17) & 1) ? "dual" : "single",
		       ((ch_conf >> 16) & 1) ? "" : ", selected");

		printk(BIOS_DEBUG, "   DIMMB %d MB width %s %s rank%s\n",
		       ((ch_conf >> 8) & 0xff) * 256,
		       ((ch_conf >> 20) & 1) ? "x16" : "x8 or x32",
		       ((ch_conf >> 18) & 1) ? "dual" : "single",
		       ((ch_conf >> 16) & 1) ? ", selected" : "");
	}
}

/*
 * Find PEI executable in coreboot filesystem and execute it.
 */
static void sdram_initialize(struct pei_data *pei_data)
{
	size_t mrc_size;
	pei_wrapper_entry_t entry;
	int ret;

	broadwell_fill_pei_data(pei_data);

	/* Assume boot device is memory mapped. */
	assert(CONFIG(BOOT_DEVICE_MEMORY_MAPPED));

	pei_data->saved_data =
		mrc_cache_current_mmap_leak(MRC_TRAINING_DATA, 0,
					    &mrc_size);
	if (pei_data->saved_data) {
		/* MRC cache found */
		pei_data->saved_data_size = mrc_size;
	} else if (pei_data->boot_mode == ACPI_S3) {
		/* Waking from S3 and no cache. */
		printk(BIOS_DEBUG,
		       "No MRC cache found in S3 resume path.\n");
		post_code(POST_RESUME_FAILURE);
		system_reset();
	} else {
		printk(BIOS_DEBUG, "No MRC cache found.\n");
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

	/* We don't care about leaking the mapping */
	entry = cbfs_ro_map("mrc.bin", NULL);
	if (entry == NULL)
		die("mrc.bin not found!");

	printk(BIOS_DEBUG, "Starting Memory Reference Code\n");

	ret = entry(pei_data);
	if (ret < 0)
		die("pei_data version mismatch\n");

	/* Print the MRC version after executing the UEFI PEI stage. */
	u32 version = mchbar_read32(MRC_REVISION);
	printk(BIOS_DEBUG, "MRC Version %u.%u.%u Build %u\n",
		(version >> 24) & 0xff, (version >> 16) & 0xff,
		(version >>  8) & 0xff, (version >>  0) & 0xff);

	report_memory_config();
}

static void setup_sdram_meminfo(struct pei_data *pei_data)
{
	struct memory_info *mem_info;

	printk(BIOS_DEBUG, "create cbmem for dimm information\n");
	mem_info = cbmem_add(CBMEM_ID_MEMINFO, sizeof(struct memory_info));

	if (!mem_info) {
		printk(BIOS_ERR, "Error! Failed to add mem_info to cbmem\n");
		return;
	}

	memset(mem_info, 0, sizeof(*mem_info));
	/* Translate pei_memory_info struct data into memory_info struct */
	mem_info->dimm_cnt = pei_data->meminfo.dimm_cnt;
	for (int i = 0; i < MIN(DIMM_INFO_TOTAL, PEI_DIMM_INFO_TOTAL); i++) {
		struct dimm_info *dimm = &mem_info->dimm[i];
		const struct pei_dimm_info *pei_dimm =
			&pei_data->meminfo.dimm[i];
		dimm->dimm_size = pei_dimm->dimm_size;
		dimm->ddr_type = pei_dimm->ddr_type;
		dimm->ddr_frequency = pei_dimm->ddr_frequency;
		dimm->rank_per_dimm = pei_dimm->rank_per_dimm;
		dimm->channel_num = pei_dimm->channel_num;
		dimm->dimm_num = pei_dimm->dimm_num;
		dimm->bank_locator = pei_dimm->bank_locator;
		memcpy(&dimm->serial, &pei_dimm->serial,
			MIN(sizeof(dimm->serial), sizeof(pei_dimm->serial)));
		memcpy(&dimm->module_part_number,
			&pei_dimm->module_part_number,
			MIN(sizeof(dimm->module_part_number),
			sizeof(pei_dimm->module_part_number)));
		dimm->module_part_number[DIMM_INFO_PART_NUMBER_SIZE - 1] = '\0';
		dimm->mod_id =  pei_dimm->mod_id;
		dimm->mod_type = pei_dimm->mod_type;
		dimm->bus_width = pei_dimm->bus_width;
	}
}

/*
 * 0 = leave channel enabled
 * 1 = disable dimm 0 on channel
 * 2 = disable dimm 1 on channel
 * 3 = disable dimm 0+1 on channel
 */
static int make_channel_disabled_mask(const struct spd_info *spdi, int ch)
{
	return (!spdi->addresses[ch + ch] << 0) | (!spdi->addresses[ch + ch + 1] << 1);
}

void perform_raminit(const struct chipset_power_state *const power_state)
{
	const int s3resume = power_state->prev_sleep_state == ACPI_S3;

	struct pei_data pei_data = { 0 };

	mainboard_fill_pei_data(&pei_data);

	/* Obtain the SPD addresses from mainboard code */
	struct spd_info spdi = { 0 };
	mb_get_spd_map(&spdi);

	if (CONFIG(HAVE_SPD_IN_CBFS))
		copy_spd(&pei_data, &spdi);

	/* Calculate unimplemented DIMM slots for each channel */
	pei_data.dimm_channel0_disabled = make_channel_disabled_mask(&spdi, 0);
	pei_data.dimm_channel1_disabled = make_channel_disabled_mask(&spdi, 1);

	/* MRC expects left-aligned SMBus addresses, and 0 for memory-down */
	for (size_t i = 0; i < ARRAY_SIZE(spdi.addresses); i++) {
		const uint8_t addr = spdi.addresses[i];
		pei_data.spd_addresses[i] = addr == SPD_MEMORY_DOWN ? 0 : addr << 1;
	}

	post_code(0x32);

	timestamp_add_now(TS_INITRAM_START);

	pei_data.boot_mode = power_state->prev_sleep_state;

	/* Initialize RAM */
	sdram_initialize(&pei_data);

	timestamp_add_now(TS_INITRAM_END);

	int cbmem_was_initted = !cbmem_recovery(s3resume);
	if (s3resume && !cbmem_was_initted) {
		/* Failed S3 resume, reset to come up cleanly */
		printk(BIOS_CRIT, "Failed to recover CBMEM in S3 resume.\n");
		system_reset();
	}

	save_mrc_data(&pei_data);

	setup_sdram_meminfo(&pei_data);
}
