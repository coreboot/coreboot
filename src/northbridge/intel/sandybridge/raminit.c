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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <string.h>
#include <arch/hlt.h>
#include <arch/io.h>
#include <arch/romcc_io.h>
#include <cbmem.h>
#include <arch/cbfs.h>
#include <cbfs.h>
#include <ip_checksum.h>
#include <pc80/mc146818rtc.h>
#include "raminit.h"
#include "pei_data.h"
#include "sandybridge.h"

/* Management Engine is in the southbridge */
#include "southbridge/intel/bd82x6x/me.h"
#if CONFIG_CHROMEOS
#include <vendorcode/google/chromeos/chromeos.h>
#endif
#if 0
#include <fdt/libfdt.h>
#endif

/*
 * MRC scrambler seed offsets should be reserved in
 * mainboard cmos.layout and not covered by checksum.
 */
#if CONFIG_USE_OPTION_TABLE
#include "option_table.h"
#define CMOS_OFFSET_MRC_SEED     (CMOS_VSTART_mrc_scrambler_seed >> 3)
#define CMOS_OFFSET_MRC_SEED_S3  (CMOS_VSTART_mrc_scrambler_seed_s3 >> 3)
#define CMOS_OFFSET_MRC_SEED_CHK (CMOS_VSTART_mrc_scrambler_seed_chk >> 3)
#else
#define CMOS_OFFSET_MRC_SEED     112
#define CMOS_OFFSET_MRC_SEED_S3  116
#define CMOS_OFFSET_MRC_SEED_CHK 120
#endif

#define MRC_DATA_ALIGN           0x1000
#define MRC_DATA_SIGNATURE       (('M'<<0)|('R'<<8)|('C'<<16)|('D'<<24))

struct mrc_data_container {
	u32	mrc_signature;	// "MRCD"
	u32	mrc_data_size;	// Actual total size of this structure
	u32	mrc_checksum;	// IP style checksum
	u32	reserved;	// For header alignment
	u8	mrc_data[0];	// Variable size, platform/run time dependent.
} __attribute__ ((packed));

static void save_mrc_data(struct pei_data *pei_data)
{
	u16 c1, c2, checksum;

#if CONFIG_EARLY_CBMEM_INIT
	struct mrc_data_container *mrcdata;
	int output_len = ALIGN(pei_data->mrc_output_len, 16);

	/* Save the MRC S3 restore data to cbmem */
	cbmem_initialize();
	mrcdata = cbmem_add
		(CBMEM_ID_MRCDATA,
		 output_len + sizeof(struct mrc_data_container));

	printk(BIOS_DEBUG, "Relocate MRC DATA from %p to %p (%u bytes)\n",
	       pei_data->mrc_output, mrcdata, output_len);

	mrcdata->mrc_signature = MRC_DATA_SIGNATURE;
	mrcdata->mrc_data_size = output_len;
	mrcdata->reserved = 0;
	memcpy(mrcdata->mrc_data, pei_data->mrc_output,
	       pei_data->mrc_output_len);

	/* Zero the unused space in aligned buffer. */
	if (output_len > pei_data->mrc_output_len)
		memset(mrcdata->mrc_data+pei_data->mrc_output_len, 0,
		       output_len - pei_data->mrc_output_len);

	mrcdata->mrc_checksum = compute_ip_checksum(mrcdata->mrc_data,
						    mrcdata->mrc_data_size);
#endif

	/* Save the MRC seed values to CMOS */
	cmos_write32(CMOS_OFFSET_MRC_SEED, pei_data->scrambler_seed);
	printk(BIOS_DEBUG, "Save scrambler seed    0x%08x to CMOS 0x%02x\n",
	       pei_data->scrambler_seed, CMOS_OFFSET_MRC_SEED);

	cmos_write32(CMOS_OFFSET_MRC_SEED_S3, pei_data->scrambler_seed_s3);
	printk(BIOS_DEBUG, "Save s3 scrambler seed 0x%08x to CMOS 0x%02x\n",
	       pei_data->scrambler_seed_s3, CMOS_OFFSET_MRC_SEED_S3);

	/* Save a simple checksum of the seed values */
	c1 = compute_ip_checksum((u8*)&pei_data->scrambler_seed,
				 sizeof(u32));
	c2 = compute_ip_checksum((u8*)&pei_data->scrambler_seed_s3,
				 sizeof(u32));
	checksum = add_ip_checksums(sizeof(u32), c1, c2);

	cmos_write(checksum & 0xff, CMOS_OFFSET_MRC_SEED_CHK);
	cmos_write((checksum >> 8) & 0xff, CMOS_OFFSET_MRC_SEED_CHK+1);
}

