/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <gpio.h>
#include <lib.h>
#include <memory_info.h>
#include <smbios.h>
#include <spd.h>
#include <soc/gpio.h>
#include <soc/romstage.h>
#include <string.h>
#include <spd_bin.h>
#include "spd_util.h"

__weak uint8_t get_ramid(void)
{
	gpio_t spd_gpios[] = {
		GP_SW_80,	/* SATA_GP3, RAMID0 */
		GP_SW_67,	/* I2C3_SCL, RAMID1 */
		GP_SE_02,	/* MF_PLT_CLK1, RAMID2 */
		GP_SW_64,	/* I2C3_SDA, RAMID3 */
	};

	return gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios));
}

static void *get_spd_pointer(int *dual)
{
	char *spd_file;
	size_t spd_file_len;
	int total_spds;
	int ram_id = 0;
	int spd_index = 0;

	/* Find the SPD data in CBFS. */
	spd_file = cbfs_map("spd.bin", &spd_file_len);
	if (!spd_file)
		die("SPD data not found.");

	if (spd_file_len < SPD_PAGE_LEN)
		die("Missing SPD data.");
	total_spds = spd_file_len / SPD_PAGE_LEN;

	ram_id = get_ramid();
	printk(BIOS_DEBUG, "ram_id=%d, total_spds: %d\n", ram_id, total_spds);

	spd_index = get_variant_spd_index(ram_id, dual);
	if (spd_index >= total_spds) {
		printk(BIOS_ERR, "SPD index > total SPDs\n");
		return NULL;
	}
	/* Return the serial product data for the RAM */
	return &spd_file[SPD_PAGE_LEN * spd_index];
}

/* Copy SPD data for on-board memory */
void spd_memory_init_params(MEMORY_INIT_UPD *memory_params)
{
	void *spd_content;
	int dual_channel = 0;

	/*
	 * Both channels are always present in SPD data. Always use matched
	 * DIMMs so use the same SPD data for each DIMM.
	 */
	spd_content = get_spd_pointer(&dual_channel);
	if (CONFIG(DISPLAY_SPD_DATA) && spd_content != NULL) {
		printk(BIOS_DEBUG, "SPD Data:\n");
		hexdump(spd_content, SPD_PAGE_LEN);
		printk(BIOS_DEBUG, "\n");
	}

	/*
	 * Set SPD and memory configuration:
	 * Memory type: 0=DimmInstalled,
	 *              1=SolderDownMemory,
	 *              2=DimmDisabled
	 */
	if (spd_content != NULL) {
		memory_params->PcdMemChannel0Config = 1;
		printk(BIOS_DEBUG, "Channel 0 DIMM soldered down\n");
		if (dual_channel) {
			printk(BIOS_DEBUG, "Channel 1 DIMM soldered down\n");
			memory_params->PcdMemChannel1Config = 1;
		} else {
			printk(BIOS_DEBUG, "Channel 1 DIMM not installed\n");
			memory_params->PcdMemChannel1Config = 2;
		}
	}

	/* Update SPD data */
	if (CONFIG(BOARD_GOOGLE_CYAN)) {
		memory_params->PcdMemoryTypeEnable = MEM_DDR3;
		memory_params->PcdMemorySpdPtr = (uintptr_t)spd_content;
	} else {
		memory_params->PcdMemoryTypeEnable = MEM_LPDDR3;
	}
}

static void set_dimm_info(const uint8_t *spd, struct dimm_info *dimm)
{
	const int spd_capmb[8] = {  1,  2,  4,  8, 16, 32, 64,  0 };
	const int spd_ranks[8] = {  1,  2,  3,  4, -1, -1, -1, -1 };
	const int spd_devw[8]  = {  4,  8, 16, 32, -1, -1, -1, -1 };
	const int spd_busw[8]  = {  8, 16, 32, 64, -1, -1, -1, -1 };

	int capmb = spd_capmb[spd[SPD_DENSITY_BANKS] & 7] * 256;
	int ranks = spd_ranks[(spd[DDR3_ORGANIZATION] >> 3) & 7];
	int devw  = spd_devw[spd[DDR3_ORGANIZATION] & 7];
	int busw  = spd_busw[spd[DDR3_BUS_DEV_WIDTH] & 7];

	void *hob_list_ptr;
	EFI_HOB_GUID_TYPE *hob_ptr;
	FSP_SMBIOS_MEMORY_INFO *memory_info_hob;
	const EFI_GUID memory_info_hob_guid = FSP_SMBIOS_MEMORY_INFO_GUID;

	/* Locate the memory info HOB, presence validated by raminit */
	hob_list_ptr = fsp_get_hob_list();
	hob_ptr = get_guid_hob(&memory_info_hob_guid, hob_list_ptr);
	if (hob_ptr != NULL) {
		memory_info_hob = (FSP_SMBIOS_MEMORY_INFO *)(hob_ptr + 1);
		dimm->ddr_frequency = memory_info_hob->MemoryFrequencyInMHz;
	} else {
		printk(BIOS_ERR, "Can't get memory info hob pointer\n");
		dimm->ddr_frequency = 0;
	}

	/* Parse the SPD data to determine the DIMM information */
	if (CONFIG(BOARD_GOOGLE_CYAN)) {
		dimm->ddr_type = MEMORY_TYPE_DDR3;
	} else {
		dimm->ddr_type = MEMORY_TYPE_LPDDR3;
	}
	dimm->dimm_size = capmb / 8 * busw / devw * ranks;  /* MiB */
	dimm->mod_type = spd[3] & 0xf;
	strncpy((char *)&dimm->module_part_number[0], (char *)&spd[0x80],
		LPDDR3_SPD_PART_LEN);
	dimm->module_part_number[LPDDR3_SPD_PART_LEN] = 0;
	dimm->mod_id = *(uint16_t *)&spd[0x94];

	switch (busw) {
	default:
	case 8:
		dimm->bus_width = MEMORY_BUS_WIDTH_8;
		break;

	case 16:
		dimm->bus_width = MEMORY_BUS_WIDTH_16;
		break;

	case 32:
		dimm->bus_width = MEMORY_BUS_WIDTH_32;
		break;

	case 64:
		dimm->bus_width = MEMORY_BUS_WIDTH_64;
		break;
	}
}

void mainboard_save_dimm_info(struct romstage_params *params)
{
	const void *spd_content;
	int dual_channel;
	struct dimm_info *dimm;
	struct memory_info *mem_info;

	spd_content = get_spd_pointer(&dual_channel);
	if (spd_content == NULL)
		return;

	/*
	 * Allocate CBMEM area for DIMM information used to populate SMBIOS
	 * table 17
	 */
	mem_info = cbmem_add(CBMEM_ID_MEMINFO, sizeof(*mem_info));
	printk(BIOS_DEBUG, "CBMEM entry for DIMM info: %p\n", mem_info);
	if (mem_info == NULL)
		return;
	memset(mem_info, 0, sizeof(*mem_info));

	/* Describe the first channel memory */
	dimm = &mem_info->dimm[0];
	set_dimm_info(spd_content, dimm);
	mem_info->dimm_cnt = 1;

	/* Describe the second channel memory */
	if (dual_channel) {
		dimm = &mem_info->dimm[1];
		set_dimm_info(spd_content, dimm);
		dimm->channel_num = 1;
		mem_info->dimm_cnt = 2;
	}
}
