/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <console/usb.h>
#include <string.h>
#include <cbmem.h>
#include <cbfs.h>
#include <cf9_reset.h>
#include <ip_checksum.h>
#include <memory_info.h>
#include <mrc_cache.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
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
	mrc_cache_stash_data(MRC_TRAINING_DATA, MRC_CACHE_VERSION, pei_data->mrc_output,
			     pei_data->mrc_output_len);
}

static void prepare_mrc_cache(struct pei_data *pei_data)
{
	size_t mrc_size;

	/* Preset just in case there is an error */
	pei_data->mrc_input = NULL;
	pei_data->mrc_input_len = 0;

	pei_data->mrc_input =
		mrc_cache_current_mmap_leak(MRC_TRAINING_DATA,
					    MRC_CACHE_VERSION,
					    &mrc_size);
	if (!pei_data->mrc_input)
		/* Error message printed in find_current_mrc_cache */
		return;

	pei_data->mrc_input_len = mrc_size;

	printk(BIOS_DEBUG, "%s: at %p, size %zx\n", __func__,
	       pei_data->mrc_input, mrc_size);
}

static const char *const ecc_decoder[] = {
	"inactive",
	"active on IO",
	"disabled on IO",
	"active",
};

/* Print out the memory controller configuration, as per the values in its registers. */
static void report_memory_config(void)
{
	int i;

	const u32 addr_decoder_common = MCHBAR32(MAD_CHNL);

	printk(BIOS_DEBUG, "memcfg DDR3 clock %d MHz\n",
	       (MCHBAR32(MC_BIOS_DATA) * 13333 * 2 + 50) / 100);

	printk(BIOS_DEBUG, "memcfg channel assignment: A: %d, B % d, C % d\n",
	       (addr_decoder_common >> 0) & 3,
	       (addr_decoder_common >> 2) & 3,
	       (addr_decoder_common >> 4) & 3);

	for (i = 0; i < NUM_CHANNELS; i++) {
		const u32 ch_conf = MCHBAR32(MAD_DIMM(i));

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

/**
 * Find PEI executable in coreboot filesystem and execute it.
 *
 * @param pei_data: configuration data for UEFI PEI reference code
 */
void sdram_initialize(struct pei_data *pei_data)
{
	int (*entry)(struct pei_data *pei_data) __attribute__((regparm(1)));

	uint32_t type = CBFS_TYPE_MRC;
	struct cbfsf f;

	printk(BIOS_DEBUG, "Starting UEFI PEI System Agent\n");

	/* Do not pass MRC data in for recovery mode boot, always pass it in for S3 resume */
	if (!(CONFIG(HASWELL_VBOOT_IN_BOOTBLOCK) && vboot_recovery_mode_enabled())
	    || pei_data->boot_mode == 2)
		prepare_mrc_cache(pei_data);

	/* If MRC data is not found, we cannot continue S3 resume */
	if (pei_data->boot_mode == 2 && !pei_data->mrc_input) {
		post_code(POST_RESUME_FAILURE);
		printk(BIOS_DEBUG, "Giving up in %s: No MRC data\n", __func__);
		system_reset();
	}

	/* Pass console handler in pei_data */
	pei_data->tx_byte = do_putchar;

	/*
	 * Locate and call UEFI System Agent binary. The binary needs to be at a fixed offset
	 * in the flash and can therefore only reside in the COREBOOT fmap region.
	 */
	if (cbfs_locate_file_in_region(&f, "COREBOOT", "mrc.bin", &type) < 0)
		die("mrc.bin not found!");

	/* We don't care about leaking the mapping */
	entry = rdev_mmap_full(&f.data);
	if (entry) {
		int rv = entry(pei_data);

		/* The mrc.bin reconfigures USB, so usbdebug needs to be reinitialized */
		if (CONFIG(USBDEBUG_IN_PRE_RAM))
			usbdebug_hw_init(true);

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
			die_with_post_code(POST_INVALID_VENDOR_BINARY,
					   "Nonzero MRC return value.\n");
		}
	} else {
		die("UEFI PEI System Agent not found.\n");
	}

	/* Print the MRC version after executing the UEFI PEI stage */
	u32 version = MCHBAR32(MRC_REVISION);
	printk(BIOS_DEBUG, "MRC Version %d.%d.%d Build %d\n",
		(version >> 24) & 0xff, (version >> 16) & 0xff,
		(version >>  8) & 0xff, (version >>  0) & 0xff);

	report_memory_config();
}

static bool nb_supports_ecc(const uint32_t capid0_a)
{
	return !(capid0_a & CAPID_ECCDIS);
}

static uint16_t nb_slots_per_channel(const uint32_t capid0_a)
{
	return !(capid0_a & CAPID_DDPCD) + 1;
}

static uint16_t nb_number_of_channels(const uint32_t capid0_a)
{
	return !(capid0_a & CAPID_PDCD) + 1;
}

static uint32_t nb_max_chan_capacity_mib(const uint32_t capid0_a)
{
	uint32_t ddrsz;

	/* Values from documentation, which assume two DIMMs per channel */
	switch (CAPID_DDRSZ(capid0_a)) {
	case 1:
		ddrsz = 8192;
		break;
	case 2:
		ddrsz = 2048;
		break;
	case 3:
		ddrsz = 512;
		break;
	default:
		ddrsz = 16384;
		break;
	}

	/* Account for the maximum number of DIMMs per channel */
	return (ddrsz / 2) * nb_slots_per_channel(capid0_a);
}

void setup_sdram_meminfo(struct pei_data *pei_data)
{
	struct memory_info *mem_info;
	struct dimm_info *dimm;
	int ch, d_num;
	int dimm_cnt = 0;

	mem_info = cbmem_add(CBMEM_ID_MEMINFO, sizeof(struct memory_info));
	if (!mem_info)
		die("Failed to add memory info to CBMEM.\n");

	memset(mem_info, 0, sizeof(struct memory_info));

	const u32 ddr_frequency = (MCHBAR32(MC_BIOS_DATA) * 13333 * 2 + 50) / 100;

	for (ch = 0; ch < NUM_CHANNELS; ch++) {
		const u32 ch_conf = MCHBAR32(MAD_DIMM(ch));
		/* DIMMs A/B */
		for (d_num = 0; d_num < NUM_SLOTS; d_num++) {
			const u32 dimm_size = ((ch_conf >> (d_num * 8)) & 0xff) * 256;
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
					(pei_data->spd_data[dimm_cnt][SPD_DIMM_MOD_ID1] & 0xff);
				dimm->mod_type = SPD_SODIMM;
				dimm->bus_width = MEMORY_BUS_WIDTH_64;
				dimm_cnt++;
			}
		}
	}
	mem_info->dimm_cnt = dimm_cnt;

	const uint32_t capid0_a = pci_read_config32(HOST_BRIDGE, CAPID0_A);

	const uint16_t channels = nb_number_of_channels(capid0_a);

	mem_info->ecc_capable = nb_supports_ecc(capid0_a);
	mem_info->max_capacity_mib = channels * nb_max_chan_capacity_mib(capid0_a);
	mem_info->number_of_devices = channels * nb_slots_per_channel(capid0_a);
}
