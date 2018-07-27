/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Damien Zammit <damien@zamaudio.com>
 * Copyright (C) 2014 Vladimir Serbinenko <phcoder@gmail.com>
 * Copyright (C) 2016 Patrick Rudolph <siro@das-labor.org>
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
#include <console/usb.h>
#include <commonlib/region.h>
#include <bootmode.h>
#include <string.h>
#include <arch/io.h>
#include <cbmem.h>
#include <halt.h>
#include <timestamp.h>
#include <mrc_cache.h>
#include <southbridge/intel/bd82x6x/me.h>
#include <southbridge/intel/common/smbus.h>
#include <cpu/x86/msr.h>
#include <delay.h>
#include <smbios.h>
#include <memory_info.h>
#include <lib.h>
#include "raminit_native.h"
#include "raminit_common.h"
#include "sandybridge.h"

#define MRC_CACHE_VERSION 1

/* FIXME: no ECC support.  */
/* FIXME: no support for 3-channel chipsets.  */

static const char *ecc_decoder[] = {
	"inactive",
	"active on IO",
	"disabled on IO",
	"active"
};

static void wait_txt_clear(void)
{
	struct cpuid_result cp;

	cp = cpuid_ext(0x1, 0x0);
	/* Check if TXT is supported?  */
	if (!(cp.ecx & 0x40))
		return;
	/* Some TXT public bit.  */
	if (!(read32((void *)0xfed30010) & 1))
		return;
	/* Wait for TXT clear.  */
	while (!(read8((void *)0xfed40000) & (1 << 7)));
}

/*
 * Disable a channel in ramctr_timing.
 */
static void disable_channel(ramctr_timing *ctrl, int channel) {
	ctrl->rankmap[channel] = 0;
	memset(&ctrl->rank_mirror[channel][0], 0, sizeof(ctrl->rank_mirror[0]));
	ctrl->channel_size_mb[channel] = 0;
	ctrl->cmd_stretch[channel] = 0;
	ctrl->mad_dimm[channel] = 0;
	memset(&ctrl->timings[channel][0], 0, sizeof(ctrl->timings[0]));
	memset(&ctrl->info.dimm[channel][0], 0, sizeof(ctrl->info.dimm[0]));
}

/*
 * Fill cbmem with information for SMBIOS type 17.
 */
static void fill_smbios17(ramctr_timing *ctrl)
{
	struct memory_info *mem_info;
	int channel, slot;
	struct dimm_info *dimm;
	uint16_t ddr_freq;
	dimm_info *info = &ctrl->info;

	ddr_freq = (1000 << 8) / ctrl->tCK;

	/*
	 * Allocate CBMEM area for DIMM information used to populate SMBIOS
	 * table 17
	 */
	mem_info = cbmem_add(CBMEM_ID_MEMINFO, sizeof(*mem_info));
	printk(BIOS_DEBUG, "CBMEM entry for DIMM info: 0x%p\n", mem_info);
	if (!mem_info)
		return;

	memset(mem_info, 0, sizeof(*mem_info));

	FOR_ALL_CHANNELS for (slot = 0; slot < NUM_SLOTS; slot++) {
		dimm = &mem_info->dimm[mem_info->dimm_cnt];
		if (info->dimm[channel][slot].size_mb) {
			dimm->ddr_type = MEMORY_TYPE_DDR3;
			dimm->ddr_frequency = ddr_freq;
			dimm->dimm_size = info->dimm[channel][slot].size_mb;
			dimm->channel_num = channel;
			dimm->rank_per_dimm = info->dimm[channel][slot].ranks;
			dimm->dimm_num = slot;
			memcpy(dimm->module_part_number,
				   info->dimm[channel][slot].part_number, 16);
			dimm->mod_id = info->dimm[channel][slot].manufacturer_id;
			dimm->mod_type = info->dimm[channel][slot].dimm_type;
			dimm->bus_width = MEMORY_BUS_WIDTH_64; // non-ECC only
			mem_info->dimm_cnt++;
		}
	}
}

/*
 * Dump in the log memory controller configuration as read from the memory
 * controller registers.
 */