#if CONFIG_CHROMEOS
static void prepare_mrc_cache(struct pei_data *pei_data)
{
#if 0
	const struct fdt_header *fdt_header;
	const struct fdt_property *fdtp;
	int offset, len;
	const char *compatible = "chromeos,flashmap";
	const char *subnode = "rw-mrc-cache";
	const char *property = "reg";
	u32 *data;
	struct mrc_data_container *mrc_cache, *mrc_next;
	u8 *mrc_region, *region_ptr;
	u16 c1, c2, checksum, seed_checksum;
	u32 region_size, entry_id = 0;
	u64 flashrom_base = 0;

	// preset just in case there is an error
	pei_data->mrc_input = NULL;
	pei_data->mrc_input_len = 0;

	/* Read scrambler seeds from CMOS */
	pei_data->scrambler_seed = cmos_read32(CMOS_OFFSET_MRC_SEED);
	printk(BIOS_DEBUG, "Read scrambler seed    0x%08x from CMOS 0x%02x\n",
	       pei_data->scrambler_seed, CMOS_OFFSET_MRC_SEED);

	pei_data->scrambler_seed_s3 = cmos_read32(CMOS_OFFSET_MRC_SEED_S3);
	printk(BIOS_DEBUG, "Read S3 scrambler seed 0x%08x from CMOS 0x%02x\n",
	       pei_data->scrambler_seed_s3, CMOS_OFFSET_MRC_SEED_S3);

	/* Compute seed checksum and compare */
	c1 = compute_ip_checksum((u8*)&pei_data->scrambler_seed,
				 sizeof(u32));
	c2 = compute_ip_checksum((u8*)&pei_data->scrambler_seed_s3,
				 sizeof(u32));
	checksum = add_ip_checksums(sizeof(u32), c1, c2);

	seed_checksum = cmos_read(CMOS_OFFSET_MRC_SEED_CHK);
	seed_checksum |= cmos_read(CMOS_OFFSET_MRC_SEED_CHK+1) << 8;

	if (checksum != seed_checksum) {
		printk(BIOS_ERR, "%s: invalid seed checksum\n", __func__);
		pei_data->scrambler_seed = 0;
		pei_data->scrambler_seed_s3 = 0;
		return;
	}

	fdt_header = cbfs_find_file(CONFIG_FDT_FILE_NAME, CBFS_TYPE_FDT);

	if (!fdt_header) {
		printk(BIOS_ERR, "%s: no FDT found!\n", __func__);
		return;
	}

	offset = fdt_node_offset_by_compatible(fdt_header, 0, compatible);
	if (offset < 0) {
		printk(BIOS_ERR, "%s: no %s  node found!\n",
		       __func__, compatible);
		return;
	}

	if (fdt_get_base_addr(fdt_header, offset, &flashrom_base) < 0) {
		printk(BIOS_ERR, "%s: no base address in node name!\n",
		       __func__);
		return;
	}

	offset = fdt_subnode_offset(fdt_header, offset, subnode);
	if (offset < 0) {
		printk(BIOS_ERR, "%s: no %s found!\n", __func__, subnode);
		return;
	}

	fdtp = fdt_get_property(fdt_header, offset, property, &len);
	if (!fdtp || (len != 8)) {
		printk(BIOS_ERR, "%s: property %s at %p, len %d!\n",
		       __func__, property, fdtp, len);
		return;
	}

	data = (u32 *)fdtp->data;

	// Calculate actual address of the MRC cache in memory
	region_size = fdt32_to_cpu(data[1]);
	mrc_region = region_ptr = (u8*)
		((unsigned long)flashrom_base + fdt32_to_cpu(data[0]));
	mrc_cache = mrc_next = (struct mrc_data_container *)mrc_region;

	if (!mrc_cache || mrc_cache->mrc_signature != MRC_DATA_SIGNATURE) {
		printk(BIOS_ERR, "%s: invalid MRC data\n", __func__);
		return;
	}

	if (mrc_cache->mrc_data_size == -1UL) {
		printk(BIOS_ERR, "%s: MRC cache not initialized?\n",  __func__);
		return;
	} else {
		/* MRC data blocks are aligned within the region */
		u32 mrc_size = sizeof(*mrc_cache) + mrc_cache->mrc_data_size;
		if (mrc_size & (MRC_DATA_ALIGN - 1UL)) {
			mrc_size &= ~(MRC_DATA_ALIGN - 1UL);
			mrc_size += MRC_DATA_ALIGN;
		}

		/* Search for the last filled entry in the region */
		while (mrc_next &&
		       mrc_next->mrc_signature == MRC_DATA_SIGNATURE) {
			entry_id++;
			mrc_cache = mrc_next;
			/* Stay in the mrcdata region defined in fdt */
			if ((entry_id * mrc_size) > region_size)
				break;
			region_ptr += mrc_size;
			mrc_next = (struct mrc_data_container *)region_ptr;
		}
		entry_id--;
	}

	/* Verify checksum */
	if (mrc_cache->mrc_checksum !=
	    compute_ip_checksum(mrc_cache->mrc_data,
				mrc_cache->mrc_data_size)) {
		printk(BIOS_ERR, "%s: MRC cache checksum mismatch\n", __func__);
		return;
	}

	pei_data->mrc_input = mrc_cache->mrc_data;
	pei_data->mrc_input_len = mrc_cache->mrc_data_size;

	printk(BIOS_DEBUG, "%s: at %p, entry %u size %x checksum %04x\n",
	       __func__, pei_data->mrc_input, entry_id,
	       pei_data->mrc_input_len, mrc_cache->mrc_checksum);
#else
	printk(BIOS_ERR, "MRC cache handling code has to be redone.");
#endif
}
#endif

