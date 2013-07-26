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
#include <bootmode.h>
#include <string.h>
#include <arch/io.h>
#include <cbmem.h>
#include <halt.h>
#include <timestamp.h>
#include <northbridge/intel/common/mrc_cache.h>
#include <southbridge/intel/bd82x6x/me.h>
#include <southbridge/intel/bd82x6x/smbus.h>
#include <cpu/x86/msr.h>
#include <delay.h>
#include <smbios.h>
#include <memory_info.h>
#include <lib.h>
#include "raminit_native.h"
#include "raminit_common.h"
#include "sandybridge.h"

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
			dimm->bus_width = info->dimm[channel][slot].width;
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
	int i;

	addr_decoder_common = MCHBAR32(0x5000);
	addr_decode_ch[0] = MCHBAR32(0x5004);
	addr_decode_ch[1] = MCHBAR32(0x5008);

	printk(BIOS_DEBUG, "memcfg DDR3 clock %d MHz\n",
	       (MCHBAR32(MC_BIOS_DATA) * 13333 * 2 + 50) / 100);
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
			spd_decode_ddr3(&dimm->dimm[channel][slot], spd[spd_slot]);
			if (dimm->dimm[channel][slot].dram_type == SPD_MEMORY_TYPE_SDRAM_DDR3)
				dimms_on_channel++;
		}

		for (slot = 0; slot < NUM_SLOTS; slot++) {
			spd_slot = 2 * channel + slot;
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

/* Frequency multiplier.  */
static u32 get_FRQ(u32 tCK)
{
	u32 FRQ;
	FRQ = 256000 / (tCK * BASEFREQ);
	if (FRQ > 8)
		return 8;
	if (FRQ < 3)
		return 3;
	return FRQ;
}

static u32 get_REFI(u32 tCK)
{
	/* Get REFI based on MCU frequency using the following rule:
	 *        _________________________________________
	 * FRQ : | 3    | 4    | 5    | 6    | 7    | 8    |
	 * REFI: | 3120 | 4160 | 5200 | 6240 | 7280 | 8320 |
	 */
	static const u32 frq_refi_map[] =
	    { 3120, 4160, 5200, 6240, 7280, 8320 };
	return frq_refi_map[get_FRQ(tCK) - 3];
}

static u8 get_XSOffset(u32 tCK)
{
	/* Get XSOffset based on MCU frequency using the following rule:
	 *             _________________________
	 * FRQ      : | 3 | 4 | 5 | 6 | 7  | 8  |
	 * XSOffset : | 4 | 6 | 7 | 8 | 10 | 11 |
	 */
	static const u8 frq_xs_map[] = { 4, 6, 7, 8, 10, 11 };
	return frq_xs_map[get_FRQ(tCK) - 3];
}

static u8 get_MOD(u32 tCK)
{
	/* Get MOD based on MCU frequency using the following rule:
	 *        _____________________________
	 * FRQ : | 3  | 4  | 5  | 6  | 7  | 8  |
	 * MOD : | 12 | 12 | 12 | 12 | 15 | 16 |
	 */
	static const u8 frq_mod_map[] = { 12, 12, 12, 12, 15, 16 };
	return frq_mod_map[get_FRQ(tCK) - 3];
}

static u8 get_WLO(u32 tCK)
{
	/* Get WLO based on MCU frequency using the following rule:
	 *        _______________________
	 * FRQ : | 3 | 4 | 5 | 6 | 7 | 8 |
	 * WLO : | 4 | 5 | 6 | 6 | 8 | 8 |
	 */
	static const u8 frq_wlo_map[] = { 4, 5, 6, 6, 8, 8 };
	return frq_wlo_map[get_FRQ(tCK) - 3];
}

static u8 get_CKE(u32 tCK)
{
	/* Get CKE based on MCU frequency using the following rule:
	 *        _______________________
	 * FRQ : | 3 | 4 | 5 | 6 | 7 | 8 |
	 * CKE : | 3 | 3 | 4 | 4 | 5 | 6 |
	 */
	static const u8 frq_cke_map[] = { 3, 3, 4, 4, 5, 6 };
	return frq_cke_map[get_FRQ(tCK) - 3];
}

static u8 get_XPDLL(u32 tCK)
{
	/* Get XPDLL based on MCU frequency using the following rule:
	 *          _____________________________
	 * FRQ   : | 3  | 4  | 5  | 6  | 7  | 8  |
	 * XPDLL : | 10 | 13 | 16 | 20 | 23 | 26 |
	 */
	static const u8 frq_xpdll_map[] = { 10, 13, 16, 20, 23, 26 };
	return frq_xpdll_map[get_FRQ(tCK) - 3];
}

static u8 get_XP(u32 tCK)
{
	/* Get XP based on MCU frequency using the following rule:
	 *        _______________________
	 * FRQ : | 3 | 4 | 5 | 6 | 7 | 8 |
	 * XP  : | 3 | 4 | 4 | 5 | 6 | 7 |
	 */
	static const u8 frq_xp_map[] = { 3, 4, 4, 5, 6, 7 };
	return frq_xp_map[get_FRQ(tCK) - 3];
}

static u8 get_AONPD(u32 tCK)
{
	/* Get AONPD based on MCU frequency using the following rule:
	 *          ________________________
	 * FRQ   : | 3 | 4 | 5 | 6 | 7 | 8  |
	 * AONPD : | 4 | 5 | 6 | 8 | 8 | 10 |
	 */
	static const u8 frq_aonpd_map[] = { 4, 5, 6, 8, 8, 10 };
	return frq_aonpd_map[get_FRQ(tCK) - 3];
}

static u32 get_COMP2(u32 tCK)
{
	/* Get COMP2 based on MCU frequency using the following rule:
	 *         ___________________________________________________________
	 * FRQ  : | 3       | 4       | 5       | 6       | 7       | 8       |
	 * COMP : | D6BEDCC | CE7C34C | CA57A4C | C6369CC | C42514C | C21410C |
	 */
	static const u32 frq_comp2_map[] = { 0xD6BEDCC, 0xCE7C34C, 0xCA57A4C,
		0xC6369CC, 0xC42514C, 0xC21410C
	};
	return frq_comp2_map[get_FRQ(tCK) - 3];
}

static void dram_timing(ramctr_timing * ctrl)
{
	u8 val;
	u32 val32;

	/* Maximum supported DDR3 frequency is 1066MHz (DDR3 2133) so make sure
	 * we cap it if we have faster DIMMs.
	 * Then, align it to the closest JEDEC standard frequency */
	if (ctrl->tCK <= TCK_1066MHZ) {
		ctrl->tCK = TCK_1066MHZ;
		ctrl->edge_offset[0] = 16;
		ctrl->edge_offset[1] = 7;
		ctrl->edge_offset[2] = 7;
		ctrl->timC_offset[0] = 18;
		ctrl->timC_offset[1] = 7;
		ctrl->timC_offset[2] = 7;
		ctrl->reg_320c_range_threshold = 13;
	} else if (ctrl->tCK <= TCK_933MHZ) {
		ctrl->tCK = TCK_933MHZ;
		ctrl->edge_offset[0] = 14;
		ctrl->edge_offset[1] = 6;
		ctrl->edge_offset[2] = 6;
		ctrl->timC_offset[0] = 15;
		ctrl->timC_offset[1] = 6;
		ctrl->timC_offset[2] = 6;
		ctrl->reg_320c_range_threshold = 15;
	} else if (ctrl->tCK <= TCK_800MHZ) {
		ctrl->tCK = TCK_800MHZ;
		ctrl->edge_offset[0] = 13;
		ctrl->edge_offset[1] = 5;
		ctrl->edge_offset[2] = 5;
		ctrl->timC_offset[0] = 14;
		ctrl->timC_offset[1] = 5;
		ctrl->timC_offset[2] = 5;
		ctrl->reg_320c_range_threshold = 15;
	} else if (ctrl->tCK <= TCK_666MHZ) {
		ctrl->tCK = TCK_666MHZ;
		ctrl->edge_offset[0] = 10;
		ctrl->edge_offset[1] = 4;
		ctrl->edge_offset[2] = 4;
		ctrl->timC_offset[0] = 11;
		ctrl->timC_offset[1] = 4;
		ctrl->timC_offset[2] = 4;
		ctrl->reg_320c_range_threshold = 16;
	} else if (ctrl->tCK <= TCK_533MHZ) {
		ctrl->tCK = TCK_533MHZ;
		ctrl->edge_offset[0] = 8;
		ctrl->edge_offset[1] = 3;
		ctrl->edge_offset[2] = 3;
		ctrl->timC_offset[0] = 9;
		ctrl->timC_offset[1] = 3;
		ctrl->timC_offset[2] = 3;
		ctrl->reg_320c_range_threshold = 17;
	} else  {
		ctrl->tCK = TCK_400MHZ;
		ctrl->edge_offset[0] = 6;
		ctrl->edge_offset[1] = 2;
		ctrl->edge_offset[2] = 2;
		ctrl->timC_offset[0] = 6;
		ctrl->timC_offset[1] = 2;
		ctrl->timC_offset[2] = 2;
		ctrl->reg_320c_range_threshold = 17;
	}

	/* Initial phase between CLK/CMD pins */
	ctrl->reg_c14_offset = (256000 / ctrl->tCK) / 66;

	/* DLL_CONFIG_MDLL_W_TIMER */
	ctrl->reg_5064b0 = (128000 / ctrl->tCK) + 3;

	val32 = (1000 << 8) / ctrl->tCK;
	printk(BIOS_DEBUG, "Selected DRAM frequency: %u MHz\n", val32);

	/* Find CAS latency */
	val = (ctrl->tAA + ctrl->tCK - 1) / ctrl->tCK;
	printk(BIOS_DEBUG, "Minimum  CAS latency   : %uT\n", val);
	/* Find lowest supported CAS latency that satisfies the minimum value */
	while (!((ctrl->cas_supported >> (val - MIN_CAS)) & 1)
	       && (ctrl->cas_supported >> (val - MIN_CAS))) {
		val++;
	}
	/* Is CAS supported */
	if (!(ctrl->cas_supported & (1 << (val - MIN_CAS)))) {
		printk(BIOS_ERR, "CAS %uT not supported. ", val);
		val = MAX_CAS;
		/* Find highest supported CAS latency */
		while (!((ctrl->cas_supported >> (val - MIN_CAS)) & 1))
			val--;

		printk(BIOS_ERR, "Using CAS %uT instead.\n", val);
	}

	printk(BIOS_DEBUG, "Selected CAS latency   : %uT\n", val);
	ctrl->CAS = val;
	ctrl->CWL = get_CWL(ctrl->tCK);
	printk(BIOS_DEBUG, "Selected CWL latency   : %uT\n", ctrl->CWL);

	/* Find tRCD */
	ctrl->tRCD = (ctrl->tRCD + ctrl->tCK - 1) / ctrl->tCK;
	printk(BIOS_DEBUG, "Selected tRCD          : %uT\n", ctrl->tRCD);

	ctrl->tRP = (ctrl->tRP + ctrl->tCK - 1) / ctrl->tCK;
	printk(BIOS_DEBUG, "Selected tRP           : %uT\n", ctrl->tRP);

	/* Find tRAS */
	ctrl->tRAS = (ctrl->tRAS + ctrl->tCK - 1) / ctrl->tCK;
	printk(BIOS_DEBUG, "Selected tRAS          : %uT\n", ctrl->tRAS);

	/* Find tWR */
	ctrl->tWR = (ctrl->tWR + ctrl->tCK - 1) / ctrl->tCK;
	printk(BIOS_DEBUG, "Selected tWR           : %uT\n", ctrl->tWR);

	/* Find tFAW */
	ctrl->tFAW = (ctrl->tFAW + ctrl->tCK - 1) / ctrl->tCK;
	printk(BIOS_DEBUG, "Selected tFAW          : %uT\n", ctrl->tFAW);

	/* Find tRRD */
	ctrl->tRRD = (ctrl->tRRD + ctrl->tCK - 1) / ctrl->tCK;
	printk(BIOS_DEBUG, "Selected tRRD          : %uT\n", ctrl->tRRD);

	/* Find tRTP */
	ctrl->tRTP = (ctrl->tRTP + ctrl->tCK - 1) / ctrl->tCK;
	printk(BIOS_DEBUG, "Selected tRTP          : %uT\n", ctrl->tRTP);

	/* Find tWTR */
	ctrl->tWTR = (ctrl->tWTR + ctrl->tCK - 1) / ctrl->tCK;
	printk(BIOS_DEBUG, "Selected tWTR          : %uT\n", ctrl->tWTR);

	/* Refresh-to-Active or Refresh-to-Refresh (tRFC) */
	ctrl->tRFC = (ctrl->tRFC + ctrl->tCK - 1) / ctrl->tCK;
	printk(BIOS_DEBUG, "Selected tRFC          : %uT\n", ctrl->tRFC);

	ctrl->tREFI = get_REFI(ctrl->tCK);
	ctrl->tMOD = get_MOD(ctrl->tCK);
	ctrl->tXSOffset = get_XSOffset(ctrl->tCK);
	ctrl->tWLO = get_WLO(ctrl->tCK);
	ctrl->tCKE = get_CKE(ctrl->tCK);
	ctrl->tXPDLL = get_XPDLL(ctrl->tCK);
	ctrl->tXP = get_XP(ctrl->tCK);
	ctrl->tAONPD = get_AONPD(ctrl->tCK);
}

static void dram_freq(ramctr_timing * ctrl)
{
	if (ctrl->tCK > TCK_400MHZ) {
		printk (BIOS_ERR, "DRAM frequency is under lowest supported frequency (400 MHz). Increasing to 400 MHz as last resort");
		ctrl->tCK = TCK_400MHZ;
	}
	while (1) {
		u8 val2;
		u32 reg1 = 0;

		/* Step 1 - Set target PCU frequency */

		if (ctrl->tCK <= TCK_1066MHZ) {
			ctrl->tCK = TCK_1066MHZ;
		} else if (ctrl->tCK <= TCK_933MHZ) {
			ctrl->tCK = TCK_933MHZ;
		} else if (ctrl->tCK <= TCK_800MHZ) {
			ctrl->tCK = TCK_800MHZ;
		} else if (ctrl->tCK <= TCK_666MHZ) {
			ctrl->tCK = TCK_666MHZ;
		} else if (ctrl->tCK <= TCK_533MHZ) {
			ctrl->tCK = TCK_533MHZ;
		} else if (ctrl->tCK <= TCK_400MHZ) {
			ctrl->tCK = TCK_400MHZ;
		} else {
			die ("No lock frequency found");
		}

		/* Frequency multiplier.  */
		u32 FRQ = get_FRQ(ctrl->tCK);

		/* The PLL will never lock if the required frequency is
		 * already set. Exit early to prevent a system hang.
		 */
		reg1 = MCHBAR32(MC_BIOS_DATA);
		val2 = (u8) reg1;
		if (val2)
			return;

		/* Step 2 - Select frequency in the MCU */
		reg1 = FRQ;
		reg1 |= 0x80000000;	// set running bit
		MCHBAR32(MC_BIOS_REQ) = reg1;
		int i=0;
		printk(BIOS_DEBUG, "PLL busy... ");
		while (reg1 & 0x80000000) {
			udelay(10);
			i++;
			reg1 = MCHBAR32(MC_BIOS_REQ);
		}
		printk(BIOS_DEBUG, "done in %d us\n", i * 10);

		/* Step 3 - Verify lock frequency */
		reg1 = MCHBAR32(MC_BIOS_DATA);
		val2 = (u8) reg1;
		if (val2 >= FRQ) {
			printk(BIOS_DEBUG, "MCU frequency is set at : %d MHz\n",
			       (1000 << 8) / ctrl->tCK);
			return;
		}
		printk(BIOS_DEBUG, "PLL didn't lock. Retrying at lower frequency\n");
		ctrl->tCK++;
	}
}

static void dram_ioregs(ramctr_timing * ctrl)
{
	u32 reg, comp2;

	int channel;

	// IO clock
	FOR_ALL_CHANNELS {
		MCHBAR32(0xc00 + 0x100 * channel) = ctrl->rankmap[channel];
	}

	// IO command
	FOR_ALL_CHANNELS {
		MCHBAR32(0x3200 + 0x100 * channel) = ctrl->rankmap[channel];
	}

	// IO control
	FOR_ALL_POPULATED_CHANNELS {
		program_timings(ctrl, channel);
	}

	// Rcomp
	printram("RCOMP...");
	reg = 0;
	while (reg == 0) {
		reg = MCHBAR32(0x5084) & 0x10000;
	}
	printram("done\n");

	// Set comp2
	comp2 = get_COMP2(ctrl->tCK);
	MCHBAR32(0x3714) = comp2;
	printram("COMP2 done\n");

	// Set comp1
	FOR_ALL_POPULATED_CHANNELS {
		reg = MCHBAR32(0x1810 + channel * 0x100);	//ch0
		reg = (reg & ~0xe00) | (1 << 9);	//odt
		reg = (reg & ~0xe00000) | (1 << 21);	//clk drive up
		reg = (reg & ~0x38000000) | (1 << 27);	//ctl drive up
		MCHBAR32(0x1810 + channel * 0x100) = reg;
	}
	printram("COMP1 done\n");

	printram("FORCE RCOMP and wait 20us...");
	MCHBAR32(0x5f08) |= 0x100;
	udelay(20);
	printram("done\n");
}

static void save_timings(ramctr_timing *ctrl)
{
	/* Save the MRC S3 restore data to cbmem */
	store_current_mrc_cache(ctrl, sizeof(*ctrl));
}

static int try_init_dram_ddr3(ramctr_timing *ctrl, int fast_boot,
		int s3_resume, int me_uma_size)
{
	int err;

	printk(BIOS_DEBUG, "Starting RAM training (%d).\n", fast_boot);

	if (!fast_boot) {
		/* Find fastest common supported parameters */
		dram_find_common_params(ctrl);

		dram_dimm_mapping(ctrl);
	}

	/* Set MCU frequency */
	dram_freq(ctrl);

	if (!fast_boot) {
		/* Calculate timings */
		dram_timing(ctrl);
	}

	/* Set version register */
	MCHBAR32(0x5034) = 0xC04EB002;

	/* Enable crossover */
	dram_xover(ctrl);

	/* Set timing and refresh registers */
	dram_timing_regs(ctrl);

	/* Power mode preset */
	MCHBAR32(0x4e80) = 0x5500;

	/* Set scheduler parameters */
	MCHBAR32(0x4c20) = 0x10100005;

	/* Set CPU specific register */
	set_4f8c();

	/* Clear IO reset bit */
	MCHBAR32(0x5030) &= ~0x20;

	/* Set MAD-DIMM registers */
	dram_dimm_set_mapping(ctrl);
	printk(BIOS_DEBUG, "Done dimm mapping\n");

	/* Zone config */
	dram_zones(ctrl, 1);

	/* Set memory map */
	dram_memorymap(ctrl, me_uma_size);
	printk(BIOS_DEBUG, "Done memory map\n");

	/* Set IO registers */
	dram_ioregs(ctrl);
	printk(BIOS_DEBUG, "Done io registers\n");

	udelay(1);

	if (fast_boot) {
		restore_timings(ctrl);
	} else {
		/* Do jedec ddr3 reset sequence */
		dram_jedecreset(ctrl);
		printk(BIOS_DEBUG, "Done jedec reset\n");

		/* MRS commands */
		dram_mrscommands(ctrl);
		printk(BIOS_DEBUG, "Done MRS commands\n");

		/* Prepare for memory training */
		prepare_training(ctrl);

		err = read_training(ctrl);
		if (err)
			return err;

		err = write_training(ctrl);
		if (err)
			return err;

		printram("CP5a\n");

		err = discover_edges(ctrl);
		if (err)
			return err;

		printram("CP5b\n");

		err = command_training(ctrl);
		if (err)
			return err;

		printram("CP5c\n");

		err = discover_edges_write(ctrl);
		if (err)
			return err;

		err = discover_timC_write(ctrl);
		if (err)
			return err;

		normalize_training(ctrl);
	}

	set_4008c(ctrl);

	write_controller_mr(ctrl);

	if (!s3_resume) {
		err = channel_test(ctrl);
		if (err)
			return err;
	}

	return 0;
}

static void init_dram_ddr3(int mobile, int min_tck, int s3resume)
{
	int me_uma_size;
	int cbmem_was_inited;
	ramctr_timing ctrl;
	int fast_boot;
	spd_raw_data spds[4];
	struct mrc_data_container *mrc_cache;
	ramctr_timing *ctrl_cached;
	int err;

	MCHBAR32(0x5f00) |= 1;

	report_platform_info();

	/* Wait for ME to be ready */
	intel_early_me_init();
	me_uma_size = intel_early_me_uma_size();

	printk(BIOS_DEBUG, "Starting native Platform init\n");

	u32 reg_5d10;

	wait_txt_clear();

	wrmsr(0x000002e6, (msr_t) { .lo = 0, .hi = 0 });

	reg_5d10 = read32(DEFAULT_MCHBAR + 0x5d10);	// !!! = 0x00000000
	if ((pci_read_config16(SOUTHBRIDGE, 0xa2) & 0xa0) == 0x20	/* 0x0004 */
	    && reg_5d10 && !s3resume) {
		write32(DEFAULT_MCHBAR + 0x5d10, 0);
		/* Need reset.  */
		outb(0x6, 0xcf9);

		halt();
	}

	early_pch_init_native();
	early_thermal_init();

	/* try to find timings in MRC cache */
	mrc_cache = find_current_mrc_cache();
	if (!mrc_cache || (mrc_cache->mrc_data_size < sizeof(ctrl))) {
		if (s3resume) {
			/* Failed S3 resume, reset to come up cleanly */
			outb(0x6, 0xcf9);
			halt();
		}
		ctrl_cached = NULL;
	} else {
		ctrl_cached = (ramctr_timing *)mrc_cache->mrc_data;
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
		ctrl.mobile = mobile;
		ctrl.tCK = min_tck;

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
		ctrl.mobile = mobile;
		ctrl.tCK = min_tck;

		/* Reset DDR3 frequency */
		dram_find_spds_ddr3(spds, &ctrl);

		/* disable failing channel */
		disable_channel(&ctrl, GET_ERR_CHANNEL(err));

		err = try_init_dram_ddr3(&ctrl, fast_boot, s3resume, me_uma_size);
	}

	if (err)
		die("raminit failed");

	/* FIXME: should be hardware revision-dependent.  */
	write32(DEFAULT_MCHBAR + 0x5024, 0x00a030ce);

	set_scrambling_seed(&ctrl);

	set_42a0(&ctrl);

	final_registers(&ctrl);

	/* Zone config */
	dram_zones(&ctrl, 0);

	if (!fast_boot)
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

	init_dram_ddr3(1, get_mem_min_tck(), s3resume);
}
