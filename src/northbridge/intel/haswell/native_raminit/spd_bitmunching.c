/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cbfs.h>
#include <commonlib/bsd/clamp.h>
#include <console/console.h>
#include <device/dram/ddr3.h>
#include <device/smbus_host.h>
#include <northbridge/intel/haswell/chip.h>
#include <northbridge/intel/haswell/haswell.h>
#include <northbridge/intel/haswell/raminit.h>
#include <static.h>
#include <string.h>
#include <types.h>

#include "raminit_native.h"

static const uint8_t *get_spd_data_from_cbfs(struct spd_info *spdi)
{
	if (!CONFIG(HAVE_SPD_IN_CBFS))
		return NULL;

	printk(RAM_DEBUG, "SPD index %u\n", spdi->spd_index);

	size_t spd_file_len;
	uint8_t *spd_file = cbfs_map("spd.bin", &spd_file_len);

	if (!spd_file) {
		printk(BIOS_ERR, "SPD data not found in CBFS\n");
		return NULL;
	}

	if (spd_file_len < ((spdi->spd_index + 1) * SPD_SIZE_MAX_DDR3)) {
		printk(BIOS_ERR, "SPD index override to 0 - old hardware?\n");
		spdi->spd_index = 0;
	}

	if (spd_file_len < SPD_SIZE_MAX_DDR3) {
		printk(BIOS_ERR, "Invalid SPD data in CBFS\n");
		return NULL;
	}

	return spd_file + (spdi->spd_index * SPD_SIZE_MAX_DDR3);
}

static void get_spd_for_dimm(struct raminit_dimm_info *const dimm, const uint8_t *cbfs_spd)
{
	if (dimm->spd_addr == SPD_MEMORY_DOWN) {
		if (cbfs_spd) {
			memcpy(dimm->raw_spd, cbfs_spd, SPD_SIZE_MAX_DDR3);
			dimm->valid = true;
			printk(RAM_DEBUG, "memory-down\n");
			return;
		} else {
			printk(RAM_DEBUG, "memory-down but no CBFS SPD data, ignoring\n");
			return;
		}
	}
	printk(RAM_DEBUG, "slotted ");
	const uint8_t spd_mem_type = smbus_read_byte(dimm->spd_addr, SPD_MEMORY_TYPE);
	if (spd_mem_type != SPD_MEMORY_TYPE_SDRAM_DDR3) {
		printk(RAM_DEBUG, "and not DDR3, ignoring\n");
		return;
	}
	printk(RAM_DEBUG, "and DDR3\n");
	if (i2c_eeprom_read(dimm->spd_addr, 0, SPD_SIZE_MAX_DDR3, dimm->raw_spd) != SPD_SIZE_MAX_DDR3) {
		printk(BIOS_WARNING, "I2C block read failed, trying SMBus byte reads\n");
		for (uint32_t i = 0; i < SPD_SIZE_MAX_DDR3; i++)
			dimm->raw_spd[i] = smbus_read_byte(dimm->spd_addr, i);
	}
	dimm->valid = true;
}

static void get_spd_data(struct sysinfo *ctrl)
{
	const struct northbridge_intel_haswell_config *cfg = config_of_soc();
	struct spd_info spdi = {0};
	get_spd_info(&spdi, cfg);
	const uint8_t *cbfs_spd = get_spd_data_from_cbfs(&spdi);
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		for (uint8_t slot = 0; slot < NUM_SLOTS; slot++) {
			struct raminit_dimm_info *const dimm = &ctrl->dimms[channel][slot];
			dimm->spd_addr = spdi.addresses[NUM_SLOTS * channel + slot];
			if (!dimm->spd_addr)
				continue;

			printk(RAM_DEBUG, "CH%uS%u is ", channel, slot);
			get_spd_for_dimm(dimm, cbfs_spd);
		}
	}
}

static void decode_spd(struct raminit_dimm_info *const dimm)
{
	/** TODO: Hook up somewhere, and handle lack of XMP data **/
	const bool enable_xmp = false;
	memset(&dimm->data, 0, sizeof(dimm->data));
	if (enable_xmp)
		spd_xmp_decode_ddr3(&dimm->data, dimm->raw_spd, DDR3_XMP_PROFILE_1);
	else
		spd_decode_ddr3(&dimm->data, dimm->raw_spd);

	if (CONFIG(DEBUG_RAM_SETUP))
		dram_print_spd_ddr3(&dimm->data);
}

static enum raminit_status find_common_spd_parameters(struct sysinfo *ctrl)
{
	ctrl->cas_supported = 0xffff;
	ctrl->flags.raw = 0xffffffff;