static const char* ecc_decoder[] = {
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
		printk(BIOS_DEBUG, "   DIMMA %d MB width x%d %s rank%s\n",
		       ((ch_conf >> 0) & 0xff) * 256,
		       ((ch_conf >> 19) & 1) ? 16 : 8,
		       ((ch_conf >> 17) & 1) ? "dual" : "single",
		       ((ch_conf >> 16) & 1) ? "" : ", selected");
		printk(BIOS_DEBUG, "   DIMMB %d MB width x%d %s rank%s\n",
		       ((ch_conf >> 8) & 0xff) * 256,
		       ((ch_conf >> 20) & 1) ? 16 : 8,
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
	struct sys_info sysinfo;
	const char *target = "mrc.bin";
	unsigned long entry;

	/* Wait for ME to be ready */
	intel_early_me_init();
	intel_early_me_uma_size();

	printk(BIOS_DEBUG, "Starting UEFI PEI System Agent\n");

	memset(&sysinfo, 0, sizeof(sysinfo));

	sysinfo.boot_path = pei_data->boot_mode;

#if CONFIG_CHROMEOS
	/*
	 * Do not pass MRC data in for recovery mode boot,
	 * Always pass it in for S3 resume.
	 */
	if (!recovery_mode_enabled() || pei_data->boot_mode == 2)
		prepare_mrc_cache(pei_data);

	/* If MRC data is not found we cannot continue S3 resume. */
	if (pei_data->boot_mode == 2 && !pei_data->mrc_input) {
		outb(0x6, 0xcf9);
		hlt();
	}
#endif

	/* Locate and call UEFI System Agent binary. */
	entry = (unsigned long)cbfs_find_file(target, 0xab);
	if (entry) {
		int rv;
		asm volatile (
			      "call *%%ecx\n\t"
			      :"=a" (rv) : "c" (entry), "a" (pei_data));
		if (rv) {
			printk(BIOS_ERR, "MRC returned %d\n", rv);
			die("Nonzero MRC return value\n");
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

	intel_early_me_init_done(ME_INIT_STATUS_SUCCESS);

	report_memory_config();

	/* S3 resume: don't save scrambler seed or MRC data */
	if (pei_data->boot_mode != 2)
		save_mrc_data(pei_data);
}

struct cbmem_entry *get_cbmem_toc(void)
{
	return (struct cbmem_entry *)(get_top_of_ram() - HIGH_MEMORY_SIZE);
}

unsigned long get_top_of_ram(void)
{
	/* Base of TSEG is top of usable DRAM */
	u32 tom = pci_read_config32(PCI_DEV(0,0,0), TSEG);
	return (unsigned long) tom;
}
