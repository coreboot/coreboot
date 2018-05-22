/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Google Inc.
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

#include <console/console.h>
#include <string.h>
#include <arch/io.h>
#include <cbmem.h>
#include <arch/cbfs.h>
#include <cbfs.h>
#include <halt.h>
#include <ip_checksum.h>
#include <memory_info.h>
#include <mrc_cache.h>
#include <pc80/mc146818rtc.h>
#include <device/pci_def.h>
#include <device/dram/ddr3.h>
#include <smbios.h>
#include <spd.h>
#include <security/vboot/vboot_common.h>
#include <commonlib/region.h>
#include "raminit.h"
#include "pei_data.h"
#include "haswell.h"

#define MRC_CACHE_VERSION 1

void save_mrc_data(struct pei_data *pei_data)
{
	/* Save the MRC S3 restore data to cbmem */
	mrc_cache_stash_data(MRC_TRAINING_DATA, MRC_CACHE_VERSION,
			pei_data->mrc_output, pei_data->mrc_output_len);
}

static void prepare_mrc_cache(struct pei_data *pei_data)
{
	struct region_device rdev;

	// preset just in case there is an error
	pei_data->mrc_input = NULL;
	pei_data->mrc_input_len = 0;

	if (mrc_cache_get_current(MRC_TRAINING_DATA, MRC_CACHE_VERSION, &rdev))
		/* error message printed in find_current_mrc_cache */
		return;

	pei_data->mrc_input = rdev_mmap_full(&rdev);
	pei_data->mrc_input_len = region_device_sz(&rdev);

	printk(BIOS_DEBUG, "%s: at %p, size %x\n",
	       __func__, pei_data->mrc_input, pei_data->mrc_input_len);
}

static const char *ecc_decoder[] = {
	"inactive",
	"active on IO",
	"disabled on IO",
	"active"
};

/*
 * Dump in the log memory controller configuration as read from the memory
 * controller registers.
 */
