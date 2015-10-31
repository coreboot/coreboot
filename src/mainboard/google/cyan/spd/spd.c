/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
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

#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <lib.h>
#include <memory_info.h>
#include <smbios.h>
#include <spd.h>
#include <soc/gpio.h>
#include <soc/romstage.h>
#include <string.h>

#define SPD_SIZE 256
#define SATA_GP3_PAD_CFG0       0x5828
#define I2C3_SCL_PAD_CFG0       0x5438
#define MF_PLT_CLK1_PAD_CFG0    0x4410
#define I2C3_SDA_PAD_CFG0       0x5420

/*
 * 0b0000 - 4GiB total - 2 x 2GiB Samsung K4B4G1646Q-HYK0 1600MHz
 * 0b0001 - 4GiB total - 2 x 2GiB Hynix  H5TC4G63CFR-PBA 1600MHz
 * 0b0010 - 2GiB total - 1 x 2GiB Samsung K4B4G1646Q-HYK0 1600MHz
 * 0b0011 - 2GiB total - 1 x 2GiB Hynix  H5TC4G63CFR-PBA 1600MHz
 */
static const uint32_t dual_channel_config = (1 << 0) | (1 << 1);

static void configure_ramid_gpios(void)
{
	write32((void *)(COMMUNITY_GPSOUTHWEST_BASE + SATA_GP3_PAD_CFG0),
		(PAD_PULL_DISABLE | PAD_GPIO_ENABLE | PAD_CONFIG0_GPI_DEFAULT));
	write32((void *)(COMMUNITY_GPSOUTHEAST_BASE + MF_PLT_CLK1_PAD_CFG0),
		(PAD_PULL_DISABLE | PAD_GPIO_ENABLE | PAD_CONFIG0_GPI_DEFAULT));
}

static void *get_spd_pointer(char *spd_file_content, int total_spds, int *dual)
{
	int ram_id = 0;
	ram_id |= get_gpio(COMMUNITY_GPSOUTHWEST_BASE, SATA_GP3_PAD_CFG0) << 0;
	ram_id |= get_gpio(COMMUNITY_GPSOUTHWEST_BASE, I2C3_SCL_PAD_CFG0) << 1;
	ram_id |= get_gpio(COMMUNITY_GPSOUTHEAST_BASE, MF_PLT_CLK1_PAD_CFG0)
		<< 2;
	ram_id |= get_gpio(COMMUNITY_GPSOUTHWEST_BASE, I2C3_SDA_PAD_CFG0) << 3;
	printk(BIOS_DEBUG, "ram_id=%d, total_spds: %d\n", ram_id, total_spds);
	if (ram_id >= total_spds)
		return NULL;

	/* Determine if this is a single or dual channel memory system */
	if (dual_channel_config & (1 << ram_id))
		*dual = 1;

	/* Display the RAM type */
	switch (ram_id) {
	case 0:
	case 2:
		printk(BIOS_DEBUG, "2GiB Samsung K4B4G1646Q-HYK0 1600MHz\n");
		break;
	case 1:
	case 3:
		printk(BIOS_DEBUG, "2GiB Hynix  H5TC4G63CFR-PBA 1600MHz\n");
		break;
	}

	/* Return the serial product data for the RAM */
	return &spd_file_content[SPD_SIZE * ram_id];
}

/* Copy SPD data for on-board memory */
void mainboard_fill_spd_data(struct pei_data *ps)
{
	char *spd_file;
	size_t spd_file_len;
	void *spd_content;
	int dual_channel = 0;

	/* Find the SPD data in CBFS. */
	spd_file = cbfs_boot_map_with_leak("spd.bin", CBFS_TYPE_SPD,
		&spd_file_len);
	if (!spd_file)
		die("SPD data not found.");

	if (spd_file_len < SPD_SIZE)
		die("Missing SPD data.");

	configure_ramid_gpios();

	/*
	 * Both channels are always present in SPD data. Always use matched
	 * DIMMs so use the same SPD data for each DIMM.
	 */
	spd_content = get_spd_pointer(spd_file,
				      spd_file_len / SPD_SIZE,
				      &dual_channel);
	if (IS_ENABLED(CONFIG_DISPLAY_SPD_DATA) && spd_content != NULL) {
		printk(BIOS_DEBUG, "SPD Data:\n");
		hexdump(spd_content, SPD_SIZE);
		printk(BIOS_DEBUG, "\n");
	}

	/*
	 * Set SPD and memory configuration:
	 * Memory type: 0=DimmInstalled,
	 *              1=SolderDownMemory,
	 *              2=DimmDisabled
	 */
	if (spd_content != NULL) {
		ps->spd_data_ch0 = spd_content;
		ps->spd_ch0_config = 1;
		printk(BIOS_DEBUG, "Channel 0 DIMM soldered down\n");
		if (dual_channel) {
			printk(BIOS_DEBUG, "Channel 1 DIMM soldered down\n");
			ps->spd_data_ch1 = spd_content;
			ps->spd_ch1_config = 1;
		} else {
			printk(BIOS_DEBUG, "Channel 1 DIMM not installed\n");
			ps->spd_ch1_config = 2;
		}
	}
}

static void set_dimm_info(uint32_t chips, uint8_t *spd, struct dimm_info *dimm)
{
	uint16_t clock_frequency;
	uint32_t log2_chips;

	/* Parse the SPD data to determine the DIMM information */
	dimm->ddr_type = MEMORY_TYPE_DDR3;
	dimm->dimm_size = (chips << (spd[4] & 0xf)) << (28 - 3 - 20);  /* MiB */
	clock_frequency = 1000 * spd[11] / (spd[10] * spd[12]);	/* MHz */
	dimm->ddr_frequency = 2 * clock_frequency;	/* Double Data Rate */
	dimm->mod_type = spd[3] & 0xf;
	memcpy((char *)&dimm->module_part_number[0], &spd[0x80],
		sizeof(dimm->module_part_number) - 1);
	dimm->mod_id = *(uint16_t *)&spd[0x94];
	switch (chips) {
	case 1:
		log2_chips = 0;
		break;

	case 2:
		log2_chips = 1;
		break;

	case 4:
		log2_chips = 2;
		break;

	case 8:
		log2_chips = 3;
		break;
	}
	dimm->bus_width = (uint8_t)(log2_chips + (spd[7] & 7) + 2 - 3);
}

void mainboard_save_dimm_info(struct romstage_params *params)
{
	struct dimm_info *dimm;
	struct memory_info *mem_info;
	uint32_t chips;

	/*
	 * Allocate CBMEM area for DIMM information used to populate SMBIOS
	 * table 17
	 */
	mem_info = cbmem_add(CBMEM_ID_MEMINFO, sizeof(*mem_info));
	printk(BIOS_DEBUG, "CBMEM entry for DIMM info: 0x%p\n", mem_info);
	if (mem_info == NULL)
		return;
	memset(mem_info, 0, sizeof(*mem_info));

	/* Describe the first channel memory */
	chips = 4;
	dimm = &mem_info->dimm[0];
	set_dimm_info(chips, params->pei_data->spd_data_ch0, dimm);
	mem_info->dimm_cnt = 1;

	/* Describe the second channel memory */
	if (params->pei_data->spd_ch1_config == 1) {
		dimm = &mem_info->dimm[1];
		set_dimm_info(chips, params->pei_data->spd_data_ch1, dimm);
		dimm->channel_num = 1;
		mem_info->dimm_cnt = 2;
	}
}
