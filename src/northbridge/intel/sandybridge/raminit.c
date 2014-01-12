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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <string.h>
#include <arch/hlt.h>
#include <arch/io.h>
#include <cbmem.h>
#include <arch/cbfs.h>
#include <cbfs.h>
#include <ip_checksum.h>
#include <pc80/mc146818rtc.h>
#include <device/pci_def.h>
#include "raminit.h"
#include "pei_data.h"
#include "sandybridge.h"

/* Management Engine is in the southbridge */
#include "southbridge/intel/bd82x6x/me.h"
#if CONFIG_CHROMEOS
#include <vendorcode/google/chromeos/chromeos.h>
#else
#define recovery_mode_enabled(x) 0
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
#define CMOS_OFFSET_MRC_SEED     152
#define CMOS_OFFSET_MRC_SEED_S3  156
#define CMOS_OFFSET_MRC_SEED_CHK 160
#endif

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

static void prepare_mrc_cache(struct pei_data *pei_data)
{
	struct mrc_data_container *mrc_cache;
	u16 c1, c2, checksum, seed_checksum;

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

	if ((mrc_cache = find_current_mrc_cache()) == NULL) {
		/* error message printed in find_current_mrc_cache */
		return;
	}

	pei_data->mrc_input = mrc_cache->mrc_data;
	pei_data->mrc_input_len = mrc_cache->mrc_data_size;

	printk(BIOS_DEBUG, "%s: at %p, size %x checksum %04x\n",
	       __func__, pei_data->mrc_input,
	       pei_data->mrc_input_len, mrc_cache->mrc_checksum);
}

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

static void post_system_agent_init(struct pei_data *pei_data)
{
	/* If PCIe init is skipped, set the PEG clock gating */
	if (!pei_data->pcie_init)
		MCHBAR32(0x7010) = MCHBAR32(0x7010) | 0x01;
}

/**
 * Find PEI executable in coreboot filesystem and execute it.
 *
 * @param pei_data: configuration data for UEFI PEI reference code
 */
void sdram_initialize(struct pei_data *pei_data)
{
	struct sys_info sysinfo;
	int (*entry) (struct pei_data *pei_data) __attribute__ ((regparm(1)));

	report_platform_info();

	/* Wait for ME to be ready */
	intel_early_me_init();
	intel_early_me_uma_size();

	printk(BIOS_DEBUG, "Starting UEFI PEI System Agent\n");

	memset(&sysinfo, 0, sizeof(sysinfo));

	sysinfo.boot_path = pei_data->boot_mode;

	/*
	 * Do not pass MRC data in for recovery mode boot,
	 * Always pass it in for S3 resume.
	 */
	if (!recovery_mode_enabled() || pei_data->boot_mode == 2)
		prepare_mrc_cache(pei_data);

	/* If MRC data is not found we cannot continue S3 resume. */
	if (pei_data->boot_mode == 2 && !pei_data->mrc_input) {
		printk(BIOS_DEBUG, "Giving up in sdram_initialize: No MRC data\n");
		outb(0x6, 0xcf9);
		hlt();
	}

	/* Pass console handler in pei_data */
	pei_data->tx_byte = console_tx_byte;

	/* Locate and call UEFI System Agent binary. */
	/* TODO make MRC blob (0xab?) defined in cbfs_core.h. */
	entry = cbfs_get_file_content(
		CBFS_DEFAULT_MEDIA, "mrc.bin", 0xab, NULL);
	if (entry) {
		int rv;
		rv = entry (pei_data);
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

#if CONFIG_USBDEBUG_IN_ROMSTAGE
	/* mrc.bin reconfigures USB, so reinit it to have debug */
	usbdebug_init();
#endif

	/* For reference print the System Agent version
	 * after executing the UEFI PEI stage.
	 */
	u32 version = MCHBAR32(0x5034);
	printk(BIOS_DEBUG, "System Agent Version %d.%d.%d Build %d\n",
		version >> 24 , (version >> 16) & 0xff,
		(version >> 8) & 0xff, version & 0xff);

	/* Send ME init done for SandyBridge here.  This is done
	 * inside the SystemAgent binary on IvyBridge. */
	if (BASE_REV_SNB ==
	    (pci_read_config16(PCI_CPU_DEVICE, PCI_DEVICE_ID) & BASE_REV_MASK))
		intel_early_me_init_done(ME_INIT_STATUS_SUCCESS);
	else
		intel_early_me_status();

	post_system_agent_init(pei_data);
	report_memory_config();

	/* S3 resume: don't save scrambler seed or MRC data */
	if (pei_data->boot_mode != 2)
		save_mrc_data(pei_data);
}

unsigned long get_top_of_ram(void)
{
	/* Base of TSEG is top of usable DRAM */
	u32 tom = pci_read_config32(PCI_DEV(0,0,0), TSEG);
	return (unsigned long) tom;
}
