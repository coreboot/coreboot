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

#define MIN_CWL		5
#define MAX_CWL		12

/* Except for tCK, hardware expects all timing values in DCLKs, not nanoseconds */
enum raminit_status convert_timings(struct sysinfo *ctrl)
{
	/*
	 * Obtain all required timing values, in DCLKs.
	 */

	/* Convert primary timings from nanoseconds to DCLKs */
	ctrl->tAA  = DIV_ROUND_UP(ctrl->tAA,  ctrl->tCK);
	ctrl->tWR  = DIV_ROUND_UP(ctrl->tWR,  ctrl->tCK);
	ctrl->tRCD = DIV_ROUND_UP(ctrl->tRCD, ctrl->tCK);
	ctrl->tRRD = DIV_ROUND_UP(ctrl->tRRD, ctrl->tCK);
	ctrl->tRP  = DIV_ROUND_UP(ctrl->tRP,  ctrl->tCK);
	ctrl->tRAS = DIV_ROUND_UP(ctrl->tRAS, ctrl->tCK);
	ctrl->tRC  = DIV_ROUND_UP(ctrl->tRC,  ctrl->tCK);
	ctrl->tRFC = DIV_ROUND_UP(ctrl->tRFC, ctrl->tCK);
	ctrl->tWTR = DIV_ROUND_UP(ctrl->tWTR, ctrl->tCK);
	ctrl->tRTP = DIV_ROUND_UP(ctrl->tRTP, ctrl->tCK);
	ctrl->tFAW = DIV_ROUND_UP(ctrl->tFAW, ctrl->tCK);
	ctrl->tCWL = DIV_ROUND_UP(ctrl->tCWL, ctrl->tCK);
	ctrl->tCMD = DIV_ROUND_UP(ctrl->tCMD, ctrl->tCK);

	/* Constrain primary timings to hardware limits */
	/** TODO: complain when clamping? **/
	ctrl->tAA  = clamp_u32(4,  ctrl->tAA,  24);
	ctrl->tWR  = clamp_u32(5,  ctrl->tWR,  16);
	ctrl->tRCD = clamp_u32(4,  ctrl->tRCD, 20);
	ctrl->tRRD = clamp_u32(4,  ctrl->tRRD, 65535);
	ctrl->tRP  = clamp_u32(4,  ctrl->tRP,  15);
	ctrl->tRAS = clamp_u32(10, ctrl->tRAS, 40);
	ctrl->tRC  = clamp_u32(1,  ctrl->tRC,  4095);
	ctrl->tRFC = clamp_u32(1,  ctrl->tRFC, 511);
	ctrl->tWTR = clamp_u32(4,  ctrl->tWTR, 10);
	ctrl->tRTP = clamp_u32(4,  ctrl->tRTP, 15);
	ctrl->tFAW = clamp_u32(10, ctrl->tFAW, 54);

	/** TODO: Honor tREFI from XMP **/
	ctrl->tREFI = get_tREFI(ctrl->mem_clock_mhz);
	ctrl->tXP   =   get_tXP(ctrl->mem_clock_mhz);

	/*
	 * Check some values, and adjust them if necessary.
	 */

	/* If tWR cannot be written into DDR3 MR0, adjust it */
	switch (ctrl->tWR) {
	case  9:
	case 11:
	case 13:
	case 15:
		ctrl->tWR++;
	}

	/* If tCWL is not supported or unspecified, look up a reasonable default */
	if (ctrl->tCWL < MIN_CWL || ctrl->tCWL > MAX_CWL)
		ctrl->tCWL = get_tCWL(ctrl->mem_clock_mhz);

	/* This is needed to support ODT properly on 2DPC */
	if (ctrl->tAA - ctrl->tCWL > 4)
		ctrl->tCWL = ctrl->tAA - 4;

	/* If tCMD is invalid, use a guesstimate default */
	if (!ctrl->tCMD) {
		ctrl->tCMD = MAX(ctrl->dpc[0], ctrl->dpc[1]);
		printk(RAM_DEBUG, "tCMD was zero, picking a guesstimate value\n");
	}
	ctrl->tCMD = clamp_u32(1, ctrl->tCMD, 3);

	/*
	 * Print final timings.
	 */

	/* tCK is special */
	printk(BIOS_DEBUG, "Selected tCK          : %u ps\n", ctrl->tCK * 1000 / 256);

	/* Primary timings */
	printk(BIOS_DEBUG, "Selected tAA          : %uT\n", ctrl->tAA);
	printk(BIOS_DEBUG, "Selected tWR          : %uT\n", ctrl->tWR);
	printk(BIOS_DEBUG, "Selected tRCD         : %uT\n", ctrl->tRCD);
	printk(BIOS_DEBUG, "Selected tRRD         : %uT\n", ctrl->tRRD);
	printk(BIOS_DEBUG, "Selected tRP          : %uT\n", ctrl->tRP);
	printk(BIOS_DEBUG, "Selected tRAS         : %uT\n", ctrl->tRAS);
	printk(BIOS_DEBUG, "Selected tRC          : %uT\n", ctrl->tRC);
	printk(BIOS_DEBUG, "Selected tRFC         : %uT\n", ctrl->tRFC);
	printk(BIOS_DEBUG, "Selected tWTR         : %uT\n", ctrl->tWTR);
	printk(BIOS_DEBUG, "Selected tRTP         : %uT\n", ctrl->tRTP);
	printk(BIOS_DEBUG, "Selected tFAW         : %uT\n", ctrl->tFAW);
	printk(BIOS_DEBUG, "Selected tCWL         : %uT\n", ctrl->tCWL);
	printk(BIOS_DEBUG, "Selected tCMD         : %uT\n", ctrl->tCMD);

	/* Derived timings */
	printk(BIOS_DEBUG, "Selected tREFI        : %uT\n", ctrl->tREFI);
	printk(BIOS_DEBUG, "Selected tXP          : %uT\n", ctrl->tXP);

	return RAMINIT_STATUS_SUCCESS;
}