static void report_memory_config(void)
{
	u32 addr_decoder_common, addr_decode_ch[2];
	int i;

	addr_decoder_common = MCHBAR32(0x5000);
	addr_decode_ch[0] = MCHBAR32(0x5004);
	addr_decode_ch[1] = MCHBAR32(0x5008);

	printk(BIOS_DEBUG, "memcfg DDR3 clock %d MHz\n",
	       (MCHBAR32(0x5e04) * 13333 * 2 + 50)/100);
	printk(BIOS_DEBUG, "memcfg channel assignment: A: %d, B % d, C % d\n",
	       addr_decoder_common & 3,
	       (addr_decoder_common >> 2) & 3,
	       (addr_decoder_common >> 4) & 3);

	for (i = 0; i < ARRAY_SIZE(addr_decode_ch); i++) {
		u32 ch_conf = addr_decode_ch[i];
		printk(BIOS_DEBUG, "memcfg channel[%d] config (%8.8x):\n",
		       i, ch_conf);
		printk(BIOS_DEBUG, "   ECC %s\n",
		       ecc_decoder[(ch_conf >> 24) & 3]);
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

/**
 * Find PEI executable in coreboot filesystem and execute it.
 *
 * @param pei_data: configuration data for UEFI PEI reference code
 */
void sdram_initialize(struct pei_data *pei_data)
{
	unsigned long entry;

	printk(BIOS_DEBUG, "Starting UEFI PEI System Agent\n");

	/*
	 * Do not pass MRC data in for recovery mode boot,
	 * Always pass it in for S3 resume.
	 */
	if (!vboot_recovery_mode_enabled() || pei_data->boot_mode == 2)
		prepare_mrc_cache(pei_data);

	/* If MRC data is not found we cannot continue S3 resume. */
	if (pei_data->boot_mode == 2 && !pei_data->mrc_input) {
		post_code(POST_RESUME_FAILURE);
		printk(BIOS_DEBUG, "Giving up in sdram_initialize: "
		       "No MRC data\n");
		outb(0x6, 0xcf9);
		halt();
	}

	/* Pass console handler in pei_data */
	pei_data->tx_byte = do_putchar;

	/* Locate and call UEFI System Agent binary. */
	entry = (unsigned long)cbfs_boot_map_with_leak("mrc.bin",
							CBFS_TYPE_MRC, NULL);
	if (entry) {
		int rv;
		asm volatile (
			      "call *%%ecx\n\t"
			      :"=a" (rv) : "c" (entry), "a" (pei_data));
		if (rv) {
			switch (rv) {
			case -1:
				printk(BIOS_ERR, "PEI version mismatch.\n");
				break;
			case -2:
				printk(BIOS_ERR, "Invalid memory frequency.\n");
				break;
			default:
				printk(BIOS_ERR, "MRC returned %x.\n", rv);
			}
			die("Nonzero MRC return value.\n");
		}
	} else {
		die("UEFI PEI System Agent not found.\n");
	}

	/* For reference print the System Agent version
	 * after executing the UEFI PEI stage.
	 */
	u32 version = MCHBAR32(0x5034);
	printk(BIOS_DEBUG, "System Agent Version %d.%d.%d Build %d\n",
		version >> 24 , (version >> 16) & 0xff,
		(version >> 8) & 0xff, version & 0xff);

	report_memory_config();
}

void setup_sdram_meminfo(struct pei_data *pei_data)
{
	u32 addr_decoder_common, addr_decode_ch[2];
	struct memory_info* mem_info;
	struct dimm_info *dimm;
	int ddr_frequency;
	int dimm_size;
	int ch, d_num;
	int dimm_cnt = 0;

	mem_info = cbmem_add(CBMEM_ID_MEMINFO, sizeof(struct memory_info));
	if (!mem_info)
		die("Failed to add memory info to CBMEM.\n");
	memset(mem_info, 0, sizeof(struct memory_info));

	addr_decoder_common = MCHBAR32(0x5000);
	addr_decode_ch[0] = MCHBAR32(0x5004);
	addr_decode_ch[1] = MCHBAR32(0x5008);

	ddr_frequency = (MCHBAR32(0x5e04) * 13333 * 2 + 50) / 100;

	for (ch = 0; ch < ARRAY_SIZE(addr_decode_ch); ch++) {
		u32 ch_conf = addr_decode_ch[ch];
		/* DIMMs A/B */
		for (d_num = 0; d_num < 2; d_num++) {
			dimm_size = ((ch_conf >> (d_num * 8)) & 0xff) * 256;
			if (dimm_size) {
				dimm = &mem_info->dimm[dimm_cnt];
				dimm->dimm_size = dimm_size;
				dimm->ddr_type = MEMORY_TYPE_DDR3;
				dimm->ddr_frequency = ddr_frequency;
				dimm->rank_per_dimm = 1 + ((ch_conf >> (17 + d_num)) & 1);
				dimm->channel_num = ch;
				dimm->dimm_num = d_num;
				dimm->bank_locator = ch * 2;
				memcpy(dimm->serial,
					&pei_data->spd_data[dimm_cnt][SPD_DIMM_SERIAL_NUM],
					SPD_DIMM_SERIAL_LEN);
				memcpy(dimm->module_part_number,
					&pei_data->spd_data[dimm_cnt][SPD_DIMM_PART_NUM],
					SPD_DIMM_PART_LEN);
				dimm->mod_id =
					(pei_data->spd_data[dimm_cnt][SPD_DIMM_MOD_ID2] << 8) |
					(pei_data->spd_data[dimm_cnt][SPD_DIMM_MOD_ID1] & 0xFF);
				dimm->mod_type = SPD_SODIMM;
				dimm->bus_width = 0x3;	/* 64-bit */
				dimm_cnt++;
			}
		}
	}
	mem_info->dimm_cnt = dimm_cnt;
}