static void report_memory_config(void)
{
	u32 addr_decoder_common, addr_decode_ch[NUM_CHANNELS];
	int i, refclk;

	addr_decoder_common = MCHBAR32(0x5000);
	addr_decode_ch[0] = MCHBAR32(0x5004);
	addr_decode_ch[1] = MCHBAR32(0x5008);

	refclk = MCHBAR32(MC_BIOS_REQ) & 0x100 ? 100 : 133;

	printk(BIOS_DEBUG, "memcfg DDR3 ref clock %d MHz\n", refclk);
	printk(BIOS_DEBUG, "memcfg DDR3 clock %d MHz\n",
	       (MCHBAR32(MC_BIOS_DATA) * refclk * 100 * 2 + 50) / 100);
	printk(BIOS_DEBUG, "memcfg channel assignment: A: %d, B % d, C % d\n",
	       addr_decoder_common & 3, (addr_decoder_common >> 2) & 3,
	       (addr_decoder_common >> 4) & 3);

	for (i = 0; i < ARRAY_SIZE(addr_decode_ch); i++) {
		u32 ch_conf = addr_decode_ch[i];
		printk(BIOS_DEBUG, "memcfg channel[%d] config (%8.8x):\n", i,
		       ch_conf);
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

/*
 * Return CRC16 match for all SPDs.
 */
static int verify_crc16_spds_ddr3(spd_raw_data *spd, ramctr_timing *ctrl)
{
	int channel, slot, spd_slot;
	int match = 1;

	FOR_ALL_CHANNELS {
		for (slot = 0; slot < NUM_SLOTS; slot++) {
			spd_slot = 2 * channel + slot;
			match &= ctrl->spd_crc[channel][slot] ==
					spd_ddr3_calc_unique_crc(spd[spd_slot], sizeof(spd_raw_data));
		}
	}
	return match;
}

void read_spd(spd_raw_data * spd, u8 addr, bool id_only)
{
	int j;
	if (id_only) {
		for (j = 117; j < 128; j++)
			(*spd)[j] = do_smbus_read_byte(SMBUS_IO_BASE, addr, j);
	} else {
		for (j = 0; j < 256; j++)
			(*spd)[j] = do_smbus_read_byte(SMBUS_IO_BASE, addr, j);
	}
}

static void dram_find_spds_ddr3(spd_raw_data *spd, ramctr_timing *ctrl)
{
	int dimms = 0, dimms_on_channel;
	int channel, slot, spd_slot;
	dimm_info *dimm = &ctrl->info;

	memset (ctrl->rankmap, 0, sizeof(ctrl->rankmap));

	ctrl->extended_temperature_range = 1;
	ctrl->auto_self_refresh = 1;

	FOR_ALL_CHANNELS {
		ctrl->channel_size_mb[channel] = 0;

		dimms_on_channel = 0;
		/* count dimms on channel */
		for (slot = 0; slot < NUM_SLOTS; slot++) {
			spd_slot = 2 * channel + slot;
			printk(BIOS_DEBUG,
			       "SPD probe channel%d, slot%d\n", channel, slot);

			spd_decode_ddr3(&dimm->dimm[channel][slot], spd[spd_slot]);
			if (dimm->dimm[channel][slot].dram_type == SPD_MEMORY_TYPE_SDRAM_DDR3)
				dimms_on_channel++;
		}

		for (slot = 0; slot < NUM_SLOTS; slot++) {
			spd_slot = 2 * channel + slot;
			printk(BIOS_DEBUG,
			       "SPD probe channel%d, slot%d\n", channel, slot);

			/* search for XMP profile */
			spd_xmp_decode_ddr3(&dimm->dimm[channel][slot],
					spd[spd_slot],
					DDR3_XMP_PROFILE_1);

			if (dimm->dimm[channel][slot].dram_type != SPD_MEMORY_TYPE_SDRAM_DDR3) {
				printram("No valid XMP profile found.\n");
				spd_decode_ddr3(&dimm->dimm[channel][slot], spd[spd_slot]);
			} else if (dimms_on_channel > dimm->dimm[channel][slot].dimms_per_channel) {
				printram("XMP profile supports %u DIMMs, but %u DIMMs are installed.\n",
						 dimm->dimm[channel][slot].dimms_per_channel,
						 dimms_on_channel);
				if (IS_ENABLED(CONFIG_NATIVE_RAMINIT_IGNORE_XMP_MAX_DIMMS))
					printk(BIOS_WARNING, "XMP maximum DIMMs will be ignored.\n");
				else
					spd_decode_ddr3(&dimm->dimm[channel][slot], spd[spd_slot]);
			} else if (dimm->dimm[channel][slot].voltage != 1500) {
				/* TODO: support other DDR3 voltage than 1500mV */
				printram("XMP profile's requested %u mV is unsupported.\n",
						 dimm->dimm[channel][slot].voltage);
				spd_decode_ddr3(&dimm->dimm[channel][slot], spd[spd_slot]);
			}

			/* fill in CRC16 for MRC cache */
			ctrl->spd_crc[channel][slot] =
					spd_ddr3_calc_unique_crc(spd[spd_slot], sizeof(spd_raw_data));

			if (dimm->dimm[channel][slot].dram_type != SPD_MEMORY_TYPE_SDRAM_DDR3) {
				// set dimm invalid
				dimm->dimm[channel][slot].ranks = 0;
				dimm->dimm[channel][slot].size_mb = 0;
				continue;
			}

			dram_print_spd_ddr3(&dimm->dimm[channel][slot]);
			dimms++;
			ctrl->rank_mirror[channel][slot * 2] = 0;
			ctrl->rank_mirror[channel][slot * 2 + 1] = dimm->dimm[channel][slot].flags.pins_mirrored;
			ctrl->channel_size_mb[channel] += dimm->dimm[channel][slot].size_mb;

			ctrl->auto_self_refresh &= dimm->dimm[channel][slot].flags.asr;
			ctrl->extended_temperature_range &= dimm->dimm[channel][slot].flags.ext_temp_refresh;

			ctrl->rankmap[channel] |= ((1 << dimm->dimm[channel][slot].ranks) - 1) << (2 * slot);
			printk(BIOS_DEBUG, "channel[%d] rankmap = 0x%x\n",
			       channel, ctrl->rankmap[channel]);
		}
		if ((ctrl->rankmap[channel] & 3) && (ctrl->rankmap[channel] & 0xc)
			&& dimm->dimm[channel][0].reference_card <= 5 && dimm->dimm[channel][1].reference_card <= 5) {
			const int ref_card_offset_table[6][6] = {
				{ 0, 0, 0, 0, 2, 2, },
				{ 0, 0, 0, 0, 2, 2, },
				{ 0, 0, 0, 0, 2, 2, },
				{ 0, 0, 0, 0, 1, 1, },
				{ 2, 2, 2, 1, 0, 0, },
				{ 2, 2, 2, 1, 0, 0, },
			};
			ctrl->ref_card_offset[channel] = ref_card_offset_table[dimm->dimm[channel][0].reference_card]
				[dimm->dimm[channel][1].reference_card];
		} else
			ctrl->ref_card_offset[channel] = 0;
	}

	if (!dimms)
		die("No DIMMs were found");
}

static void save_timings(ramctr_timing *ctrl)
{
	/* Save the MRC S3 restore data to cbmem */
	mrc_cache_stash_data(MRC_TRAINING_DATA, MRC_CACHE_VERSION, ctrl,
			sizeof(*ctrl));
}

static int try_init_dram_ddr3(ramctr_timing *ctrl, int fast_boot,
		int s3_resume, int me_uma_size)
{
	if (ctrl->sandybridge)
		return try_init_dram_ddr3_sandy(ctrl, fast_boot, s3_resume, me_uma_size);
	else
		return try_init_dram_ddr3_ivy(ctrl, fast_boot, s3_resume, me_uma_size);
}

static void init_dram_ddr3(int min_tck, int s3resume)
{
	int me_uma_size;
	int cbmem_was_inited;
	ramctr_timing ctrl;
	int fast_boot;
	spd_raw_data spds[4];
	struct region_device rdev;
	ramctr_timing *ctrl_cached;
	struct cpuid_result cpures;
	int err;
	u32 cpu;

	MCHBAR32(0x5f00) |= 1;

	/* Wait for ME to be ready */
	intel_early_me_init();
	me_uma_size = intel_early_me_uma_size();

	printk(BIOS_DEBUG, "Starting native Platform init\n");

	u32 reg_5d10;

	wait_txt_clear();

	wrmsr(0x000002e6, (msr_t) { .lo = 0, .hi = 0 });

	reg_5d10 = MCHBAR32(0x5d10);	// !!! = 0x00000000
	if ((pci_read_config16(SOUTHBRIDGE, 0xa2) & 0xa0) == 0x20	/* 0x0004 */
	    && reg_5d10 && !s3resume) {
		MCHBAR32(0x5d10) = 0;
		/* Need reset.  */
		outb(0x6, 0xcf9);

		halt();
	}

	early_pch_init_native();
	early_thermal_init();

	/* try to find timings in MRC cache */
	int cache_not_found = mrc_cache_get_current(MRC_TRAINING_DATA,
						MRC_CACHE_VERSION, &rdev);
	if (cache_not_found || (region_device_sz(&rdev) < sizeof(ctrl))) {
		if (s3resume) {
			/* Failed S3 resume, reset to come up cleanly */
			outb(0x6, 0xcf9);
			halt();
		}
		ctrl_cached = NULL;
	} else {
		ctrl_cached = rdev_mmap_full(&rdev);
	}

	/* verify MRC cache for fast boot */
	if (!s3resume && ctrl_cached) {
		/* Load SPD unique information data. */
		memset(spds, 0, sizeof(spds));
		mainboard_get_spd(spds, 1);

		/* check SPD CRC16 to make sure the DIMMs haven't been replaced */
		fast_boot = verify_crc16_spds_ddr3(spds, ctrl_cached);
		if (!fast_boot)
			printk(BIOS_DEBUG, "Stored timings CRC16 mismatch.\n");
	} else {
		fast_boot = s3resume;
	}

	if (fast_boot) {
		printk(BIOS_DEBUG, "Trying stored timings.\n");
		memcpy(&ctrl, ctrl_cached, sizeof(ctrl));

		err = try_init_dram_ddr3(&ctrl, fast_boot, s3resume, me_uma_size);
		if (err) {
			if (s3resume) {
				/* Failed S3 resume, reset to come up cleanly */
				outb(0x6, 0xcf9);
				halt();
			}
			/* no need to erase bad mrc cache here, it gets overwritten on
			 * successful boot. */
			printk(BIOS_ERR, "Stored timings are invalid !\n");
			fast_boot = 0;
		}
	}
	if (!fast_boot) {
		/* Reset internal state */
		memset(&ctrl, 0, sizeof(ctrl));
		ctrl.tCK = min_tck;

		/* Get architecture */
		cpures = cpuid(1);
		cpu = cpures.eax;
		ctrl.sandybridge = IS_SANDY_CPU(cpu);

		/* Get DDR3 SPD data */
		memset(spds, 0, sizeof(spds));
		mainboard_get_spd(spds, 0);
		dram_find_spds_ddr3(spds, &ctrl);

		err = try_init_dram_ddr3(&ctrl, fast_boot, s3resume, me_uma_size);
	}

	if (err) {
		/* fallback: disable failing channel */
		printk(BIOS_ERR, "RAM training failed, trying fallback.\n");
		printram("Disable failing channel.\n");

		/* Reset internal state */
		memset(&ctrl, 0, sizeof(ctrl));
		ctrl.tCK = min_tck;

		/* Get architecture */
		cpures = cpuid(1);
		cpu = cpures.eax;
		ctrl.sandybridge = IS_SANDY_CPU(cpu);

		/* Reset DDR3 frequency */
		dram_find_spds_ddr3(spds, &ctrl);

		/* disable failing channel */
		disable_channel(&ctrl, GET_ERR_CHANNEL(err));

		err = try_init_dram_ddr3(&ctrl, fast_boot, s3resume, me_uma_size);
	}

	if (err)
		die("raminit failed");

	/* FIXME: should be hardware revision-dependent.  */
	MCHBAR32(0x5024) = 0x00a030ce;

	set_scrambling_seed(&ctrl);

	set_42a0(&ctrl);

	final_registers(&ctrl);

	/* Zone config */
	dram_zones(&ctrl, 0);

	/* Non intrusive, fast ram check */
	quick_ram_check();

	intel_early_me_status();
	intel_early_me_init_done(ME_INIT_STATUS_SUCCESS);
	intel_early_me_status();

	report_memory_config();

	cbmem_was_inited = !cbmem_recovery(s3resume);
	if (!fast_boot)
		save_timings(&ctrl);
	if (s3resume && !cbmem_was_inited) {
		/* Failed S3 resume, reset to come up cleanly */
		outb(0x6, 0xcf9);
		halt();
	}

	fill_smbios17(&ctrl);
}

void perform_raminit(int s3resume)
{
	post_code(0x3a);

	timestamp_add_now(TS_BEFORE_INITRAM);

	init_dram_ddr3(get_mem_min_tck(), s3resume);
}