	ctrl->tCK  = 0;
	ctrl->tAA  = 0;
	ctrl->tWR  = 0;
	ctrl->tRCD = 0;
	ctrl->tRRD = 0;
	ctrl->tRP  = 0;
	ctrl->tRAS = 0;
	ctrl->tRC  = 0;
	ctrl->tRFC = 0;
	ctrl->tWTR = 0;
	ctrl->tRTP = 0;
	ctrl->tFAW = 0;
	ctrl->tCWL = 0;
	ctrl->tCMD = 0;
	ctrl->chanmap = 0;

	bool yes_ecc = false;
	bool not_ecc = false;

	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		ctrl->dpc[channel] = 0;
		ctrl->rankmap[channel] = 0;
		ctrl->rank_mirrored[channel] = 0;
		ctrl->channel_size_mb[channel] = 0;
		for (uint8_t slot = 0; slot < NUM_SLOTS; slot++) {
			struct raminit_dimm_info *const dimm = &ctrl->dimms[channel][slot];
			if (!dimm->valid)
				continue;

			printk(RAM_DEBUG, "\nCH%uS%u SPD:\n", channel, slot);
			decode_spd(dimm);

			ctrl->chanmap |= BIT(channel);
			ctrl->dpc[channel]++;
			ctrl->channel_size_mb[channel] += dimm->data.size_mb;

			/* The first rank of a populated slot is always present */
			const uint8_t rank = slot + slot;
			assert(dimm->data.ranks);
			ctrl->rankmap[channel] |= (BIT(dimm->data.ranks) - 1) << rank;

			if (dimm->data.flags.pins_mirrored)
				ctrl->rank_mirrored[channel] |= BIT(rank + 1);

			/* Find common settings */
			ctrl->cas_supported &= dimm->data.cas_supported;
			ctrl->flags.raw &= dimm->data.flags.raw;
			ctrl->tCK  = MAX(ctrl->tCK,  dimm->data.tCK);
			ctrl->tAA  = MAX(ctrl->tAA,  dimm->data.tAA);
			ctrl->tWR  = MAX(ctrl->tWR,  dimm->data.tWR);
			ctrl->tRCD = MAX(ctrl->tRCD, dimm->data.tRCD);
			ctrl->tRRD = MAX(ctrl->tRRD, dimm->data.tRRD);
			ctrl->tRP  = MAX(ctrl->tRP,  dimm->data.tRP);
			ctrl->tRAS = MAX(ctrl->tRAS, dimm->data.tRAS);
			ctrl->tRC  = MAX(ctrl->tRC,  dimm->data.tRC);
			ctrl->tRFC = MAX(ctrl->tRFC, dimm->data.tRFC);
			ctrl->tWTR = MAX(ctrl->tWTR, dimm->data.tWTR);
			ctrl->tRTP = MAX(ctrl->tRTP, dimm->data.tRTP);
			ctrl->tFAW = MAX(ctrl->tFAW, dimm->data.tFAW);
			ctrl->tCWL = MAX(ctrl->tCWL, dimm->data.tCWL);
			ctrl->tCMD = MAX(ctrl->tCMD, dimm->data.tCMD);

			yes_ecc |=  dimm->data.flags.is_ecc;
			not_ecc |= !dimm->data.flags.is_ecc;
		}
	}

	if (!ctrl->chanmap) {
		printk(BIOS_ERR, "No DIMMs were found\n");
		return RAMINIT_STATUS_NO_MEMORY_INSTALLED;
	}
	if (!ctrl->cas_supported) {
		printk(BIOS_ERR, "Could not resolve common CAS latency\n");
		return RAMINIT_STATUS_UNSUPPORTED_MEMORY;
	}
	/** TODO: Properly handle ECC support and ECC forced **/
	if (yes_ecc && not_ecc) {
		/** TODO: Test if the ECC DIMMs can be operated as non-ECC DIMMs **/
		printk(BIOS_ERR, "Both ECC and non-ECC DIMMs present, this is unsupported\n");
		return RAMINIT_STATUS_UNSUPPORTED_MEMORY;
	}
	if (yes_ecc)
		ctrl->lanes = NUM_LANES;
	else
		ctrl->lanes = NUM_LANES_NO_ECC;

	ctrl->is_ecc = yes_ecc;

	/** TODO: Complete LPDDR support **/
	ctrl->lpddr = false;

	return RAMINIT_STATUS_SUCCESS;
}

enum raminit_status collect_spd_info(struct sysinfo *ctrl)
{
	get_spd_data(ctrl);
	return find_common_spd_parameters(ctrl);
}
