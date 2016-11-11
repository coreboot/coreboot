/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Damien Zammit <damien@zamaudio.com>
 * Copyright (C) 2014 Vladimir Serbinenko <phcoder@gmail.com>
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
#include <arch/cbfs.h>
#include <cbfs.h>
#include <halt.h>
#include <ip_checksum.h>
#include <timestamp.h>
#include <pc80/mc146818rtc.h>
#include <northbridge/intel/common/mrc_cache.h>
#include <device/pci_def.h>
#include <memory_info.h>
#include <smbios.h>
#include "raminit_native.h"
#include "sandybridge.h"
#include <delay.h>
#include <lib.h>
#include <device/device.h>

/* Management Engine is in the southbridge */
#include "southbridge/intel/bd82x6x/me.h"
/* For SPD.  */
#include "southbridge/intel/bd82x6x/smbus.h"
#include "arch/cpu.h"
#include "cpu/x86/msr.h"
#include <northbridge/intel/sandybridge/chip.h>

/* FIXME: no ECC support.  */
/* FIXME: no support for 3-channel chipsets.  */

/*
 * Register description:
 * Intel provides a command queue of depth four.
 * Every command is configured by using multiple registers.
 * On executing the command queue you have to provide the depth used.
 *
 * Known registers:
 * Channel X = [0, 1]
 * Command queue index Y = [0, 1, 2, 3]
 *
 * DEFAULT_MCHBAR + 0x4220 + 0x400 * X + 4 * Y: command io register
 *  Controls the DRAM command signals
 *  Bit 0: !RAS
 *  Bit 1: !CAS
 *  Bit 2: !WE
 *
 * DEFAULT_MCHBAR + 0x4200 + 0x400 * X + 4 * Y: addr bankslot io register
 *  Controls the address, bank address and slotrank signals
 *  Bit 0-15 : Address
 *  Bit 20-22: Bank Address
 *  Bit 24-25: slotrank
 *
 * DEFAULT_MCHBAR + 0x4230 + 0x400 * X + 4 * Y: idle register
 *  Controls the idle time after issuing this DRAM command
 *  Bit 16-32: number of clock-cylces to idle
 *
 * DEFAULT_MCHBAR + 0x4284 + 0x400 * channel: execute command queue
 *  Starts to execute all queued commands
 *  Bit 0    : start DRAM command execution
 *  Bit 16-20: (number of queued commands - 1) * 4
 */

#define BASEFREQ 133
#define tDLLK 512

#define IS_SANDY_CPU(x) ((x & 0xffff0) == 0x206a0)
#define IS_SANDY_CPU_C(x) ((x & 0xf) == 4)
#define IS_SANDY_CPU_D0(x) ((x & 0xf) == 5)
#define IS_SANDY_CPU_D1(x) ((x & 0xf) == 6)
#define IS_SANDY_CPU_D2(x) ((x & 0xf) == 7)

#define IS_IVY_CPU(x) ((x & 0xffff0) == 0x306a0)
#define IS_IVY_CPU_C(x) ((x & 0xf) == 4)
#define IS_IVY_CPU_K(x) ((x & 0xf) == 5)
#define IS_IVY_CPU_D(x) ((x & 0xf) == 6)
#define IS_IVY_CPU_E(x) ((x & 0xf) >= 8)

#define NUM_CHANNELS 2
#define NUM_SLOTRANKS 4
#define NUM_SLOTS 2
#define NUM_LANES 8

/* FIXME: Vendor BIOS uses 64 but our algorithms are less
   performant and even 1 seems to be enough in practice.  */
#define NUM_PATTERNS 4

typedef struct odtmap_st {
	u16 rttwr;
	u16 rttnom;
} odtmap;

typedef struct dimm_info_st {
	dimm_attr dimm[NUM_CHANNELS][NUM_SLOTS];
} dimm_info;

struct ram_rank_timings {
	/* Register 4024. One byte per slotrank.  */
	u8 val_4024;
	/* Register 4028. One nibble per slotrank.  */
	u8 val_4028;

	int val_320c;

	struct ram_lane_timings {
		/* lane register offset 0x10.  */
		u16 timA;	/* bits 0 - 5, bits 16 - 18 */
		u8 rising;	/* bits 8 - 14 */
		u8 falling;	/* bits 20 - 26.  */

		/* lane register offset 0x20.  */
		int timC;	/* bit 0 - 5, 19.  */
		u16 timB;	/* bits 8 - 13, 15 - 17.  */
	} lanes[NUM_LANES];
};

struct ramctr_timing_st;

typedef struct ramctr_timing_st {
	u16 spd_crc[NUM_CHANNELS][NUM_SLOTS];
	int mobile;

	u16 cas_supported;
	/* tLatencies are in units of ns, scaled by x256 */
	u32 tCK;
	u32 tAA;
	u32 tWR;
	u32 tRCD;
	u32 tRRD;
	u32 tRP;
	u32 tRAS;
	u32 tRFC;
	u32 tWTR;
	u32 tRTP;
	u32 tFAW;
	/* Latencies in terms of clock cycles
	 * They are saved separately as they are needed for DRAM MRS commands*/
	u8 CAS;			/* CAS read latency */
	u8 CWL;			/* CAS write latency */

	u32 tREFI;
	u32 tMOD;
	u32 tXSOffset;
	u32 tWLO;
	u32 tCKE;
	u32 tXPDLL;
	u32 tXP;
	u32 tAONPD;

	u16 reg_5064b0; /* bits 0-11. */

	u8 rankmap[NUM_CHANNELS];
	int ref_card_offset[NUM_CHANNELS];
	u32 mad_dimm[NUM_CHANNELS];
	int channel_size_mb[NUM_CHANNELS];
	u32 cmd_stretch[NUM_CHANNELS];

	int reg_c14_offset;
	int reg_320c_range_threshold;

	int edge_offset[3];
	int timC_offset[3];

	int extended_temperature_range;
	int auto_self_refresh;

	int rank_mirror[NUM_CHANNELS][NUM_SLOTRANKS];

	struct ram_rank_timings timings[NUM_CHANNELS][NUM_SLOTRANKS];

	dimm_info info;
} ramctr_timing;

#define SOUTHBRIDGE PCI_DEV(0, 0x1f, 0)
#define NORTHBRIDGE PCI_DEV(0, 0x0, 0)
#define FOR_ALL_LANES for (lane = 0; lane < NUM_LANES; lane++)
#define FOR_ALL_CHANNELS for (channel = 0; channel < NUM_CHANNELS; channel++)
#define FOR_ALL_POPULATED_RANKS for (slotrank = 0; slotrank < NUM_SLOTRANKS; slotrank++) if (ctrl->rankmap[channel] & (1 << slotrank))
#define FOR_ALL_POPULATED_CHANNELS for (channel = 0; channel < NUM_CHANNELS; channel++) if (ctrl->rankmap[channel])
#define MAX_EDGE_TIMING 71
#define MAX_TIMC 127
#define MAX_TIMB 511
#define MAX_TIMA 127

#define MAKE_ERR ((channel<<16)|(slotrank<<8)|1)
#define GET_ERR_CHANNEL(x) (x>>16)

static void program_timings(ramctr_timing * ctrl, int channel);
static unsigned int get_mmio_size(void);

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

static void sfence(void)
{
	asm volatile ("sfence");
}

static void toggle_io_reset(void) {
	/* toggle IO reset bit */
	u32 r32 = read32(DEFAULT_MCHBAR + 0x5030);
	write32(DEFAULT_MCHBAR + 0x5030, r32 | 0x20);
	udelay(1);
	write32(DEFAULT_MCHBAR + 0x5030, r32 & ~0x20);
	udelay(1);
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
	       (MCHBAR32(0x5e04) * 13333 * 2 + 50) / 100);
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
					spd_ddr3_calc_crc(spd[spd_slot], sizeof(spd_raw_data));
		}
	}
	return match;
}

void read_spd(spd_raw_data * spd, u8 addr)
{
	int j;
	for (j = 0; j < 256; j++)
		(*spd)[j] = do_smbus_read_byte(SMBUS_IO_BASE, addr, j);
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
					spd_ddr3_calc_crc(spd[spd_slot], sizeof(spd_raw_data));

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

static void dram_find_common_params(ramctr_timing *ctrl)
{
	size_t valid_dimms;
	int channel, slot;
	dimm_info *dimms = &ctrl->info;

	ctrl->cas_supported = 0xff;
	valid_dimms = 0;
	FOR_ALL_CHANNELS for (slot = 0; slot < 2; slot++) {
		const dimm_attr *dimm = &dimms->dimm[channel][slot];
		if (dimm->dram_type != SPD_MEMORY_TYPE_SDRAM_DDR3)
			continue;
		valid_dimms++;

		/* Find all possible CAS combinations */
		ctrl->cas_supported &= dimm->cas_supported;

		/* Find the smallest common latencies supported by all DIMMs */
		ctrl->tCK = MAX(ctrl->tCK, dimm->tCK);
		ctrl->tAA = MAX(ctrl->tAA, dimm->tAA);
		ctrl->tWR = MAX(ctrl->tWR, dimm->tWR);
		ctrl->tRCD = MAX(ctrl->tRCD, dimm->tRCD);
		ctrl->tRRD = MAX(ctrl->tRRD, dimm->tRRD);
		ctrl->tRP = MAX(ctrl->tRP, dimm->tRP);
		ctrl->tRAS = MAX(ctrl->tRAS, dimm->tRAS);
		ctrl->tRFC = MAX(ctrl->tRFC, dimm->tRFC);
		ctrl->tWTR = MAX(ctrl->tWTR, dimm->tWTR);
		ctrl->tRTP = MAX(ctrl->tRTP, dimm->tRTP);
		ctrl->tFAW = MAX(ctrl->tFAW, dimm->tFAW);
	}

	if (!ctrl->cas_supported)
		die("Unsupported DIMM combination. "
		    "DIMMS do not support common CAS latency");
	if (!valid_dimms)
		die("No valid DIMMs found");
}

/* CAS write latency. To be programmed in MR2.
 * See DDR3 SPEC for MR2 documentation. */
static u8 get_CWL(u32 tCK)
{
	/* Get CWL based on tCK using the following rule: */
	switch (tCK) {
	case TCK_1333MHZ:
		return 12;
	case TCK_1200MHZ:
	case TCK_1100MHZ:
		return 11;
	case TCK_1066MHZ:
	case TCK_1000MHZ:
		return 10;
	case TCK_933MHZ:
	case TCK_900MHZ:
		return 9;
	case TCK_800MHZ:
	case TCK_700MHZ:
		return 8;
	case TCK_666MHZ:
		return 7;
	case TCK_533MHZ:
		return 6;
	default:
		return 5;
	}
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

static u32 get_XOVER_CLK(u8 rankmap)
{
	return rankmap << 24;
}

static u32 get_XOVER_CMD(u8 rankmap)
{
	u32 reg;

	// enable xover cmd
	reg = 0x4000;

	// enable xover ctl
	if (rankmap & 0x3)
		reg |= 0x20000;

	if (rankmap & 0xc)
		reg |= 0x4000000;

	return reg;
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
	while (!((ctrl->cas_supported >> (val - 4)) & 1)
	       && (ctrl->cas_supported >> (val - 4))) {
		val++;
	}
	/* Is CAS supported */
	if (!(ctrl->cas_supported & (1 << (val - 4)))) {
		printk(BIOS_ERR, "CAS %uT not supported. ", val);
		val = 18;
		/* Find highest supported CAS latency */
		while (!((ctrl->cas_supported >> (val - 4)) & 1))
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

		/* Frequency mulitplier.  */
		u32 FRQ = get_FRQ(ctrl->tCK);

		/* The PLL will never lock if the required frequency is
		 * already set. Exit early to prevent a system hang.
		 */
		reg1 = MCHBAR32(0x5e04);
		val2 = (u8) reg1;
		if (val2 == FRQ) {
			printk(BIOS_DEBUG, "MCU frequency is set at : %d MHz\n",
			       (1000 << 8) / ctrl->tCK);
			return;
		}

		/* Step 2 - Select frequency in the MCU */
		reg1 = FRQ;
		reg1 |= 0x80000000;	// set running bit
		MCHBAR32(0x5e00) = reg1;
		while (reg1 & 0x80000000) {
			printk(BIOS_DEBUG, " PLL busy...");
			reg1 = MCHBAR32(0x5e00);
		}
		printk(BIOS_DEBUG, "done\n");

		/* Step 3 - Verify lock frequency */
		reg1 = MCHBAR32(0x5e04);
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

static void dram_xover(ramctr_timing * ctrl)
{
	u32 reg;
	int channel;

	FOR_ALL_CHANNELS {
		// enable xover clk
		reg = get_XOVER_CLK(ctrl->rankmap[channel]);
		printram("XOVER CLK [%x] = %x\n", channel * 0x100 + 0xc14,
			 reg);
		MCHBAR32(channel * 0x100 + 0xc14) = reg;

		// enable xover ctl & xover cmd
		reg = get_XOVER_CMD(ctrl->rankmap[channel]);
		printram("XOVER CMD [%x] = %x\n", 0x100 * channel + 0x320c,
			 reg);
		MCHBAR32(0x100 * channel + 0x320c) = reg;
	}
}

static void dram_timing_regs(ramctr_timing * ctrl)
{
	u32 reg, addr, val32, cpu, stretch;
	struct cpuid_result cpures;
	int channel;

	FOR_ALL_CHANNELS {
		// DBP
		reg = 0;
		reg |= ctrl->tRCD;
		reg |= (ctrl->tRP << 4);
		reg |= (ctrl->CAS << 8);
		reg |= (ctrl->CWL << 12);
		reg |= (ctrl->tRAS << 16);
		printram("DBP [%x] = %x\n", 0x400 * channel + 0x4000, reg);
		MCHBAR32(0x400 * channel + 0x4000) = reg;

		// RAP
		reg = 0;
		reg |= ctrl->tRRD;
		reg |= (ctrl->tRTP << 4);
		reg |= (ctrl->tCKE << 8);
		reg |= (ctrl->tWTR << 12);
		reg |= (ctrl->tFAW << 16);
		reg |= (ctrl->tWR << 24);
		reg |= (3 << 30);
		printram("RAP [%x] = %x\n", 0x400 * channel + 0x4004, reg);
		MCHBAR32(0x400 * channel + 0x4004) = reg;

		// OTHP
		addr = 0x400 * channel + 0x400c;
		reg = 0;
		reg |= ctrl->tXPDLL;
		reg |= (ctrl->tXP << 5);
		reg |= (ctrl->tAONPD << 8);
		reg |= 0xa0000;
		printram("OTHP [%x] = %x\n", addr, reg);
		MCHBAR32(addr) = reg;

		MCHBAR32(0x400 * channel + 0x4014) = 0;

		MCHBAR32(addr) |= 0x00020000;

		// ODT stretch
		reg = 0;

		cpures = cpuid(1);
		cpu = cpures.eax;
		if (IS_IVY_CPU(cpu)
		    || (IS_SANDY_CPU(cpu) && IS_SANDY_CPU_D2(cpu))) {
			stretch = 2;
			addr = 0x400 * channel + 0x400c;
			printram("ODT stretch [%x] = %x\n",
			       0x400 * channel + 0x400c, reg);
			reg = MCHBAR32(addr);

			if (((ctrl->rankmap[channel] & 3) == 0)
			    || (ctrl->rankmap[channel] & 0xc) == 0) {

				// Rank 0 - operate on rank 2
				reg = (reg & ~0xc0000) | (stretch << 18);

				// Rank 2 - operate on rank 0
				reg = (reg & ~0x30000) | (stretch << 16);

				printram("ODT stretch [%x] = %x\n", addr, reg);
				MCHBAR32(addr) = reg;
			}

		} else if (IS_SANDY_CPU(cpu) && IS_SANDY_CPU_C(cpu)) {
			stretch = 3;
			addr = 0x400 * channel + 0x401c;
			reg = MCHBAR32(addr);

			if (((ctrl->rankmap[channel] & 3) == 0)
			    || (ctrl->rankmap[channel] & 0xc) == 0) {

				// Rank 0 - operate on rank 2
				reg = (reg & ~0x3000) | (stretch << 12);

				// Rank 2 - operate on rank 0
				reg = (reg & ~0xc00) | (stretch << 10);

				printram("ODT stretch [%x] = %x\n", addr, reg);
				MCHBAR32(addr) = reg;
			}
		} else {
			stretch = 0;
		}

		// REFI
		reg = 0;
		val32 = ctrl->tREFI;
		reg = (reg & ~0xffff) | val32;
		val32 = ctrl->tRFC;
		reg = (reg & ~0x1ff0000) | (val32 << 16);
		val32 = (u32) (ctrl->tREFI * 9) / 1024;
		reg = (reg & ~0xfe000000) | (val32 << 25);
		printram("REFI [%x] = %x\n", 0x400 * channel + 0x4298,
		       reg);
		MCHBAR32(0x400 * channel + 0x4298) = reg;

		MCHBAR32(0x400 * channel + 0x4294) |= 0xff;

		// SRFTP
		reg = 0;
		val32 = tDLLK;
		reg = (reg & ~0xfff) | val32;
		val32 = ctrl->tXSOffset;
		reg = (reg & ~0xf000) | (val32 << 12);
		val32 = tDLLK - ctrl->tXSOffset;
		reg = (reg & ~0x3ff0000) | (val32 << 16);
		val32 = ctrl->tMOD - 8;
		reg = (reg & ~0xf0000000) | (val32 << 28);
		printram("SRFTP [%x] = %x\n", 0x400 * channel + 0x42a4,
		       reg);
		MCHBAR32(0x400 * channel + 0x42a4) = reg;
	}
}

static void dram_dimm_mapping(ramctr_timing *ctrl)
{
	u32 reg, val32;
	int channel;
	dimm_info *info = &ctrl->info;

	FOR_ALL_CHANNELS {
		dimm_attr *dimmA = 0;
		dimm_attr *dimmB = 0;
		reg = 0;
		val32 = 0;
		if (info->dimm[channel][0].size_mb >=
		    info->dimm[channel][1].size_mb) {
			// dimm 0 is bigger, set it to dimmA
			dimmA = &info->dimm[channel][0];
			dimmB = &info->dimm[channel][1];
			reg |= (0 << 16);
		} else {
			// dimm 1 is bigger, set it to dimmA
			dimmA = &info->dimm[channel][1];
			dimmB = &info->dimm[channel][0];
			reg |= (1 << 16);
		}
		// dimmA
		if (dimmA && (dimmA->ranks > 0)) {
			val32 = dimmA->size_mb / 256;
			reg = (reg & ~0xff) | val32;
			val32 = dimmA->ranks - 1;
			reg = (reg & ~0x20000) | (val32 << 17);
			val32 = (dimmA->width / 8) - 1;
			reg = (reg & ~0x80000) | (val32 << 19);
		}
		// dimmB
		if (dimmB && (dimmB->ranks > 0)) {
			val32 = dimmB->size_mb / 256;
			reg = (reg & ~0xff00) | (val32 << 8);
			val32 = dimmB->ranks - 1;
			reg = (reg & ~0x40000) | (val32 << 18);
			val32 = (dimmB->width / 8) - 1;
			reg = (reg & ~0x100000) | (val32 << 20);
		}
		reg = (reg & ~0x200000) | (1 << 21);	// rank interleave
		reg = (reg & ~0x400000) | (1 << 22);	// enhanced interleave

		// Save MAD-DIMM register
		if ((dimmA && (dimmA->ranks > 0))
		    || (dimmB && (dimmB->ranks > 0))) {
			ctrl->mad_dimm[channel] = reg;
		} else {
			ctrl->mad_dimm[channel] = 0;
		}
	}
}

static void dram_dimm_set_mapping(ramctr_timing * ctrl)
{
	int channel;
	FOR_ALL_CHANNELS {
		MCHBAR32(0x5004 + channel * 4) = ctrl->mad_dimm[channel];
	}
}

static void dram_zones(ramctr_timing * ctrl, int training)
{
	u32 reg, ch0size, ch1size;
	u8 val;
	reg = 0;
	val = 0;
	if (training) {
		ch0size = ctrl->channel_size_mb[0] ? 256 : 0;
		ch1size = ctrl->channel_size_mb[1] ? 256 : 0;
	} else {
		ch0size = ctrl->channel_size_mb[0];
		ch1size = ctrl->channel_size_mb[1];
	}

	if (ch0size >= ch1size) {
		reg = MCHBAR32(0x5014);
		val = ch1size / 256;
		reg = (reg & ~0xff000000) | val << 24;
		reg = (reg & ~0xff0000) | (2 * val) << 16;
		MCHBAR32(0x5014) = reg;
		MCHBAR32(0x5000) = 0x24;
	} else {
		reg = MCHBAR32(0x5014);
		val = ch0size / 256;
		reg = (reg & ~0xff000000) | val << 24;
		reg = (reg & ~0xff0000) | (2 * val) << 16;
		MCHBAR32(0x5014) = reg;
		MCHBAR32(0x5000) = 0x21;
	}
}

static void dram_memorymap(ramctr_timing * ctrl, int me_uma_size)
{
	u32 reg, val, reclaim;
	u32 tom, gfxstolen, gttsize;
	size_t tsegsize, mmiosize, toludbase, touudbase, gfxstolenbase, gttbase,
	    tsegbase, mestolenbase;
	size_t tsegbasedelta, remapbase, remaplimit;
	uint16_t ggc;

	mmiosize = get_mmio_size();

	ggc = pci_read_config16(NORTHBRIDGE, GGC);
	if (!(ggc & 2)) {
		gfxstolen = ((ggc >> 3) & 0x1f) * 32;
		gttsize = ((ggc >> 8) & 0x3);
	} else {
		gfxstolen = 0;
		gttsize = 0;
	}

	tsegsize = CONFIG_SMM_TSEG_SIZE >> 20;

	tom = ctrl->channel_size_mb[0] + ctrl->channel_size_mb[1];

	mestolenbase = tom - me_uma_size;

	toludbase = MIN(4096 - mmiosize + gfxstolen + gttsize + tsegsize,
			tom - me_uma_size);
	gfxstolenbase = toludbase - gfxstolen;
	gttbase = gfxstolenbase - gttsize;

	tsegbase = gttbase - tsegsize;

	// Round tsegbase down to nearest address aligned to tsegsize
	tsegbasedelta = tsegbase & (tsegsize - 1);
	tsegbase &= ~(tsegsize - 1);

	gttbase -= tsegbasedelta;
	gfxstolenbase -= tsegbasedelta;
	toludbase -= tsegbasedelta;

	// Test if it is possible to reclaim a hole in the RAM addressing
	if (tom - me_uma_size > toludbase) {
		// Reclaim is possible
		reclaim = 1;
		remapbase = MAX(4096, tom - me_uma_size);
		remaplimit =
		    remapbase + MIN(4096, tom - me_uma_size) - toludbase - 1;
		touudbase = remaplimit + 1;
	} else {
		// Reclaim not possible
		reclaim = 0;
		touudbase = tom - me_uma_size;
	}

	// Update memory map in pci-e configuration space
	printk(BIOS_DEBUG, "Update PCI-E configuration space:\n");

	// TOM (top of memory)
	reg = pcie_read_config32(PCI_DEV(0, 0, 0), 0xa0);
	val = tom & 0xfff;
	reg = (reg & ~0xfff00000) | (val << 20);
	printk(BIOS_DEBUG, "PCI(0, 0, 0)[%x] = %x\n", 0xa0, reg);
	pcie_write_config32(PCI_DEV(0, 0, 0), 0xa0, reg);

	reg = pcie_read_config32(PCI_DEV(0, 0, 0), 0xa4);
	val = tom & 0xfffff000;
	reg = (reg & ~0x000fffff) | (val >> 12);
	printk(BIOS_DEBUG, "PCI(0, 0, 0)[%x] = %x\n", 0xa4, reg);
	pcie_write_config32(PCI_DEV(0, 0, 0), 0xa4, reg);

	// TOLUD (top of low used dram)
	reg = pcie_read_config32(PCI_DEV(0, 0, 0), 0xbc);
	val = toludbase & 0xfff;
	reg = (reg & ~0xfff00000) | (val << 20);
	printk(BIOS_DEBUG, "PCI(0, 0, 0)[%x] = %x\n", 0xbc, reg);
	pcie_write_config32(PCI_DEV(0, 0, 0), 0xbc, reg);

	// TOUUD LSB (top of upper usable dram)
	reg = pcie_read_config32(PCI_DEV(0, 0, 0), 0xa8);
	val = touudbase & 0xfff;
	reg = (reg & ~0xfff00000) | (val << 20);
	printk(BIOS_DEBUG, "PCI(0, 0, 0)[%x] = %x\n", 0xa8, reg);
	pcie_write_config32(PCI_DEV(0, 0, 0), 0xa8, reg);

	// TOUUD MSB
	reg = pcie_read_config32(PCI_DEV(0, 0, 0), 0xac);
	val = touudbase & 0xfffff000;
	reg = (reg & ~0x000fffff) | (val >> 12);
	printk(BIOS_DEBUG, "PCI(0, 0, 0)[%x] = %x\n", 0xac, reg);
	pcie_write_config32(PCI_DEV(0, 0, 0), 0xac, reg);

	if (reclaim) {
		// REMAP BASE
		pcie_write_config32(PCI_DEV(0, 0, 0), 0x90, remapbase << 20);
		pcie_write_config32(PCI_DEV(0, 0, 0), 0x94, remapbase >> 12);

		// REMAP LIMIT
		pcie_write_config32(PCI_DEV(0, 0, 0), 0x98, remaplimit << 20);
		pcie_write_config32(PCI_DEV(0, 0, 0), 0x9c, remaplimit >> 12);
	}
	// TSEG
	reg = pcie_read_config32(PCI_DEV(0, 0, 0), 0xb8);
	val = tsegbase & 0xfff;
	reg = (reg & ~0xfff00000) | (val << 20);
	printk(BIOS_DEBUG, "PCI(0, 0, 0)[%x] = %x\n", 0xb8, reg);
	pcie_write_config32(PCI_DEV(0, 0, 0), 0xb8, reg);

	// GFX stolen memory
	reg = pcie_read_config32(PCI_DEV(0, 0, 0), 0xb0);
	val = gfxstolenbase & 0xfff;
	reg = (reg & ~0xfff00000) | (val << 20);
	printk(BIOS_DEBUG, "PCI(0, 0, 0)[%x] = %x\n", 0xb0, reg);
	pcie_write_config32(PCI_DEV(0, 0, 0), 0xb0, reg);

	// GTT stolen memory
	reg = pcie_read_config32(PCI_DEV(0, 0, 0), 0xb4);
	val = gttbase & 0xfff;
	reg = (reg & ~0xfff00000) | (val << 20);
	printk(BIOS_DEBUG, "PCI(0, 0, 0)[%x] = %x\n", 0xb4, reg);
	pcie_write_config32(PCI_DEV(0, 0, 0), 0xb4, reg);

	if (me_uma_size) {
		reg = pcie_read_config32(PCI_DEV(0, 0, 0), 0x7c);
		val = (0x80000 - me_uma_size) & 0xfffff000;
		reg = (reg & ~0x000fffff) | (val >> 12);
		printk(BIOS_DEBUG, "PCI(0, 0, 0)[%x] = %x\n", 0x7c, reg);
		pcie_write_config32(PCI_DEV(0, 0, 0), 0x7c, reg);

		// ME base
		reg = pcie_read_config32(PCI_DEV(0, 0, 0), 0x70);
		val = mestolenbase & 0xfff;
		reg = (reg & ~0xfff00000) | (val << 20);
		printk(BIOS_DEBUG, "PCI(0, 0, 0)[%x] = %x\n", 0x70, reg);
		pcie_write_config32(PCI_DEV(0, 0, 0), 0x70, reg);

		reg = pcie_read_config32(PCI_DEV(0, 0, 0), 0x74);
		val = mestolenbase & 0xfffff000;
		reg = (reg & ~0x000fffff) | (val >> 12);
		printk(BIOS_DEBUG, "PCI(0, 0, 0)[%x] = %x\n", 0x74, reg);
		pcie_write_config32(PCI_DEV(0, 0, 0), 0x74, reg);

		// ME mask
		reg = pcie_read_config32(PCI_DEV(0, 0, 0), 0x78);
		val = (0x80000 - me_uma_size) & 0xfff;
		reg = (reg & ~0xfff00000) | (val << 20);
		reg = (reg & ~0x400) | (1 << 10);	// set lockbit on ME mem

		reg = (reg & ~0x800) | (1 << 11);	// set ME memory enable
		printk(BIOS_DEBUG, "PCI(0, 0, 0)[%x] = %x\n", 0x78, reg);
		pcie_write_config32(PCI_DEV(0, 0, 0), 0x78, reg);
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

static void wait_428c(int channel)
{
	while (1) {
		if (read32(DEFAULT_MCHBAR + 0x428c + (channel << 10)) & 0x50)
			return;
	}
}

static void write_reset(ramctr_timing * ctrl)
{
	int channel, slotrank;

	/* choose a populated channel.  */
	channel = (ctrl->rankmap[0]) ? 0 : 1;

	wait_428c(channel);

	/* choose a populated rank.  */
	slotrank = (ctrl->rankmap[channel] & 1) ? 0 : 2;

	/* DRAM command ZQCS */
	write32(DEFAULT_MCHBAR + 0x4220 + 0x400 * channel, 0x0f003);
	write32(DEFAULT_MCHBAR + 0x4230 + 0x400 * channel, 0x80c01);

	write32(DEFAULT_MCHBAR + 0x4200 + 0x400 * channel,
		(slotrank << 24) | 0x60000);

	write32(DEFAULT_MCHBAR + 0x4210 + 0x400 * channel, 0);

	write32(DEFAULT_MCHBAR + 0x4284 + 0x400 * channel, 0x400001);
	wait_428c(channel);
}

static void dram_jedecreset(ramctr_timing * ctrl)
{
	u32 reg, addr;
	int channel;

	while (!(MCHBAR32(0x5084) & 0x10000));
	do {
		reg = MCHBAR32(0x428c);
	} while ((reg & 0x14) == 0);

	// Set state of memory controller
	reg = 0x112;
	MCHBAR32(0x5030) = reg;
	MCHBAR32(0x4ea0) = 0;
	reg |= 2;		//ddr reset
	MCHBAR32(0x5030) = reg;

	// Assert dimm reset signal
	reg = MCHBAR32(0x5030);
	reg &= ~0x2;
	MCHBAR32(0x5030) = reg;

	// Wait 200us
	udelay(200);

	// Deassert dimm reset signal
	MCHBAR32(0x5030) |= 2;

	// Wait 500us
	udelay(500);

	// Enable DCLK
	MCHBAR32(0x5030) |= 4;

	// XXX Wait 20ns
	udelay(1);

	FOR_ALL_CHANNELS {
		// Set valid rank CKE
		reg = 0;
		reg = (reg & ~0xf) | ctrl->rankmap[channel];
		addr = 0x400 * channel + 0x42a0;
		MCHBAR32(addr) = reg;

		// Wait 10ns for ranks to settle
		//udelay(0.01);

		reg = (reg & ~0xf0) | (ctrl->rankmap[channel] << 4);
		MCHBAR32(addr) = reg;

		// Write reset using a NOP
		write_reset(ctrl);
	}
}

static odtmap get_ODT(ramctr_timing *ctrl, u8 rank, int channel)
{
	/* Get ODT based on rankmap: */
	int dimms_per_ch = (ctrl->rankmap[channel] & 1)
					+ ((ctrl->rankmap[channel] >> 2) & 1);

	if (dimms_per_ch == 1) {
		return (const odtmap){60, 60};
	} else {
		return (const odtmap){120, 30};
	}
}

static void write_mrreg(ramctr_timing *ctrl, int channel, int slotrank,
			int reg, u32 val)
{
	wait_428c(channel);

	if (ctrl->rank_mirror[channel][slotrank]) {
		/* DDR3 Rank1 Address mirror
		 * swap the following pins:
		 * A3<->A4, A5<->A6, A7<->A8, BA0<->BA1 */
		reg = ((reg >> 1) & 1) | ((reg << 1) & 2);
		val = (val & ~0x1f8) | ((val >> 1) & 0xa8)
		    | ((val & 0xa8) << 1);
	}

	/* DRAM command MRS */
	write32(DEFAULT_MCHBAR + 0x4220 + 0x400 * channel, 0x0f000);
	write32(DEFAULT_MCHBAR + 0x4230 + 0x400 * channel, 0x41001);
	write32(DEFAULT_MCHBAR + 0x4200 + 0x400 * channel,
		(slotrank << 24) | (reg << 20) | val | 0x60000);
	write32(DEFAULT_MCHBAR + 0x4210 + 0x400 * channel, 0);

	/* DRAM command MRS */
	write32(DEFAULT_MCHBAR + 0x4224 + 0x400 * channel, 0x1f000);
	write32(DEFAULT_MCHBAR + 0x4234 + 0x400 * channel, 0x41001);
	write32(DEFAULT_MCHBAR + 0x4204 + 0x400 * channel,
		(slotrank << 24) | (reg << 20) | val | 0x60000);
	write32(DEFAULT_MCHBAR + 0x4214 + 0x400 * channel, 0);

	/* DRAM command MRS */
	write32(DEFAULT_MCHBAR + 0x4228 + 0x400 * channel, 0x0f000);
	write32(DEFAULT_MCHBAR + 0x4238 + 0x400 * channel,
		0x1001 | (ctrl->tMOD << 16));
	write32(DEFAULT_MCHBAR + 0x4208 + 0x400 * channel,
		(slotrank << 24) | (reg << 20) | val | 0x60000);
	write32(DEFAULT_MCHBAR + 0x4218 + 0x400 * channel, 0);
	write32(DEFAULT_MCHBAR + 0x4284 + 0x400 * channel, 0x80001);
}

static u32 make_mr0(ramctr_timing * ctrl, u8 rank)
{
	u16 mr0reg, mch_cas, mch_wr;
	static const u8 mch_wr_t[12] = { 1, 2, 3, 4, 0, 5, 0, 6, 0, 7, 0, 0 };

	/* DLL Reset - self clearing - set after CLK frequency has been changed */
	mr0reg = 0x100;

	// Convert CAS to MCH register friendly
	if (ctrl->CAS < 12) {
		mch_cas = (u16) ((ctrl->CAS - 4) << 1);
	} else {
		mch_cas = (u16) (ctrl->CAS - 12);
		mch_cas = ((mch_cas << 1) | 0x1);
	}

	// Convert tWR to MCH register friendly
	mch_wr = mch_wr_t[ctrl->tWR - 5];

	mr0reg = (mr0reg & ~0x4) | (mch_cas & 0x1);
	mr0reg = (mr0reg & ~0x70) | ((mch_cas & 0xe) << 3);
	mr0reg = (mr0reg & ~0xe00) | (mch_wr << 9);

	// Precharge PD - Fast (desktop) 0x1 or slow (mobile) 0x0 - mostly power-saving feature
	mr0reg = (mr0reg & ~0x1000) | (!ctrl->mobile << 12);
	return mr0reg;
}

static void dram_mr0(ramctr_timing *ctrl, u8 rank, int channel)
{
	write_mrreg(ctrl, channel, rank, 0,
				make_mr0(ctrl, rank));
}

static u32 encode_odt(u32 odt)
{
	switch (odt) {
	case 30:
		return (1 << 9) | (1 << 2);	// RZQ/8, RZQ/4
	case 60:
		return (1 << 2);	// RZQ/4
	case 120:
		return (1 << 6);	// RZQ/2
	default:
	case 0:
		return 0;
	}
}

static u32 make_mr1(ramctr_timing *ctrl, u8 rank, int channel)
{
	odtmap odt;
	u32 mr1reg;

	odt = get_ODT(ctrl, rank, channel);
	mr1reg = 0x2;

	mr1reg |= encode_odt(odt.rttnom);

	return mr1reg;
}

static void dram_mr1(ramctr_timing *ctrl, u8 rank, int channel)
{
	u16 mr1reg;

	mr1reg = make_mr1(ctrl, rank, channel);

	write_mrreg(ctrl, channel, rank, 1, mr1reg);
}

static void dram_mr2(ramctr_timing *ctrl, u8 rank, int channel)
{
	u16 pasr, cwl, mr2reg;
	odtmap odt;
	int srt;

	pasr = 0;
	cwl = ctrl->CWL - 5;
	odt = get_ODT(ctrl, rank, channel);

	srt = ctrl->extended_temperature_range && !ctrl->auto_self_refresh;

	mr2reg = 0;
	mr2reg = (mr2reg & ~0x7) | pasr;
	mr2reg = (mr2reg & ~0x38) | (cwl << 3);
	mr2reg = (mr2reg & ~0x40) | (ctrl->auto_self_refresh << 6);
	mr2reg = (mr2reg & ~0x80) | (srt << 7);
	mr2reg |= (odt.rttwr / 60) << 9;

	write_mrreg(ctrl, channel, rank, 2, mr2reg);
}

static void dram_mr3(ramctr_timing *ctrl, u8 rank, int channel)
{
	write_mrreg(ctrl, channel, rank, 3, 0);
}

static void dram_mrscommands(ramctr_timing * ctrl)
{
	u8 slotrank;
	u32 reg, addr;
	int channel;

	FOR_ALL_POPULATED_CHANNELS {
		FOR_ALL_POPULATED_RANKS {
			// MR2
			dram_mr2(ctrl, slotrank, channel);

			// MR3
			dram_mr3(ctrl, slotrank, channel);

			// MR1
			dram_mr1(ctrl, slotrank, channel);

			// MR0
			dram_mr0(ctrl, slotrank, channel);
		}
	}

	/* DRAM command NOP */
	write32(DEFAULT_MCHBAR + 0x4e20, 0x7);
	write32(DEFAULT_MCHBAR + 0x4e30, 0xf1001);
	write32(DEFAULT_MCHBAR + 0x4e00, 0x60002);
	write32(DEFAULT_MCHBAR + 0x4e10, 0);

	/* DRAM command ZQCL */
	write32(DEFAULT_MCHBAR + 0x4e24, 0x1f003);
	write32(DEFAULT_MCHBAR + 0x4e34, 0x1901001);
	write32(DEFAULT_MCHBAR + 0x4e04, 0x60400);
	write32(DEFAULT_MCHBAR + 0x4e14, 0x288);

	/* execute command queue on all channels ? */
	write32(DEFAULT_MCHBAR + 0x4e84, 0x40004);

	// Drain
	FOR_ALL_CHANNELS {
		// Wait for ref drained
		wait_428c(channel);
	}

	// Refresh enable
	MCHBAR32(0x5030) |= 8;

	FOR_ALL_POPULATED_CHANNELS {
		addr = 0x400 * channel + 0x4020;
		reg = MCHBAR32(addr);
		reg &= ~0x200000;
		MCHBAR32(addr) = reg;

		wait_428c(channel);

		slotrank = (ctrl->rankmap[channel] & 1) ? 0 : 2;

		// Drain
		wait_428c(channel);

		/* DRAM command ZQCS */
		write32(DEFAULT_MCHBAR + 0x4220 + 0x400 * channel, 0x0f003);
		write32(DEFAULT_MCHBAR + 0x4230 + 0x400 * channel, 0x659001);
		write32(DEFAULT_MCHBAR + 0x4200 + 0x400 * channel,
			(slotrank << 24) | 0x60000);
		write32(DEFAULT_MCHBAR + 0x4210 + 0x400 * channel, 0x3e0);
		write32(DEFAULT_MCHBAR + 0x4284 + 0x400 * channel, 0x1);

		// Drain
		wait_428c(channel);
	}
}

const u32 lane_registers[] = {
	0x0000, 0x0200, 0x0400, 0x0600,
	0x1000, 0x1200, 0x1400, 0x1600,
	0x0800
};

static void program_timings(ramctr_timing * ctrl, int channel)
{
	u32 reg32, reg_4024, reg_c14, reg_c18, reg_4028;
	int lane;
	int slotrank, slot;
	int full_shift = 0;
	u16 slot320c[NUM_SLOTS];

	FOR_ALL_POPULATED_RANKS {
		if (full_shift < -ctrl->timings[channel][slotrank].val_320c)
			full_shift = -ctrl->timings[channel][slotrank].val_320c;
	}

	for (slot = 0; slot < NUM_SLOTS; slot++)
		switch ((ctrl->rankmap[channel] >> (2 * slot)) & 3) {
		case 0:
		default:
			slot320c[slot] = 0x7f;
			break;
		case 1:
			slot320c[slot] =
			    ctrl->timings[channel][2 * slot + 0].val_320c +
			    full_shift;
			break;
		case 2:
			slot320c[slot] =
			    ctrl->timings[channel][2 * slot + 1].val_320c +
			    full_shift;
			break;
		case 3:
			slot320c[slot] =
			    (ctrl->timings[channel][2 * slot].val_320c +
			     ctrl->timings[channel][2 * slot +
						    1].val_320c) / 2 +
			    full_shift;
			break;
		}

	/* enable CMD XOVER */
	reg32 = get_XOVER_CMD(ctrl->rankmap[channel]);
	reg32 |= ((slot320c[0] & 0x3f) << 6) | ((slot320c[0] & 0x40) << 9);
	reg32 |= (slot320c[1] & 0x7f) << 18;
	reg32 |= (full_shift & 0x3f) | ((full_shift & 0x40) << 6);

	MCHBAR32(0x320c + 0x100 * channel) = reg32;

	/* enable CLK XOVER */
	reg_c14 = get_XOVER_CLK(ctrl->rankmap[channel]);
	reg_c18 = 0;

	FOR_ALL_POPULATED_RANKS {
		int shift =
		    ctrl->timings[channel][slotrank].val_320c + full_shift;
		int offset_val_c14;
		if (shift < 0)
			shift = 0;
		offset_val_c14 = ctrl->reg_c14_offset + shift;
		/* set CLK phase shift */
		reg_c14 |= (offset_val_c14 & 0x3f) << (6 * slotrank);
		reg_c18 |= ((offset_val_c14 >> 6) & 1) << slotrank;
	}

	MCHBAR32(0xc14 + channel * 0x100) = reg_c14;
	MCHBAR32(0xc18 + channel * 0x100) = reg_c18;

	reg_4028 = MCHBAR32(0x4028 + 0x400 * channel);
	reg_4028 &= 0xffff0000;

	reg_4024 = 0;

	FOR_ALL_POPULATED_RANKS {
		int post_timA_min_high = 7, post_timA_max_high = 0;
		int pre_timA_min_high = 7, pre_timA_max_high = 0;
		int shift_402x = 0;
		int shift =
		    ctrl->timings[channel][slotrank].val_320c + full_shift;

		if (shift < 0)
			shift = 0;

		FOR_ALL_LANES {
			if (post_timA_min_high >
			    ((ctrl->timings[channel][slotrank].lanes[lane].
			      timA + shift) >> 6))
				post_timA_min_high =
				    ((ctrl->timings[channel][slotrank].
				      lanes[lane].timA + shift) >> 6);
			if (pre_timA_min_high >
			    (ctrl->timings[channel][slotrank].lanes[lane].
			     timA >> 6))
				pre_timA_min_high =
				    (ctrl->timings[channel][slotrank].
				     lanes[lane].timA >> 6);
			if (post_timA_max_high <
			    ((ctrl->timings[channel][slotrank].lanes[lane].
			      timA + shift) >> 6))
				post_timA_max_high =
				    ((ctrl->timings[channel][slotrank].
				      lanes[lane].timA + shift) >> 6);
			if (pre_timA_max_high <
			    (ctrl->timings[channel][slotrank].lanes[lane].
			     timA >> 6))
				pre_timA_max_high =
				    (ctrl->timings[channel][slotrank].
				     lanes[lane].timA >> 6);
		}

		if (pre_timA_max_high - pre_timA_min_high <
		    post_timA_max_high - post_timA_min_high)
			shift_402x = +1;
		else if (pre_timA_max_high - pre_timA_min_high >
			 post_timA_max_high - post_timA_min_high)
			shift_402x = -1;

		reg_4028 |=
		    (ctrl->timings[channel][slotrank].val_4028 + shift_402x -
		     post_timA_min_high) << (4 * slotrank);
		reg_4024 |=
		    (ctrl->timings[channel][slotrank].val_4024 +
		     shift_402x) << (8 * slotrank);

		FOR_ALL_LANES {
			MCHBAR32(lane_registers[lane] + 0x10 + 0x100 * channel +
				 4 * slotrank)
			    =
			    (((ctrl->timings[channel][slotrank].lanes[lane].
			       timA + shift) & 0x3f)
			     |
			     ((ctrl->timings[channel][slotrank].lanes[lane].
			       rising + shift) << 8)
			     |
			     (((ctrl->timings[channel][slotrank].lanes[lane].
				timA + shift -
				(post_timA_min_high << 6)) & 0x1c0) << 10)
			     | ((ctrl->timings[channel][slotrank].lanes[lane].
				falling + shift) << 20));

			MCHBAR32(lane_registers[lane] + 0x20 + 0x100 * channel +
				 4 * slotrank)
			    =
			    (((ctrl->timings[channel][slotrank].lanes[lane].
			       timC + shift) & 0x3f)
			     |
			     (((ctrl->timings[channel][slotrank].lanes[lane].
				timB + shift) & 0x3f) << 8)
			     |
			     (((ctrl->timings[channel][slotrank].lanes[lane].
				timB + shift) & 0x1c0) << 9)
			     |
			     (((ctrl->timings[channel][slotrank].lanes[lane].
				timC + shift) & 0x40) << 13));
		}
	}
	MCHBAR32(0x4024 + 0x400 * channel) = reg_4024;
	MCHBAR32(0x4028 + 0x400 * channel) = reg_4028;
}

static void test_timA(ramctr_timing * ctrl, int channel, int slotrank)
{
	wait_428c(channel);

	/* DRAM command MRS
	 * write MR3 MPR enable
	 * in this mode only RD and RDA are allowed
	 * all reads return a predefined pattern */
	write32(DEFAULT_MCHBAR + 0x4220 + 0x400 * channel, 0x1f000);
	write32(DEFAULT_MCHBAR + 0x4230 + 0x400 * channel,
		(0xc01 | (ctrl->tMOD << 16)));
	write32(DEFAULT_MCHBAR + 0x4200 + 0x400 * channel,
		(slotrank << 24) | 0x360004);
	write32(DEFAULT_MCHBAR + 0x4210 + 0x400 * channel, 0);

	/* DRAM command RD */
	write32(DEFAULT_MCHBAR + 0x4224 + 0x400 * channel, 0x1f105);
	write32(DEFAULT_MCHBAR + 0x4234 + 0x400 * channel, 0x4040c01);
	write32(DEFAULT_MCHBAR + 0x4204 + 0x400 * channel, (slotrank << 24));
	write32(DEFAULT_MCHBAR + 0x4214 + 0x400 * channel, 0);

	/* DRAM command RD */
	write32(DEFAULT_MCHBAR + 0x4228 + 0x400 * channel, 0x1f105);
	write32(DEFAULT_MCHBAR + 0x4238 + 0x400 * channel,
		0x100f | ((ctrl->CAS + 36) << 16));
	write32(DEFAULT_MCHBAR + 0x4208 + 0x400 * channel,
		(slotrank << 24) | 0x60000);
	write32(DEFAULT_MCHBAR + 0x4218 + 0x400 * channel, 0);

	/* DRAM command MRS
	 * write MR3 MPR disable */
	write32(DEFAULT_MCHBAR + 0x422c + 0x400 * channel, 0x1f000);
	write32(DEFAULT_MCHBAR + 0x423c + 0x400 * channel,
		(0xc01 | (ctrl->tMOD << 16)));
	write32(DEFAULT_MCHBAR + 0x420c + 0x400 * channel,
		(slotrank << 24) | 0x360000);
	write32(DEFAULT_MCHBAR + 0x421c + 0x400 * channel, 0);

	write32(DEFAULT_MCHBAR + 0x4284 + 0x400 * channel, 0xc0001);

	wait_428c(channel);
}

static int does_lane_work(ramctr_timing * ctrl, int channel, int slotrank,
			  int lane)
{
	u32 timA = ctrl->timings[channel][slotrank].lanes[lane].timA;
	return ((read32
		 (DEFAULT_MCHBAR + lane_registers[lane] + channel * 0x100 + 4 +
		  ((timA / 32) & 1) * 4)
		 >> (timA % 32)) & 1);
}

struct run {
	int middle;
	int end;
	int start;
	int all;
	int length;
};

static struct run get_longest_zero_run(int *seq, int sz)
{
	int i, ls;
	int bl = 0, bs = 0;
	struct run ret;

	ls = 0;
	for (i = 0; i < 2 * sz; i++)
		if (seq[i % sz]) {
			if (i - ls > bl) {
				bl = i - ls;
				bs = ls;
			}
			ls = i + 1;
		}
	if (bl == 0) {
		ret.middle = sz / 2;
		ret.start = 0;
		ret.end = sz;
		ret.all = 1;
		return ret;
	}

	ret.start = bs % sz;
	ret.end = (bs + bl - 1) % sz;
	ret.middle = (bs + (bl - 1) / 2) % sz;
	ret.length = bl;
	ret.all = 0;

	return ret;
}

static void discover_timA_coarse(ramctr_timing * ctrl, int channel,
				 int slotrank, int *upperA)
{
	int timA;
	int statistics[NUM_LANES][128];
	int lane;

	for (timA = 0; timA < 128; timA++) {
		FOR_ALL_LANES {
			ctrl->timings[channel][slotrank].lanes[lane].timA = timA;
		}
		program_timings(ctrl, channel);

		test_timA(ctrl, channel, slotrank);

		FOR_ALL_LANES {
			statistics[lane][timA] =
			    !does_lane_work(ctrl, channel, slotrank, lane);
			printram("Astat: %d, %d, %d: %x, %x\n",
			       channel, slotrank, lane, timA,
			       statistics[lane][timA]);
		}
	}
	FOR_ALL_LANES {
		struct run rn = get_longest_zero_run(statistics[lane], 128);
		ctrl->timings[channel][slotrank].lanes[lane].timA = rn.middle;
		upperA[lane] = rn.end;
		if (upperA[lane] < rn.middle)
			upperA[lane] += 128;
		printram("Aval: %d, %d, %d: %x\n", channel, slotrank,
		       lane, ctrl->timings[channel][slotrank].lanes[lane].timA);
		printram("Aend: %d, %d, %d: %x\n", channel, slotrank,
		       lane, upperA[lane]);
	}
}

static void discover_timA_fine(ramctr_timing * ctrl, int channel, int slotrank,
			       int *upperA)
{
	int timA_delta;
	int statistics[NUM_LANES][51];
	int lane, i;

	memset(statistics, 0, sizeof(statistics));

	for (timA_delta = -25; timA_delta <= 25; timA_delta++) {
		FOR_ALL_LANES ctrl->timings[channel][slotrank].lanes[lane].
		    timA = upperA[lane] + timA_delta + 0x40;
		program_timings(ctrl, channel);

		for (i = 0; i < 100; i++) {
			test_timA(ctrl, channel, slotrank);
			FOR_ALL_LANES {
				statistics[lane][timA_delta + 25] +=
				    does_lane_work(ctrl, channel, slotrank,
						   lane);
			}
		}
	}
	FOR_ALL_LANES {
		int last_zero, first_all;

		for (last_zero = -25; last_zero <= 25; last_zero++)
			if (statistics[lane][last_zero + 25])
				break;
		last_zero--;
		for (first_all = -25; first_all <= 25; first_all++)
			if (statistics[lane][first_all + 25] == 100)
				break;

		printram("lane %d: %d, %d\n", lane, last_zero,
		       first_all);

		ctrl->timings[channel][slotrank].lanes[lane].timA =
		    (last_zero + first_all) / 2 + upperA[lane];
		printram("Aval: %d, %d, %d: %x\n", channel, slotrank,
		       lane, ctrl->timings[channel][slotrank].lanes[lane].timA);
	}
}

static int discover_402x(ramctr_timing *ctrl, int channel, int slotrank,
			  int *upperA)
{
	int works[NUM_LANES];
	int lane;
	while (1) {
		int all_works = 1, some_works = 0;
		program_timings(ctrl, channel);
		test_timA(ctrl, channel, slotrank);
		FOR_ALL_LANES {
			works[lane] =
			    !does_lane_work(ctrl, channel, slotrank, lane);
			if (works[lane])
				some_works = 1;
			else
				all_works = 0;
		}
		if (all_works)
			return 0;
		if (!some_works) {
			if (ctrl->timings[channel][slotrank].val_4024 < 2) {
				printk(BIOS_EMERG, "402x discovery failed (1): %d, %d\n",
				       channel, slotrank);
				return MAKE_ERR;
			}
			ctrl->timings[channel][slotrank].val_4024 -= 2;
			printram("4024 -= 2;\n");
			continue;
		}
		ctrl->timings[channel][slotrank].val_4028 += 2;
		printram("4028 += 2;\n");
		if (ctrl->timings[channel][slotrank].val_4028 >= 0x10) {
			printk(BIOS_EMERG, "402x discovery failed (2): %d, %d\n",
			       channel, slotrank);
			return MAKE_ERR;
		}
		FOR_ALL_LANES if (works[lane]) {
			ctrl->timings[channel][slotrank].lanes[lane].timA +=
			    128;
			upperA[lane] += 128;
			printram("increment %d, %d, %d\n", channel,
			       slotrank, lane);
		}
	}
	return 0;
}

struct timA_minmax {
	int timA_min_high, timA_max_high;
};

static void pre_timA_change(ramctr_timing * ctrl, int channel, int slotrank,
			    struct timA_minmax *mnmx)
{
	int lane;
	mnmx->timA_min_high = 7;
	mnmx->timA_max_high = 0;

	FOR_ALL_LANES {
		if (mnmx->timA_min_high >
		    (ctrl->timings[channel][slotrank].lanes[lane].timA >> 6))
			mnmx->timA_min_high =
			    (ctrl->timings[channel][slotrank].lanes[lane].
			     timA >> 6);
		if (mnmx->timA_max_high <
		    (ctrl->timings[channel][slotrank].lanes[lane].timA >> 6))
			mnmx->timA_max_high =
			    (ctrl->timings[channel][slotrank].lanes[lane].
			     timA >> 6);
	}
}

static void post_timA_change(ramctr_timing * ctrl, int channel, int slotrank,
			     struct timA_minmax *mnmx)
{
	struct timA_minmax post;
	int shift_402x = 0;

	/* Get changed maxima.  */
	pre_timA_change(ctrl, channel, slotrank, &post);

	if (mnmx->timA_max_high - mnmx->timA_min_high <
	    post.timA_max_high - post.timA_min_high)
		shift_402x = +1;
	else if (mnmx->timA_max_high - mnmx->timA_min_high >
		 post.timA_max_high - post.timA_min_high)
		shift_402x = -1;
	else
		shift_402x = 0;

	ctrl->timings[channel][slotrank].val_4028 += shift_402x;
	ctrl->timings[channel][slotrank].val_4024 += shift_402x;
	printram("4024 += %d;\n", shift_402x);
	printram("4028 += %d;\n", shift_402x);
}

/* Compensate the skew between DQS and DQs.
 * To ease PCB design a small skew between Data Strobe signals and
 * Data Signals is allowed.
 * The controller has to measure and compensate this skew for every byte-lane.
 * By delaying either all DQs signals or DQS signal, a full phase
 * shift can be introduced.
 * It is assumed that one byte-lane's DQs signals have the same routing delay.
 *
 * To measure the actual skew, the DRAM is placed in "read leveling" mode.
 * In read leveling mode the DRAM-chip outputs an alternating periodic pattern.
 * The memory controller iterates over all possible values to do a full phase shift
 * and issues read commands.
 * With DQS and DQs in phase the data read is expected to alternate on every byte:
 * 0xFF 0x00 0xFF ...
 * Once the controller has detected this pattern a bit in the result register is
 * set for the current phase shift.
 */
static int read_training(ramctr_timing * ctrl)
{
	int channel, slotrank, lane;
	int err;

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS {
		int all_high, some_high;
		int upperA[NUM_LANES];
		struct timA_minmax mnmx;

		 wait_428c(channel);

		 /* DRAM command PREA */
		 write32(DEFAULT_MCHBAR + 0x4220 + 0x400 * channel, 0x1f002);
		 write32(DEFAULT_MCHBAR + 0x4230 + 0x400 * channel,
			 0xc01 | (ctrl->tRP << 16));
		 write32(DEFAULT_MCHBAR + 0x4200 + 0x400 * channel,
			 (slotrank << 24) | 0x60400);
		 write32(DEFAULT_MCHBAR + 0x4210 + 0x400 * channel, 0);
		 write32(DEFAULT_MCHBAR + 0x4284 + 0x400 * channel, 1);

		 write32(DEFAULT_MCHBAR + 0x3400, (slotrank << 2) | 0x8001);

		 ctrl->timings[channel][slotrank].val_4028 = 4;
		 ctrl->timings[channel][slotrank].val_4024 = 55;
		 program_timings(ctrl, channel);

		 discover_timA_coarse(ctrl, channel, slotrank, upperA);

		 all_high = 1;
		 some_high = 0;
		 FOR_ALL_LANES {
			 if (ctrl->timings[channel][slotrank].lanes[lane].
			     timA >= 0x40)
				 some_high = 1;
			 else
				 all_high = 0;
		 }

		if (all_high) {
			ctrl->timings[channel][slotrank].val_4028--;
			printram("4028--;\n");
			FOR_ALL_LANES {
				ctrl->timings[channel][slotrank].lanes[lane].
				    timA -= 0x40;
				upperA[lane] -= 0x40;

			}
		} else if (some_high) {
			ctrl->timings[channel][slotrank].val_4024++;
			ctrl->timings[channel][slotrank].val_4028++;
			printram("4024++;\n");
			printram("4028++;\n");
		}

		program_timings(ctrl, channel);

		pre_timA_change(ctrl, channel, slotrank, &mnmx);

		err = discover_402x(ctrl, channel, slotrank, upperA);
		if (err)
			return err;

		post_timA_change(ctrl, channel, slotrank, &mnmx);
		pre_timA_change(ctrl, channel, slotrank, &mnmx);

		discover_timA_fine(ctrl, channel, slotrank, upperA);

		post_timA_change(ctrl, channel, slotrank, &mnmx);
		pre_timA_change(ctrl, channel, slotrank, &mnmx);

		FOR_ALL_LANES {
			ctrl->timings[channel][slotrank].lanes[lane].timA -= mnmx.timA_min_high * 0x40;
		}
		ctrl->timings[channel][slotrank].val_4028 -= mnmx.timA_min_high;
		printram("4028 -= %d;\n", mnmx.timA_min_high);

		post_timA_change(ctrl, channel, slotrank, &mnmx);

		printram("4/8: %d, %d, %x, %x\n", channel, slotrank,
		       ctrl->timings[channel][slotrank].val_4024,
		       ctrl->timings[channel][slotrank].val_4028);

		printram("final results:\n");
		FOR_ALL_LANES
		    printram("Aval: %d, %d, %d: %x\n", channel, slotrank,
			   lane,
			   ctrl->timings[channel][slotrank].lanes[lane].timA);

		write32(DEFAULT_MCHBAR + 0x3400, 0);

		toggle_io_reset();
	}

	FOR_ALL_POPULATED_CHANNELS {
		program_timings(ctrl, channel);
	}
	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
		write32(DEFAULT_MCHBAR + 0x4080 + 0x400 * channel
			+ 4 * lane, 0);
	}
	return 0;
}

static void test_timC(ramctr_timing * ctrl, int channel, int slotrank)
{
	int lane;

	FOR_ALL_LANES {
		write32(DEFAULT_MCHBAR + 0x4340 + 0x400 * channel + 4 * lane, 0);
		read32(DEFAULT_MCHBAR + 0x4140 + 0x400 * channel + 4 * lane);
	}

	wait_428c(channel);

	/* DRAM command ACT */
	write32(DEFAULT_MCHBAR + 0x4220 + 0x400 * channel, 0x1f006);
	write32(DEFAULT_MCHBAR + 0x4230 + 0x400 * channel,
		(max((ctrl->tFAW >> 2) + 1, ctrl->tRRD) << 10)
		| 4 | (ctrl->tRCD << 16));

	write32(DEFAULT_MCHBAR + 0x4200 + 0x400 * channel,
		(slotrank << 24) | (6 << 16));

	write32(DEFAULT_MCHBAR + 0x4210 + 0x400 * channel, 0x244);

	/* DRAM command NOP */
	write32(DEFAULT_MCHBAR + 0x4224 + 0x400 * channel, 0x1f207);
	write32(DEFAULT_MCHBAR + 0x4234 + 0x400 * channel, 0x8041001);
	write32(DEFAULT_MCHBAR + 0x4204 + 0x400 * channel,
		(slotrank << 24) | 8);
	write32(DEFAULT_MCHBAR + 0x4214 + 0x400 * channel, 0x3e0);

	/* DRAM command WR */
	write32(DEFAULT_MCHBAR + 0x4228 + 0x400 * channel, 0x1f201);
	write32(DEFAULT_MCHBAR + 0x4238 + 0x400 * channel, 0x80411f4);
	write32(DEFAULT_MCHBAR + 0x4208 + 0x400 * channel, (slotrank << 24));
	write32(DEFAULT_MCHBAR + 0x4218 + 0x400 * channel, 0x242);

	/* DRAM command NOP */
	write32(DEFAULT_MCHBAR + 0x422c + 0x400 * channel, 0x1f207);
	write32(DEFAULT_MCHBAR + 0x423c + 0x400 * channel,
		0x8000c01 | ((ctrl->CWL + ctrl->tWTR + 5) << 16));
	write32(DEFAULT_MCHBAR + 0x420c + 0x400 * channel,
		(slotrank << 24) | 8);
	write32(DEFAULT_MCHBAR + 0x421c + 0x400 * channel, 0x3e0);

	write32(DEFAULT_MCHBAR + 0x4284 + 0x400 * channel, 0xc0001);

	wait_428c(channel);

	/* DRAM command PREA */
	write32(DEFAULT_MCHBAR + 0x4220 + 0x400 * channel, 0x1f002);
	write32(DEFAULT_MCHBAR + 0x4230 + 0x400 * channel,
		0xc01 | (ctrl->tRP << 16));
	write32(DEFAULT_MCHBAR + 0x4200 + 0x400 * channel,
		(slotrank << 24) | 0x60400);
	write32(DEFAULT_MCHBAR + 0x4210 + 0x400 * channel, 0x240);

	/* DRAM command ACT */
	write32(DEFAULT_MCHBAR + 0x4224 + 0x400 * channel, 0x1f006);
	write32(DEFAULT_MCHBAR + 0x4234 + 0x400 * channel,
		(max(ctrl->tRRD, (ctrl->tFAW >> 2) + 1) << 10)
		| 8 | (ctrl->CAS << 16));

	write32(DEFAULT_MCHBAR + 0x4204 + 0x400 * channel,
		(slotrank << 24) | 0x60000);

	write32(DEFAULT_MCHBAR + 0x4214 + 0x400 * channel, 0x244);

	/* DRAM command RD */
	write32(DEFAULT_MCHBAR + 0x4228 + 0x400 * channel, 0x1f105);
	write32(DEFAULT_MCHBAR + 0x4238 + 0x400 * channel,
		0x40011f4 | (max(ctrl->tRTP, 8) << 16));
	write32(DEFAULT_MCHBAR + 0x4208 + 0x400 * channel, (slotrank << 24));
	write32(DEFAULT_MCHBAR + 0x4218 + 0x400 * channel, 0x242);

	/* DRAM command PREA */
	write32(DEFAULT_MCHBAR + 0x422c + 0x400 * channel, 0x1f002);
	write32(DEFAULT_MCHBAR + 0x423c + 0x400 * channel,
		0xc01 | (ctrl->tRP << 16));
	write32(DEFAULT_MCHBAR + 0x420c + 0x400 * channel,
		(slotrank << 24) | 0x60400);
	write32(DEFAULT_MCHBAR + 0x421c + 0x400 * channel, 0x240);
	write32(DEFAULT_MCHBAR + 0x4284 + 0x400 * channel, 0xc0001);
	wait_428c(channel);
}

static int discover_timC(ramctr_timing *ctrl, int channel, int slotrank)
{
	int timC;
	int statistics[NUM_LANES][MAX_TIMC + 1];
	int lane;

	wait_428c(channel);

	/* DRAM command PREA */
	write32(DEFAULT_MCHBAR + 0x4220 + 0x400 * channel, 0x1f002);
	write32(DEFAULT_MCHBAR + 0x4230 + 0x400 * channel,
		0xc01 | (ctrl->tRP << 16));
	write32(DEFAULT_MCHBAR + 0x4200 + 0x400 * channel,
		(slotrank << 24) | 0x60400);
	write32(DEFAULT_MCHBAR + 0x4210 + 0x400 * channel, 0x240);
	write32(DEFAULT_MCHBAR + 0x4284 + 0x400 * channel, 1);

	for (timC = 0; timC <= MAX_TIMC; timC++) {
		FOR_ALL_LANES ctrl->timings[channel][slotrank].lanes[lane].
		    timC = timC;
		program_timings(ctrl, channel);

		test_timC(ctrl, channel, slotrank);

		FOR_ALL_LANES {
			statistics[lane][timC] =
			    read32(DEFAULT_MCHBAR + 0x4340 + 4 * lane +
				   0x400 * channel);
			printram("Cstat: %d, %d, %d, %x, %x\n",
			       channel, slotrank, lane, timC,
			       statistics[lane][timC]);
		}
	}
	FOR_ALL_LANES {
		struct run rn =
		    get_longest_zero_run(statistics[lane], MAX_TIMC + 1);
		ctrl->timings[channel][slotrank].lanes[lane].timC = rn.middle;
		if (rn.all) {
			printk(BIOS_EMERG, "timC discovery failed: %d, %d, %d\n",
			       channel, slotrank, lane);
			return MAKE_ERR;
		}
		printram("Cval: %d, %d, %d: %x\n", channel, slotrank,
		       lane, ctrl->timings[channel][slotrank].lanes[lane].timC);
	}
	return 0;
}

static int get_precedening_channels(ramctr_timing * ctrl, int target_channel)
{
	int channel, ret = 0;
	FOR_ALL_POPULATED_CHANNELS if (channel < target_channel)
		 ret++;
	return ret;
}

static void fill_pattern0(ramctr_timing * ctrl, int channel, u32 a, u32 b)
{
	unsigned j;
	unsigned channel_offset =
	    get_precedening_channels(ctrl, channel) * 0x40;
	for (j = 0; j < 16; j++)
		write32((void *)(0x04000000 + channel_offset + 4 * j), j & 2 ? b : a);
	sfence();
}

static int num_of_channels(const ramctr_timing * ctrl)
{
	int ret = 0;
	int channel;
	FOR_ALL_POPULATED_CHANNELS ret++;
	return ret;
}

static void fill_pattern1(ramctr_timing * ctrl, int channel)
{
	unsigned j;
	unsigned channel_offset =
	    get_precedening_channels(ctrl, channel) * 0x40;
	unsigned channel_step = 0x40 * num_of_channels(ctrl);
	for (j = 0; j < 16; j++)
		write32((void *)(0x04000000 + channel_offset + j * 4), 0xffffffff);
	for (j = 0; j < 16; j++)
		write32((void *)(0x04000000 + channel_offset + channel_step + j * 4), 0);
	sfence();
}

static void precharge(ramctr_timing * ctrl)
{
	int channel, slotrank, lane;

	FOR_ALL_POPULATED_CHANNELS {
		FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
			ctrl->timings[channel][slotrank].lanes[lane].falling =
			    16;
			ctrl->timings[channel][slotrank].lanes[lane].rising =
			    16;
		}

		program_timings(ctrl, channel);

		FOR_ALL_POPULATED_RANKS {
			wait_428c(channel);

			/* DRAM command MRS
			 * write MR3 MPR enable
			 * in this mode only RD and RDA are allowed
			 * all reads return a predefined pattern */
			write32(DEFAULT_MCHBAR + 0x4220 + 0x400 * channel,
				0x1f000);
			write32(DEFAULT_MCHBAR + 0x4230 + 0x400 * channel,
				0xc01 | (ctrl->tMOD << 16));
			write32(DEFAULT_MCHBAR + 0x4200 + 0x400 * channel,
				(slotrank << 24) | 0x360004);
			write32(DEFAULT_MCHBAR + 0x4210 + 0x400 * channel, 0);

			/* DRAM command RD */
			write32(DEFAULT_MCHBAR + 0x4224 + 0x400 * channel,
				0x1f105);
			write32(DEFAULT_MCHBAR + 0x4234 + 0x400 * channel,
				0x4041003);
			write32(DEFAULT_MCHBAR + 0x4204 + 0x400 * channel,
				(slotrank << 24) | 0);
			write32(DEFAULT_MCHBAR + 0x4214 + 0x400 * channel, 0);

			/* DRAM command RD */
			write32(DEFAULT_MCHBAR + 0x4228 + 0x400 * channel,
				0x1f105);
			write32(DEFAULT_MCHBAR + 0x4238 + 0x400 * channel,
				0x1001 | ((ctrl->CAS + 8) << 16));
			write32(DEFAULT_MCHBAR + 0x4208 + 0x400 * channel,
				(slotrank << 24) | 0x60000);
			write32(DEFAULT_MCHBAR + 0x4218 + 0x400 * channel, 0);

			/* DRAM command MRS
			 * write MR3 MPR disable */
			write32(DEFAULT_MCHBAR + 0x422c + 0x400 * channel,
				0x1f000);
			write32(DEFAULT_MCHBAR + 0x423c + 0x400 * channel,
				0xc01 | (ctrl->tMOD << 16));
			write32(DEFAULT_MCHBAR + 0x420c + 0x400 * channel,
				(slotrank << 24) | 0x360000);
			write32(DEFAULT_MCHBAR + 0x421c + 0x400 * channel, 0);
			write32(DEFAULT_MCHBAR + 0x4284 + 0x400 * channel,
				0xc0001);

			wait_428c(channel);
		}

		FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
			ctrl->timings[channel][slotrank].lanes[lane].falling =
			    48;
			ctrl->timings[channel][slotrank].lanes[lane].rising =
			    48;
		}

		program_timings(ctrl, channel);

		FOR_ALL_POPULATED_RANKS {
			wait_428c(channel);
			/* DRAM command MRS
			 * write MR3 MPR enable
			 * in this mode only RD and RDA are allowed
			 * all reads return a predefined pattern */
			write32(DEFAULT_MCHBAR + 0x4220 + 0x400 * channel,
				0x1f000);
			write32(DEFAULT_MCHBAR + 0x4230 + 0x400 * channel,
				0xc01 | (ctrl->tMOD << 16));
			write32(DEFAULT_MCHBAR + 0x4200 + 0x400 * channel,
				(slotrank << 24) | 0x360004);
			write32(DEFAULT_MCHBAR + 0x4210 + 0x400 * channel, 0);

			/* DRAM command RD */
			write32(DEFAULT_MCHBAR + 0x4224 + 0x400 * channel,
				0x1f105);
			write32(DEFAULT_MCHBAR + 0x4234 + 0x400 * channel,
				0x4041003);
			write32(DEFAULT_MCHBAR + 0x4204 + 0x400 * channel,
				(slotrank << 24) | 0);
			write32(DEFAULT_MCHBAR + 0x4214 + 0x400 * channel, 0);

			/* DRAM command RD */
			write32(DEFAULT_MCHBAR + 0x4228 + 0x400 * channel,
				0x1f105);
			write32(DEFAULT_MCHBAR + 0x4238 + 0x400 * channel,
				0x1001 | ((ctrl->CAS + 8) << 16));
			write32(DEFAULT_MCHBAR + 0x4208 + 0x400 * channel,
				(slotrank << 24) | 0x60000);
			write32(DEFAULT_MCHBAR + 0x4218 + 0x400 * channel, 0);

			/* DRAM command MRS
			 * write MR3 MPR disable */
			write32(DEFAULT_MCHBAR + 0x422c + 0x400 * channel,
				0x1f000);
			write32(DEFAULT_MCHBAR + 0x423c + 0x400 * channel,
				0xc01 | (ctrl->tMOD << 16));

			write32(DEFAULT_MCHBAR + 0x420c + 0x400 * channel,
				(slotrank << 24) | 0x360000);
			write32(DEFAULT_MCHBAR + 0x421c + 0x400 * channel, 0);

			write32(DEFAULT_MCHBAR + 0x4284 + 0x400 * channel,
				0xc0001);
			wait_428c(channel);
		}
	}
}

static void test_timB(ramctr_timing * ctrl, int channel, int slotrank)
{
	/* enable DQs on this slotrank */
	write_mrreg(ctrl, channel, slotrank, 1,
		    0x80 | make_mr1(ctrl, slotrank, channel));

	wait_428c(channel);
	/* DRAM command NOP */
	write32(DEFAULT_MCHBAR + 0x4220 + 0x400 * channel, 0x1f207);
	write32(DEFAULT_MCHBAR + 0x4230 + 0x400 * channel,
		0x8000c01 | ((ctrl->CWL + ctrl->tWLO) << 16));
	write32(DEFAULT_MCHBAR + 0x4200 + 0x400 * channel,
		8 | (slotrank << 24));
	write32(DEFAULT_MCHBAR + 0x4210 + 0x400 * channel, 0);

	/* DRAM command NOP */
	write32(DEFAULT_MCHBAR + 0x4224 + 0x400 * channel, 0x1f107);
	write32(DEFAULT_MCHBAR + 0x4234 + 0x400 * channel,
		0x4000c01 | ((ctrl->CAS + 38) << 16));
	write32(DEFAULT_MCHBAR + 0x4204 + 0x400 * channel,
		(slotrank << 24) | 4);
	write32(DEFAULT_MCHBAR + 0x4214 + 0x400 * channel, 0);

	write32(DEFAULT_MCHBAR + 0x400 * channel + 0x4284, 0x40001);
	wait_428c(channel);

	/* disable DQs on this slotrank */
	write_mrreg(ctrl, channel, slotrank, 1,
		    0x1080 | make_mr1(ctrl, slotrank, channel));
}

static int discover_timB(ramctr_timing *ctrl, int channel, int slotrank)
{
	int timB;
	int statistics[NUM_LANES][128];
	int lane;

	write32(DEFAULT_MCHBAR + 0x3400, 0x108052 | (slotrank << 2));

	for (timB = 0; timB < 128; timB++) {
		FOR_ALL_LANES {
			ctrl->timings[channel][slotrank].lanes[lane].timB = timB;
		}
		program_timings(ctrl, channel);

		test_timB(ctrl, channel, slotrank);

		FOR_ALL_LANES {
			statistics[lane][timB] =
			    !((read32
			       (DEFAULT_MCHBAR + lane_registers[lane] +
				channel * 0x100 + 4 + ((timB / 32) & 1) * 4)
			       >> (timB % 32)) & 1);
			printram("Bstat: %d, %d, %d: %x, %x\n",
			       channel, slotrank, lane, timB,
			       statistics[lane][timB]);
		}
	}
	FOR_ALL_LANES {
		struct run rn = get_longest_zero_run(statistics[lane], 128);
		/* timC is a direct function of timB's 6 LSBs.
		 * Some tests increments the value of timB by a small value,
		 * which might cause the 6bit value to overflow, if it's close
		 * to 0x3F. Increment the value by a small offset if it's likely
		 * to overflow, to make sure it won't overflow while running
		 * tests and bricks the system due to a non matching timC.
		 *
		 * TODO: find out why some tests (edge write discovery)
		 *       increment timB. */
		if ((rn.start & 0x3F) == 0x3E)
			rn.start += 2;
		else if ((rn.start & 0x3F) == 0x3F)
			rn.start += 1;
		ctrl->timings[channel][slotrank].lanes[lane].timB = rn.start;
		if (rn.all) {
			printk(BIOS_EMERG, "timB discovery failed: %d, %d, %d\n",
			       channel, slotrank, lane);
			return MAKE_ERR;
		}
		printram("Bval: %d, %d, %d: %x\n", channel, slotrank,
		       lane, ctrl->timings[channel][slotrank].lanes[lane].timB);
	}
	return 0;
}

static int get_timB_high_adjust(u64 val)
{
	int i;

	/* good */
	if (val == 0xffffffffffffffffLL)
		return 0;

	if (val >= 0xf000000000000000LL) {
		/* needs negative adjustment */
		for (i = 0; i < 8; i++)
			if (val << (8 * (7 - i) + 4))
				return -i;
	} else {
		/* needs positive adjustment */
		for (i = 0; i < 8; i++)
			if (val >> (8 * (7 - i) + 4))
				return i;
	}
	return 8;
}

static void adjust_high_timB(ramctr_timing * ctrl)
{
	int channel, slotrank, lane, old;
	write32(DEFAULT_MCHBAR + 0x3400, 0x200);
	FOR_ALL_POPULATED_CHANNELS {
		fill_pattern1(ctrl, channel);
		write32(DEFAULT_MCHBAR + 0x4288 + (channel << 10), 1);
	}
	FOR_ALL_POPULATED_CHANNELS FOR_ALL_POPULATED_RANKS {

		write32(DEFAULT_MCHBAR + 0x4288 + 0x400 * channel, 0x10001);

		wait_428c(channel);

		/* DRAM command ACT */
		write32(DEFAULT_MCHBAR + 0x4220 + 0x400 * channel, 0x1f006);
		write32(DEFAULT_MCHBAR + 0x4230 + 0x400 * channel,
			0xc01 | (ctrl->tRCD << 16));
		write32(DEFAULT_MCHBAR + 0x4200 + 0x400 * channel,
			(slotrank << 24) | 0x60000);
		write32(DEFAULT_MCHBAR + 0x4210 + 0x400 * channel, 0);

		/* DRAM command NOP */
		write32(DEFAULT_MCHBAR + 0x4224 + 0x400 * channel, 0x1f207);
		write32(DEFAULT_MCHBAR + 0x4234 + 0x400 * channel, 0x8040c01);
		write32(DEFAULT_MCHBAR + 0x4204 + 0x400 * channel,
			(slotrank << 24) | 0x8);
		write32(DEFAULT_MCHBAR + 0x4214 + 0x400 * channel, 0x3e0);

		/* DRAM command WR */
		write32(DEFAULT_MCHBAR + 0x4228 + 0x400 * channel, 0x1f201);
		write32(DEFAULT_MCHBAR + 0x4238 + 0x400 * channel, 0x8041003);
		write32(DEFAULT_MCHBAR + 0x4208 + 0x400 * channel,
			(slotrank << 24));
		write32(DEFAULT_MCHBAR + 0x4218 + 0x400 * channel, 0x3e2);

		/* DRAM command NOP */
		write32(DEFAULT_MCHBAR + 0x422c + 0x400 * channel, 0x1f207);
		write32(DEFAULT_MCHBAR + 0x423c + 0x400 * channel,
			0x8000c01 | ((ctrl->CWL + ctrl->tWTR + 5) << 16));
		write32(DEFAULT_MCHBAR + 0x420c + 0x400 * channel,
			(slotrank << 24) | 0x8);
		write32(DEFAULT_MCHBAR + 0x421c + 0x400 * channel, 0x3e0);

		write32(DEFAULT_MCHBAR + 0x4284 + 0x400 * channel, 0xc0001);

		wait_428c(channel);

		/* DRAM command PREA */
		write32(DEFAULT_MCHBAR + 0x4220 + 0x400 * channel, 0x1f002);
		write32(DEFAULT_MCHBAR + 0x4230 + 0x400 * channel,
			0xc01 | ((ctrl->tRP) << 16));
		write32(DEFAULT_MCHBAR + 0x4200 + 0x400 * channel,
			(slotrank << 24) | 0x60400);
		write32(DEFAULT_MCHBAR + 0x4210 + 0x400 * channel, 0x240);

		/* DRAM command ACT */
		write32(DEFAULT_MCHBAR + 0x4224 + 0x400 * channel, 0x1f006);
		write32(DEFAULT_MCHBAR + 0x4234 + 0x400 * channel,
			0xc01 | ((ctrl->tRCD) << 16));
		write32(DEFAULT_MCHBAR + 0x4204 + 0x400 * channel,
			(slotrank << 24) | 0x60000);
		write32(DEFAULT_MCHBAR + 0x4214 + 0x400 * channel, 0);

		/* DRAM command RD */
		write32(DEFAULT_MCHBAR + 0x4228 + 0x400 * channel, 0x3f105);
		write32(DEFAULT_MCHBAR + 0x4238 + 0x400 * channel,
			0x4000c01 |
			((ctrl->tRP +
			  ctrl->timings[channel][slotrank].val_4024 +
			  ctrl->timings[channel][slotrank].val_4028) << 16));
		write32(DEFAULT_MCHBAR + 0x4208 + 0x400 * channel,
			(slotrank << 24) | 0x60008);
		write32(DEFAULT_MCHBAR + 0x4218 + 0x400 * channel, 0);

		write32(DEFAULT_MCHBAR + 0x4284 + 0x400 * channel, 0x80001);
		wait_428c(channel);
		FOR_ALL_LANES {
			u64 res =
				read32(DEFAULT_MCHBAR + lane_registers[lane] +
					0x100 * channel + 4);
			res |=
				((u64) read32(DEFAULT_MCHBAR + lane_registers[lane] +
					0x100 * channel + 8)) << 32;
			old = ctrl->timings[channel][slotrank].lanes[lane].timB;
			ctrl->timings[channel][slotrank].lanes[lane].timB +=
				get_timB_high_adjust(res) * 64;

			printram("High adjust %d:%016llx\n", lane, res);
			printram("Bval+: %d, %d, %d, %x -> %x\n", channel,
				slotrank, lane, old,
				ctrl->timings[channel][slotrank].lanes[lane].
				timB);
		}
	}
	write32(DEFAULT_MCHBAR + 0x3400, 0);
}

static void write_op(ramctr_timing * ctrl, int channel)
{
	int slotrank;

	wait_428c(channel);

	/* choose an existing rank.  */
	slotrank = !(ctrl->rankmap[channel] & 1) ? 2 : 0;

	/* DRAM command ACT */
	write32(DEFAULT_MCHBAR + 0x4220 + 0x400 * channel, 0x0f003);
	write32(DEFAULT_MCHBAR + 0x4230 + 0x400 * channel, 0x41001);

	write32(DEFAULT_MCHBAR + 0x4200 + 0x400 * channel,
		(slotrank << 24) | 0x60000);

	write32(DEFAULT_MCHBAR + 0x4210 + 0x400 * channel, 0x3e0);

	write32(DEFAULT_MCHBAR + 0x4284 + 0x400 * channel, 1);
	wait_428c(channel);
}

/* Compensate the skew between CMD/ADDR/CLK and DQ/DQS lanes.
 * DDR3 adopted the fly-by topology. The data and strobes signals reach
 * the chips at different times with respect to command, address and
 * clock signals.
 * By delaying either all DQ/DQs or all CMD/ADDR/CLK signals, a full phase
 * shift can be introduced.
 * It is assumed that the CLK/ADDR/CMD signals have the same routing delay.
 *
 * To find the required phase shift the DRAM is placed in "write leveling" mode.
 * In this mode the DRAM-chip samples the CLK on every DQS edge and feeds back the
 * sampled value on the data lanes (DQs).
 */
static int write_training(ramctr_timing * ctrl)
{
	int channel, slotrank, lane;
	int err;

	FOR_ALL_POPULATED_CHANNELS
	    write32(DEFAULT_MCHBAR + 0x4008 + 0x400 * channel,
		    read32(DEFAULT_MCHBAR + 0x4008 +
			   0x400 * channel) | 0x8000000);

	FOR_ALL_POPULATED_CHANNELS {
		write_op(ctrl, channel);
		write32(DEFAULT_MCHBAR + 0x4020 + 0x400 * channel,
			read32(DEFAULT_MCHBAR + 0x4020 +
			       0x400 * channel) | 0x200000);
	}

	/* refresh disable */
	write32(DEFAULT_MCHBAR + 0x5030, read32(DEFAULT_MCHBAR + 0x5030) & ~8);
	FOR_ALL_POPULATED_CHANNELS {
		write_op(ctrl, channel);
	}

	/* enable write leveling on all ranks
	 * disable all DQ outputs
	 * only NOP is allowed in this mode */
	FOR_ALL_CHANNELS
	    FOR_ALL_POPULATED_RANKS
		write_mrreg(ctrl, channel, slotrank, 1,
			    make_mr1(ctrl, slotrank, channel) | 0x1080);

	write32(DEFAULT_MCHBAR + 0x3400, 0x108052);

	toggle_io_reset();

	/* set any valid value for timB, it gets corrected later */
	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS {
		err = discover_timB(ctrl, channel, slotrank);
		if (err)
			return err;
	}

	/* disable write leveling on all ranks */
	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS
		write_mrreg(ctrl, channel,
			    slotrank, 1, make_mr1(ctrl, slotrank, channel));

	write32(DEFAULT_MCHBAR + 0x3400, 0);

	FOR_ALL_POPULATED_CHANNELS
		wait_428c(channel);

	/* refresh enable */
	write32(DEFAULT_MCHBAR + 0x5030, read32(DEFAULT_MCHBAR + 0x5030) | 8);

	FOR_ALL_POPULATED_CHANNELS {
		write32(DEFAULT_MCHBAR + 0x4020 + 0x400 * channel,
			~0x00200000 & read32(DEFAULT_MCHBAR + 0x4020 +
					     0x400 * channel));
		read32(DEFAULT_MCHBAR + 0x428c + 0x400 * channel);
		wait_428c(channel);

		/* DRAM command ZQCS */
		write32(DEFAULT_MCHBAR + 0x4220 + 0x400 * channel, 0x0f003);
		write32(DEFAULT_MCHBAR + 0x4230 + 0x400 * channel, 0x659001);
		write32(DEFAULT_MCHBAR + 0x4200 + 0x400 * channel, 0x60000);
		write32(DEFAULT_MCHBAR + 0x4210 + 0x400 * channel, 0x3e0);

		write32(DEFAULT_MCHBAR + 0x4284 + 0x400 * channel, 1);
		wait_428c(channel);
	}

	toggle_io_reset();

	printram("CPE\n");
	precharge(ctrl);
	printram("CPF\n");

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
		read32(DEFAULT_MCHBAR + 0x4080 + 0x400 * channel + 4 * lane);
		write32(DEFAULT_MCHBAR + 0x4080 + 0x400 * channel + 4 * lane,
			0);
	}

	FOR_ALL_POPULATED_CHANNELS {
		fill_pattern0(ctrl, channel, 0xaaaaaaaa, 0x55555555);
		write32(DEFAULT_MCHBAR + 0x4288 + (channel << 10), 0);
	}

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS {
		err = discover_timC(ctrl, channel, slotrank);
		if (err)
			return err;
	}

	FOR_ALL_POPULATED_CHANNELS
		program_timings(ctrl, channel);

	/* measure and adjust timB timings */
	adjust_high_timB(ctrl);

	FOR_ALL_POPULATED_CHANNELS
		program_timings(ctrl, channel);

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
		read32(DEFAULT_MCHBAR + 0x4080 + 0x400 * channel + 4 * lane);
		write32(DEFAULT_MCHBAR + 0x4080 + 0x400 * channel + 4 * lane,
			0);
	}
	return 0;
}

static int test_320c(ramctr_timing * ctrl, int channel, int slotrank)
{
	struct ram_rank_timings saved_rt = ctrl->timings[channel][slotrank];
	int timC_delta;
	int lanes_ok = 0;
	int ctr = 0;
	int lane;

	for (timC_delta = -5; timC_delta <= 5; timC_delta++) {
		FOR_ALL_LANES {
			ctrl->timings[channel][slotrank].lanes[lane].timC =
			    saved_rt.lanes[lane].timC + timC_delta;
		}
		program_timings(ctrl, channel);
		FOR_ALL_LANES {
			write32(DEFAULT_MCHBAR + 4 * lane + 0x4f40, 0);
		}

		write32(DEFAULT_MCHBAR + 0x4288 + 0x400 * channel, 0x1f);

		wait_428c(channel);
		/* DRAM command ACT */
		write32(DEFAULT_MCHBAR + 0x4220 + 0x400 * channel, 0x1f006);
		write32(DEFAULT_MCHBAR + 0x4230 + 0x400 * channel,
			((max(ctrl->tRRD, (ctrl->tFAW >> 2) + 1)) << 10)
			| 8 | (ctrl->tRCD << 16));

		write32(DEFAULT_MCHBAR + 0x4200 + 0x400 * channel,
			(slotrank << 24) | ctr | 0x60000);

		write32(DEFAULT_MCHBAR + 0x4210 + 0x400 * channel, 0x244);
		/* DRAM command WR */
		write32(DEFAULT_MCHBAR + 0x4224 + 0x400 * channel, 0x1f201);
		write32(DEFAULT_MCHBAR + 0x4234 + 0x400 * channel,
			0x8001020 | ((ctrl->CWL + ctrl->tWTR + 8) << 16));
		write32(DEFAULT_MCHBAR + 0x4204 + 0x400 * channel,
			(slotrank << 24));
		write32(DEFAULT_MCHBAR + 0x4244 + 0x400 * channel, 0x389abcd);
		write32(DEFAULT_MCHBAR + 0x4214 + 0x400 * channel, 0x20e42);

		/* DRAM command RD */
		write32(DEFAULT_MCHBAR + 0x4228 + 0x400 * channel, 0x1f105);
		write32(DEFAULT_MCHBAR + 0x4238 + 0x400 * channel,
			0x4001020 | (max(ctrl->tRTP, 8) << 16));
		write32(DEFAULT_MCHBAR + 0x4208 + 0x400 * channel,
			(slotrank << 24));
		write32(DEFAULT_MCHBAR + 0x4248 + 0x400 * channel, 0x389abcd);
		write32(DEFAULT_MCHBAR + 0x4218 + 0x400 * channel, 0x20e42);

		/* DRAM command PRE */
		write32(DEFAULT_MCHBAR + 0x422c + 0x400 * channel, 0x1f002);
		write32(DEFAULT_MCHBAR + 0x423c + 0x400 * channel, 0xf1001);
		write32(DEFAULT_MCHBAR + 0x420c + 0x400 * channel,
			(slotrank << 24) | 0x60400);
		write32(DEFAULT_MCHBAR + 0x421c + 0x400 * channel, 0x240);

		write32(DEFAULT_MCHBAR + 0x4284 + 0x400 * channel, 0xc0001);
		wait_428c(channel);
		FOR_ALL_LANES {
			u32 r32 =
			    read32(DEFAULT_MCHBAR + 0x4340 + 4 * lane +
				   0x400 * channel);

			if (r32 == 0)
				lanes_ok |= 1 << lane;
		}
		ctr++;
		if (lanes_ok == ((1 << NUM_LANES) - 1))
			break;
	}

	ctrl->timings[channel][slotrank] = saved_rt;

	printram("3lanes: %x\n", lanes_ok);
	return lanes_ok != ((1 << NUM_LANES) - 1);
}

#include "raminit_patterns.h"

static void fill_pattern5(ramctr_timing * ctrl, int channel, int patno)
{
	unsigned i, j;
	unsigned channel_offset =
	    get_precedening_channels(ctrl, channel) * 0x40;
	unsigned channel_step = 0x40 * num_of_channels(ctrl);

	if (patno) {
		u8 base8 = 0x80 >> ((patno - 1) % 8);
		u32 base = base8 | (base8 << 8) | (base8 << 16) | (base8 << 24);
		for (i = 0; i < 32; i++) {
			for (j = 0; j < 16; j++) {
				u32 val = use_base[patno - 1][i] & (1 << (j / 2)) ? base : 0;
				if (invert[patno - 1][i] & (1 << (j / 2)))
					val = ~val;
				write32((void *)(0x04000000 + channel_offset + i * channel_step +
						 j * 4), val);
			}
		}

	} else {
		for (i = 0; i < sizeof(pattern) / sizeof(pattern[0]); i++) {
			for (j = 0; j < 16; j++)
				write32((void *)(0x04000000 + channel_offset + i * channel_step +
						 j * 4), pattern[i][j]);
		}
		sfence();
	}
}

static void reprogram_320c(ramctr_timing * ctrl)
{
	int channel, slotrank;

	FOR_ALL_POPULATED_CHANNELS {
		wait_428c(channel);

		/* choose an existing rank.  */
		slotrank = !(ctrl->rankmap[channel] & 1) ? 2 : 0;

		/* DRAM command ZQCS */
		write32(DEFAULT_MCHBAR + 0x4220 + 0x400 * channel, 0x0f003);
		write32(DEFAULT_MCHBAR + 0x4230 + 0x400 * channel, 0x41001);

		write32(DEFAULT_MCHBAR + 0x4200 + 0x400 * channel,
			(slotrank << 24) | 0x60000);

		write32(DEFAULT_MCHBAR + 0x4210 + 0x400 * channel, 0x3e0);

		write32(DEFAULT_MCHBAR + 0x4284 + 0x400 * channel, 1);
		wait_428c(channel);
		write32(DEFAULT_MCHBAR + 0x4020 + 0x400 * channel,
			read32(DEFAULT_MCHBAR + 0x4020 +
			       0x400 * channel) | 0x200000);
	}

	/* refresh disable */
	write32(DEFAULT_MCHBAR + 0x5030, read32(DEFAULT_MCHBAR + 0x5030) & ~8);
	FOR_ALL_POPULATED_CHANNELS {
		wait_428c(channel);

		/* choose an existing rank.  */
		slotrank = !(ctrl->rankmap[channel] & 1) ? 2 : 0;

		/* DRAM command ZQCS */
		write32(DEFAULT_MCHBAR + 0x4220 + 0x400 * channel, 0x0f003);
		write32(DEFAULT_MCHBAR + 0x4230 + 0x400 * channel, 0x41001);

		write32(DEFAULT_MCHBAR + 0x4200 + 0x400 * channel,
			(slotrank << 24) | 0x60000);

		write32(DEFAULT_MCHBAR + 0x4210 + 0x400 * channel, 0x3e0);

		write32(DEFAULT_MCHBAR + 0x4284 + 0x400 * channel, 1);
		wait_428c(channel);
	}

	/* jedec reset */
	dram_jedecreset(ctrl);
	/* mrs commands. */
	dram_mrscommands(ctrl);

	toggle_io_reset();
}

#define MIN_C320C_LEN 13

static int try_cmd_stretch(ramctr_timing *ctrl, int cmd_stretch)
{
	struct ram_rank_timings saved_timings[NUM_CHANNELS][NUM_SLOTRANKS];
	int channel, slotrank;
	int c320c;
	int stat[NUM_SLOTRANKS][256];

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS {
		saved_timings[channel][slotrank] = ctrl->timings[channel][slotrank];
	}

	FOR_ALL_POPULATED_CHANNELS {
		ctrl->cmd_stretch[channel] = cmd_stretch;
	}

	FOR_ALL_POPULATED_CHANNELS
	    MCHBAR32(0x4004 + 0x400 * channel) =
		ctrl->tRRD
		| (ctrl->tRTP << 4)
		| (ctrl->tCKE << 8)
		| (ctrl->tWTR << 12)
		| (ctrl->tFAW << 16)
		| (ctrl->tWR << 24)
		| (ctrl->cmd_stretch[channel] << 30);


	FOR_ALL_CHANNELS {
		int delta = 0;
		if (ctrl->cmd_stretch[channel] == 2)
			delta = 2;
		else if (ctrl->cmd_stretch[channel] == 0)
			delta = 4;

		FOR_ALL_POPULATED_RANKS {
			ctrl->timings[channel][slotrank].val_4024 -= delta;
		}
	}

	FOR_ALL_POPULATED_CHANNELS {
		for (c320c = -127; c320c <= 127; c320c++) {
			FOR_ALL_POPULATED_RANKS {
				ctrl->timings[channel][slotrank].val_320c = c320c;
			}
			program_timings(ctrl, channel);
			reprogram_320c(ctrl);
			FOR_ALL_POPULATED_RANKS {
				stat[slotrank][c320c + 127] =
				    test_320c(ctrl, channel, slotrank);
				printram("3stat: %d, %d, %d: %x\n",
				       channel, slotrank, c320c,
				       stat[slotrank][c320c + 127]);
			}
		}
		FOR_ALL_POPULATED_RANKS {
			struct run rn =
			    get_longest_zero_run(stat[slotrank], 255);
			ctrl->timings[channel][slotrank].val_320c =
			    rn.middle - 127;
			printram("3val: %d, %d: %d\n", channel,
			       slotrank,
			       ctrl->timings[channel][slotrank].val_320c);
			if (rn.all || rn.length < MIN_C320C_LEN) {
				FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS {
					ctrl->timings[channel][slotrank] = saved_timings[channel][slotrank];
				}
				return MAKE_ERR;
			}
		}
	}
	return 0;
}

/* Adjust CMD phase shift and try multiple command rates.
 * A command rate of 2T doubles the time needed for address and
 * command decode. */
static int command_training(ramctr_timing *ctrl)
{
	int channel;
	int err;

	FOR_ALL_POPULATED_CHANNELS {
		fill_pattern5(ctrl, channel, 0);
		write32(DEFAULT_MCHBAR + 0x4288 + 0x400 * channel, 0x1f);
	}

	/* try command rate 1T and 2T */
	err = try_cmd_stretch(ctrl, 0);
	if (err) {
		err = try_cmd_stretch(ctrl, 2);
		if (err) {
			printk(BIOS_EMERG, "c320c discovery failed\n");
			return err;
		}
	}

	FOR_ALL_POPULATED_CHANNELS {
		program_timings(ctrl, channel);
	}

	reprogram_320c(ctrl);
	return 0;
}

static int discover_edges_real(ramctr_timing *ctrl, int channel, int slotrank,
				int *edges)
{
	int edge;
	int statistics[NUM_LANES][MAX_EDGE_TIMING + 1];
	int lane;

	for (edge = 0; edge <= MAX_EDGE_TIMING; edge++) {
		FOR_ALL_LANES {
			ctrl->timings[channel][slotrank].lanes[lane].rising =
			    edge;
			ctrl->timings[channel][slotrank].lanes[lane].falling =
			    edge;
		}
		program_timings(ctrl, channel);

		FOR_ALL_LANES {
			write32(DEFAULT_MCHBAR + 0x4340 + 0x400 * channel +
				4 * lane, 0);
			read32(DEFAULT_MCHBAR + 0x400 * channel + 4 * lane +
			       0x4140);
		}

		wait_428c(channel);
		/* DRAM command MRS
		 * write MR3 MPR enable
		 * in this mode only RD and RDA are allowed
		 * all reads return a predefined pattern */
		write32(DEFAULT_MCHBAR + 0x4220 + 0x400 * channel, 0x1f000);
		write32(DEFAULT_MCHBAR + 0x4230 + 0x400 * channel,
			(0xc01 | (ctrl->tMOD << 16)));
		write32(DEFAULT_MCHBAR + 0x4200 + 0x400 * channel,
			(slotrank << 24) | 0x360004);
		write32(DEFAULT_MCHBAR + 0x4210 + 0x400 * channel, 0);

		/* DRAM command RD */
		write32(DEFAULT_MCHBAR + 0x4224 + 0x400 * channel, 0x1f105);
		write32(DEFAULT_MCHBAR + 0x4234 + 0x400 * channel, 0x40411f4);
		write32(DEFAULT_MCHBAR + 0x4204 + 0x400 * channel,
			(slotrank << 24));
		write32(DEFAULT_MCHBAR + 0x4214 + 0x400 * channel, 0);

		/* DRAM command RD */
		write32(DEFAULT_MCHBAR + 0x4228 + 0x400 * channel, 0x1f105);
		write32(DEFAULT_MCHBAR + 0x4238 + 0x400 * channel,
			0x1001 | ((ctrl->CAS + 8) << 16));
		write32(DEFAULT_MCHBAR + 0x4208 + 0x400 * channel,
			(slotrank << 24) | 0x60000);
		write32(DEFAULT_MCHBAR + 0x4218 + 0x400 * channel, 0);

		/* DRAM command MRS
		 * MR3 disable MPR */
		write32(DEFAULT_MCHBAR + 0x422c + 0x400 * channel, 0x1f000);
		write32(DEFAULT_MCHBAR + 0x423c + 0x400 * channel,
			(0xc01 | (ctrl->tMOD << 16)));
		write32(DEFAULT_MCHBAR + 0x420c + 0x400 * channel,
			(slotrank << 24) | 0x360000);
		write32(DEFAULT_MCHBAR + 0x421c + 0x400 * channel, 0);

		write32(DEFAULT_MCHBAR + 0x4284 + 0x400 * channel, 0xc0001);

		wait_428c(channel);

		FOR_ALL_LANES {
			statistics[lane][edge] =
			    read32(DEFAULT_MCHBAR + 0x4340 + 0x400 * channel +
				   lane * 4);
		}
	}
	FOR_ALL_LANES {
		struct run rn =
		    get_longest_zero_run(statistics[lane], MAX_EDGE_TIMING + 1);
		edges[lane] = rn.middle;
		if (rn.all) {
			printk(BIOS_EMERG, "edge discovery failed: %d, %d, %d\n",
			       channel, slotrank, lane);
			return MAKE_ERR;
		}
		printram("eval %d, %d, %d: %02x\n", channel, slotrank,
		       lane, edges[lane]);
	}
	return 0;
}

static int discover_edges(ramctr_timing *ctrl)
{
	int falling_edges[NUM_CHANNELS][NUM_SLOTRANKS][NUM_LANES];
	int rising_edges[NUM_CHANNELS][NUM_SLOTRANKS][NUM_LANES];
	int channel, slotrank, lane;
	int err;

	write32(DEFAULT_MCHBAR + 0x3400, 0);

	toggle_io_reset();

	FOR_ALL_POPULATED_CHANNELS FOR_ALL_LANES {
		write32(DEFAULT_MCHBAR + 4 * lane +
			0x400 * channel + 0x4080, 0);
	}

	FOR_ALL_POPULATED_CHANNELS {
		fill_pattern0(ctrl, channel, 0, 0);
		write32(DEFAULT_MCHBAR + 0x4288 + (channel << 10), 0);
		FOR_ALL_LANES {
			read32(DEFAULT_MCHBAR + 0x400 * channel +
			       lane * 4 + 0x4140);
		}

		FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
			ctrl->timings[channel][slotrank].lanes[lane].falling =
			    16;
			ctrl->timings[channel][slotrank].lanes[lane].rising =
			    16;
		}

		program_timings(ctrl, channel);

		FOR_ALL_POPULATED_RANKS {
			wait_428c(channel);

			/* DRAM command MRS
			 * MR3 enable MPR
			 * write MR3 MPR enable
			 * in this mode only RD and RDA are allowed
			 * all reads return a predefined pattern */
			write32(DEFAULT_MCHBAR + 0x4220 + 0x400 * channel,
				0x1f000);
			write32(DEFAULT_MCHBAR + 0x4230 + 0x400 * channel,
				0xc01 | (ctrl->tMOD << 16));
			write32(DEFAULT_MCHBAR + 0x4200 + 0x400 * channel,
				(slotrank << 24) | 0x360004);
			write32(DEFAULT_MCHBAR + 0x4210 + 0x400 * channel, 0);

			/* DRAM command RD */
			write32(DEFAULT_MCHBAR + 0x4224 + 0x400 * channel,
				0x1f105);
			write32(DEFAULT_MCHBAR + 0x4234 + 0x400 * channel,
				0x4041003);
			write32(DEFAULT_MCHBAR + 0x4204 + 0x400 * channel,
				(slotrank << 24) | 0);
			write32(DEFAULT_MCHBAR + 0x4214 + 0x400 * channel, 0);

			/* DRAM command RD */
			write32(DEFAULT_MCHBAR + 0x4228 + 0x400 * channel,
				0x1f105);
			write32(DEFAULT_MCHBAR + 0x4238 + 0x400 * channel,
				0x1001 | ((ctrl->CAS + 8) << 16));
			write32(DEFAULT_MCHBAR + 0x4208 + 0x400 * channel,
				(slotrank << 24) | 0x60000);
			write32(DEFAULT_MCHBAR + 0x4218 + 0x400 * channel, 0);

			/* DRAM command MRS
			 * MR3 disable MPR */
			write32(DEFAULT_MCHBAR + 0x422c + 0x400 * channel,
				0x1f000);
			write32(DEFAULT_MCHBAR + 0x423c + 0x400 * channel,
				0xc01 | (ctrl->tMOD << 16));
			write32(DEFAULT_MCHBAR + 0x420c + 0x400 * channel,
				(slotrank << 24) | 0x360000);
			write32(DEFAULT_MCHBAR + 0x421c + 0x400 * channel, 0);
			write32(DEFAULT_MCHBAR + 0x4284 + 0x400 * channel,
				0xc0001);

			wait_428c(channel);
		}

		/* XXX: check any measured value ? */

		FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
			ctrl->timings[channel][slotrank].lanes[lane].falling =
			    48;
			ctrl->timings[channel][slotrank].lanes[lane].rising =
			    48;
		}

		program_timings(ctrl, channel);

		FOR_ALL_POPULATED_RANKS {
			wait_428c(channel);

			/* DRAM command MRS
			 * MR3 enable MPR
			 * write MR3 MPR enable
			 * in this mode only RD and RDA are allowed
			 * all reads return a predefined pattern */
			write32(DEFAULT_MCHBAR + 0x4220 + 0x400 * channel,
				0x1f000);
			write32(DEFAULT_MCHBAR + 0x4230 + 0x400 * channel,
				0xc01 | (ctrl->tMOD << 16));
			write32(DEFAULT_MCHBAR + 0x4200 + 0x400 * channel,
				(slotrank << 24) | 0x360004);
			write32(DEFAULT_MCHBAR + 0x4210 + 0x400 * channel, 0);

			/* DRAM command RD */
			write32(DEFAULT_MCHBAR + 0x4224 + 0x400 * channel,
				0x1f105);
			write32(DEFAULT_MCHBAR + 0x4234 + 0x400 * channel,
				0x4041003);
			write32(DEFAULT_MCHBAR + 0x4204 + 0x400 * channel,
				(slotrank << 24) | 0);
			write32(DEFAULT_MCHBAR + 0x4214 + 0x400 * channel, 0);

			/* DRAM command RD */
			write32(DEFAULT_MCHBAR + 0x4228 + 0x400 * channel,
				0x1f105);
			write32(DEFAULT_MCHBAR + 0x4238 + 0x400 * channel,
				0x1001 | ((ctrl->CAS + 8) << 16));
			write32(DEFAULT_MCHBAR + 0x4208 + 0x400 * channel,
				(slotrank << 24) | 0x60000);
			write32(DEFAULT_MCHBAR + 0x4218 + 0x400 * channel, 0);

			/* DRAM command MRS
			 * MR3 disable MPR */
			write32(DEFAULT_MCHBAR + 0x422c + 0x400 * channel,
				0x1f000);
			write32(DEFAULT_MCHBAR + 0x423c + 0x400 * channel,
				0xc01 | (ctrl->tMOD << 16));
			write32(DEFAULT_MCHBAR + 0x420c + 0x400 * channel,
				(slotrank << 24) | 0x360000);
			write32(DEFAULT_MCHBAR + 0x421c + 0x400 * channel, 0);

			write32(DEFAULT_MCHBAR + 0x4284 + 0x400 * channel,
				0xc0001);
			wait_428c(channel);
		}

		/* XXX: check any measured value ? */

		FOR_ALL_LANES {
			write32(DEFAULT_MCHBAR + 0x4080 + 0x400 * channel +
				lane * 4,
				~read32(DEFAULT_MCHBAR + 0x4040 +
					0x400 * channel + lane * 4) & 0xff);
		}

		fill_pattern0(ctrl, channel, 0, 0xffffffff);
		write32(DEFAULT_MCHBAR + 0x4288 + (channel << 10), 0);
	}

	/* FIXME: under some conditions (older chipsets?) vendor BIOS sets both edges to the same value.  */
	write32(DEFAULT_MCHBAR + 0x4eb0, 0x300);
	printram("discover falling edges:\n[%x] = %x\n", 0x4eb0, 0x300);

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS {
		err = discover_edges_real(ctrl, channel, slotrank,
				    falling_edges[channel][slotrank]);
		if (err)
			return err;
	}

	write32(DEFAULT_MCHBAR + 0x4eb0, 0x200);
	printram("discover rising edges:\n[%x] = %x\n", 0x4eb0, 0x200);

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS {
		err = discover_edges_real(ctrl, channel, slotrank,
				    rising_edges[channel][slotrank]);
		if (err)
			return err;
	}

	write32(DEFAULT_MCHBAR + 0x4eb0, 0);

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
		ctrl->timings[channel][slotrank].lanes[lane].falling =
		    falling_edges[channel][slotrank][lane];
		ctrl->timings[channel][slotrank].lanes[lane].rising =
		    rising_edges[channel][slotrank][lane];
	}

	FOR_ALL_POPULATED_CHANNELS {
		program_timings(ctrl, channel);
	}

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
		write32(DEFAULT_MCHBAR + 0x4080 + 0x400 * channel + 4 * lane,
			0);
	}
	return 0;
}

static int discover_edges_write_real(ramctr_timing *ctrl, int channel,
				      int slotrank, int *edges)
{
	int edge;
	u32 raw_statistics[MAX_EDGE_TIMING + 1];
	int statistics[MAX_EDGE_TIMING + 1];
	const int reg3000b24[] = { 0, 0xc, 0x2c };
	int lane, i;
	int lower[NUM_LANES];
	int upper[NUM_LANES];
	int pat;

	FOR_ALL_LANES {
		lower[lane] = 0;
		upper[lane] = MAX_EDGE_TIMING;
	}

	for (i = 0; i < 3; i++) {
		write32(DEFAULT_MCHBAR + 0x3000 + 0x100 * channel,
			reg3000b24[i] << 24);
		printram("[%x] = 0x%08x\n",
		       0x3000 + 0x100 * channel, reg3000b24[i] << 24);
		for (pat = 0; pat < NUM_PATTERNS; pat++) {
			fill_pattern5(ctrl, channel, pat);
			write32(DEFAULT_MCHBAR + 0x4288 + 0x400 * channel, 0x1f);
			printram("using pattern %d\n", pat);
			for (edge = 0; edge <= MAX_EDGE_TIMING; edge++) {
				FOR_ALL_LANES {
					ctrl->timings[channel][slotrank].lanes[lane].
						rising = edge;
					ctrl->timings[channel][slotrank].lanes[lane].
						falling = edge;
				}
				program_timings(ctrl, channel);

				FOR_ALL_LANES {
					write32(DEFAULT_MCHBAR + 0x4340 +
						0x400 * channel + 4 * lane, 0);
					read32(DEFAULT_MCHBAR + 0x400 * channel +
					       4 * lane + 0x4140);
				}
				wait_428c(channel);

				/* DRAM command ACT */
				write32(DEFAULT_MCHBAR + 0x4220 + 0x400 * channel,
					0x1f006);
				write32(DEFAULT_MCHBAR + 0x4230 + 0x400 * channel,
					0x4 | (ctrl->tRCD << 16)
					| (max(ctrl->tRRD, (ctrl->tFAW >> 2) + 1) <<
					   10));
				write32(DEFAULT_MCHBAR + 0x4200 + 0x400 * channel,
					(slotrank << 24) | 0x60000);
				write32(DEFAULT_MCHBAR + 0x4210 + 0x400 * channel,
					0x240);

				/* DRAM command WR */
				write32(DEFAULT_MCHBAR + 0x4224 + 0x400 * channel,
					0x1f201);
				write32(DEFAULT_MCHBAR + 0x4234 + 0x400 * channel,
					0x8005020 | ((ctrl->tWTR + ctrl->CWL + 8) <<
						     16));
				write32(DEFAULT_MCHBAR + 0x4204 + 0x400 * channel,
					(slotrank << 24));
				write32(DEFAULT_MCHBAR + 0x4214 + 0x400 * channel,
					0x242);

				/* DRAM command RD */
				write32(DEFAULT_MCHBAR + 0x4228 + 0x400 * channel,
					0x1f105);
				write32(DEFAULT_MCHBAR + 0x4238 + 0x400 * channel,
					0x4005020 | (max(ctrl->tRTP, 8) << 16));
				write32(DEFAULT_MCHBAR + 0x4208 + 0x400 * channel,
					(slotrank << 24));
				write32(DEFAULT_MCHBAR + 0x4218 + 0x400 * channel,
					0x242);

				/* DRAM command PRE */
				write32(DEFAULT_MCHBAR + 0x422c + 0x400 * channel,
					0x1f002);
				write32(DEFAULT_MCHBAR + 0x423c + 0x400 * channel,
					0xc01 | (ctrl->tRP << 16));
				write32(DEFAULT_MCHBAR + 0x420c + 0x400 * channel,
					(slotrank << 24) | 0x60400);
				write32(DEFAULT_MCHBAR + 0x421c + 0x400 * channel, 0);

				write32(DEFAULT_MCHBAR + 0x4284 + 0x400 * channel,
					0xc0001);
				wait_428c(channel);
				FOR_ALL_LANES {
					read32(DEFAULT_MCHBAR + 0x4340 +
					       0x400 * channel + lane * 4);
				}

				raw_statistics[edge] =
					MCHBAR32(0x436c + 0x400 * channel);
			}
			FOR_ALL_LANES {
				struct run rn;
				for (edge = 0; edge <= MAX_EDGE_TIMING; edge++)
					statistics[edge] =
						! !(raw_statistics[edge] & (1 << lane));
				rn = get_longest_zero_run(statistics,
							  MAX_EDGE_TIMING + 1);
				printram("edges: %d, %d, %d: 0x%02x-0x%02x-0x%02x, 0x%02x-0x%02x\n",
					 channel, slotrank, i, rn.start, rn.middle,
					 rn.end, rn.start + ctrl->edge_offset[i],
					 rn.end - ctrl->edge_offset[i]);
				lower[lane] =
					max(rn.start + ctrl->edge_offset[i], lower[lane]);
				upper[lane] =
					min(rn.end - ctrl->edge_offset[i], upper[lane]);
				edges[lane] = (lower[lane] + upper[lane]) / 2;
				if (rn.all || (lower[lane] > upper[lane])) {
					printk(BIOS_EMERG, "edge write discovery failed: %d, %d, %d\n",
					       channel, slotrank, lane);
					return MAKE_ERR;
				}
			}
		}
	}

	write32(DEFAULT_MCHBAR + 0x3000, 0);
	printram("CPA\n");
	return 0;
}

static int discover_edges_write(ramctr_timing *ctrl)
{
	int falling_edges[NUM_CHANNELS][NUM_SLOTRANKS][NUM_LANES];
	int rising_edges[NUM_CHANNELS][NUM_SLOTRANKS][NUM_LANES];
	int channel, slotrank, lane;
	int err;

	/* FIXME: under some conditions (older chipsets?) vendor BIOS sets both edges to the same value.  */
	write32(DEFAULT_MCHBAR + 0x4eb0, 0x300);
	printram("discover falling edges write:\n[%x] = %x\n", 0x4eb0, 0x300);

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS {
		err = discover_edges_write_real(ctrl, channel, slotrank,
					  falling_edges[channel][slotrank]);
		if (err)
			return err;
	}

	write32(DEFAULT_MCHBAR + 0x4eb0, 0x200);
	printram("discover rising edges write:\n[%x] = %x\n", 0x4eb0, 0x200);

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS {
		err = discover_edges_write_real(ctrl, channel, slotrank,
					  rising_edges[channel][slotrank]);
		if (err)
			return err;
	}

	write32(DEFAULT_MCHBAR + 0x4eb0, 0);

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
		ctrl->timings[channel][slotrank].lanes[lane].falling =
		    falling_edges[channel][slotrank][lane];
		ctrl->timings[channel][slotrank].lanes[lane].rising =
		    rising_edges[channel][slotrank][lane];
	}

	FOR_ALL_POPULATED_CHANNELS
		program_timings(ctrl, channel);

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
		write32(DEFAULT_MCHBAR + 0x4080 + 0x400 * channel + 4 * lane,
			0);
	}
	return 0;
}

static void test_timC_write(ramctr_timing *ctrl, int channel, int slotrank)
{
	wait_428c(channel);
	/* DRAM command ACT */
	write32(DEFAULT_MCHBAR + 0x4220 + 0x400 * channel, 0x1f006);
	write32(DEFAULT_MCHBAR + 0x4230 + 0x400 * channel,
		(max((ctrl->tFAW >> 2) + 1, ctrl->tRRD)
		 << 10) | (ctrl->tRCD << 16) | 4);
	write32(DEFAULT_MCHBAR + 0x4200 + 0x400 * channel,
		(slotrank << 24) | 0x60000);
	write32(DEFAULT_MCHBAR + 0x4210 + 0x400 * channel, 0x244);

	/* DRAM command WR */
	write32(DEFAULT_MCHBAR + 0x4224 + 0x400 * channel, 0x1f201);
	write32(DEFAULT_MCHBAR + 0x4234 + 0x400 * channel,
		0x80011e0 |
		((ctrl->tWTR + ctrl->CWL + 8) << 16));
	write32(DEFAULT_MCHBAR + 0x4204 +
		0x400 * channel, (slotrank << 24));
	write32(DEFAULT_MCHBAR + 0x4214 +
		0x400 * channel, 0x242);

	/* DRAM command RD */
	write32(DEFAULT_MCHBAR + 0x4228 +
		0x400 * channel, 0x1f105);
	write32(DEFAULT_MCHBAR + 0x4238 +
		0x400 * channel,
		0x40011e0 | (max(ctrl->tRTP, 8) << 16));
	write32(DEFAULT_MCHBAR + 0x4208 +
		0x400 * channel, (slotrank << 24));
	write32(DEFAULT_MCHBAR + 0x4218 +
		0x400 * channel, 0x242);

	/* DRAM command PRE */
	write32(DEFAULT_MCHBAR + 0x422c +
		0x400 * channel, 0x1f002);
	write32(DEFAULT_MCHBAR + 0x423c +
		0x400 * channel,
		0x1001 | (ctrl->tRP << 16));
	write32(DEFAULT_MCHBAR + 0x420c +
		0x400 * channel,
		(slotrank << 24) | 0x60400);
	write32(DEFAULT_MCHBAR + 0x421c +
		0x400 * channel, 0);

	write32(DEFAULT_MCHBAR + 0x4284 +
		0x400 * channel, 0xc0001);
	wait_428c(channel);
}

static int discover_timC_write(ramctr_timing *ctrl)
{
	const u8 rege3c_b24[3] = { 0, 0xf, 0x2f };
	int i, pat;

	int lower[NUM_CHANNELS][NUM_SLOTRANKS][NUM_LANES];
	int upper[NUM_CHANNELS][NUM_SLOTRANKS][NUM_LANES];
	int channel, slotrank, lane;

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
		lower[channel][slotrank][lane] = 0;
		upper[channel][slotrank][lane] = MAX_TIMC;
	}

	write32(DEFAULT_MCHBAR + 0x4ea8, 1);
	printram("discover timC write:\n");

	for (i = 0; i < 3; i++)
		FOR_ALL_POPULATED_CHANNELS {
			write32(DEFAULT_MCHBAR + 0xe3c + (channel * 0x100),
				(rege3c_b24[i] << 24)
				| (read32(DEFAULT_MCHBAR + 0xe3c + (channel * 0x100))
				   & ~0x3f000000));
			udelay(2);
			for (pat = 0; pat < NUM_PATTERNS; pat++) {
				FOR_ALL_POPULATED_RANKS {
					int timC;
					u32 raw_statistics[MAX_TIMC + 1];
					int statistics[MAX_TIMC + 1];

					/* Make sure rn.start < rn.end */
					statistics[MAX_TIMC] = 1;

					fill_pattern5(ctrl, channel, pat);
					write32(DEFAULT_MCHBAR + 0x4288 + 0x400 * channel, 0x1f);
					for (timC = 0; timC < MAX_TIMC; timC++) {
						FOR_ALL_LANES
							ctrl->timings[channel][slotrank].lanes[lane].timC = timC;
						program_timings(ctrl, channel);

						test_timC_write (ctrl, channel, slotrank);

						raw_statistics[timC] =
							MCHBAR32(0x436c + 0x400 * channel);
					}
					FOR_ALL_LANES {
						struct run rn;
						for (timC = 0; timC < MAX_TIMC; timC++)
							statistics[timC] =
								!!(raw_statistics[timC] &
								   (1 << lane));

						rn = get_longest_zero_run(statistics,
									  MAX_TIMC + 1);
						if (rn.all) {
							printk(BIOS_EMERG, "timC write discovery failed: %d, %d, %d\n",
							       channel, slotrank, lane);
							return MAKE_ERR;
						}
						printram("timC: %d, %d, %d: 0x%02x-0x%02x-0x%02x, 0x%02x-0x%02x\n",
							 channel, slotrank, i, rn.start,
							 rn.middle, rn.end,
							 rn.start + ctrl->timC_offset[i],
							 rn.end - ctrl->timC_offset[i]);
						lower[channel][slotrank][lane] =
							max(rn.start + ctrl->timC_offset[i],
							    lower[channel][slotrank][lane]);
						upper[channel][slotrank][lane] =
							min(rn.end - ctrl->timC_offset[i],
							    upper[channel][slotrank][lane]);

					}
				}
			}
		}

	FOR_ALL_CHANNELS {
		write32(DEFAULT_MCHBAR + (channel * 0x100) + 0xe3c,
			0 | (read32(DEFAULT_MCHBAR + (channel * 0x100) + 0xe3c) &
			     ~0x3f000000));
		udelay(2);
	}

	write32(DEFAULT_MCHBAR + 0x4ea8, 0);

	printram("CPB\n");

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
		printram("timC %d, %d, %d: %x\n", channel,
		       slotrank, lane,
		       (lower[channel][slotrank][lane] +
			upper[channel][slotrank][lane]) / 2);
		ctrl->timings[channel][slotrank].lanes[lane].timC =
		    (lower[channel][slotrank][lane] +
		     upper[channel][slotrank][lane]) / 2;
	}
	FOR_ALL_POPULATED_CHANNELS {
		program_timings(ctrl, channel);
	}
	return 0;
}

static void normalize_training(ramctr_timing * ctrl)
{
	int channel, slotrank, lane;
	int mat = 0;

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS {
		int delta;
		FOR_ALL_LANES mat =
		    max(ctrl->timings[channel][slotrank].lanes[lane].timA, mat);
		 delta = (mat >> 6) - ctrl->timings[channel][slotrank].val_4028;
		 ctrl->timings[channel][slotrank].val_4024 += delta;
		 ctrl->timings[channel][slotrank].val_4028 += delta;
	}

	FOR_ALL_POPULATED_CHANNELS {
		program_timings(ctrl, channel);
	}
}

static void write_controller_mr(ramctr_timing * ctrl)
{
	int channel, slotrank;

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS {
		write32(DEFAULT_MCHBAR + 0x0004 + (channel << 8) +
			lane_registers[slotrank], make_mr0(ctrl, slotrank));
		write32(DEFAULT_MCHBAR + 0x0008 + (channel << 8) +
			lane_registers[slotrank],
			make_mr1(ctrl, slotrank, channel));
	}
}

static int channel_test(ramctr_timing *ctrl)
{
	int channel, slotrank, lane;

	slotrank = 0;
	FOR_ALL_POPULATED_CHANNELS
		if (read32(DEFAULT_MCHBAR + 0x42a0 + (channel << 10)) & 0xa000) {
			printk(BIOS_EMERG, "Mini channel test failed (1): %d\n",
			       channel);
			return MAKE_ERR;
		}
	FOR_ALL_POPULATED_CHANNELS {
		fill_pattern0(ctrl, channel, 0x12345678, 0x98765432);

		write32(DEFAULT_MCHBAR + 0x4288 + (channel << 10), 0);
	}

	for (slotrank = 0; slotrank < 4; slotrank++)
		FOR_ALL_CHANNELS
			if (ctrl->rankmap[channel] & (1 << slotrank)) {
		FOR_ALL_LANES {
			write32(DEFAULT_MCHBAR + (0x4f40 + 4 * lane), 0);
			write32(DEFAULT_MCHBAR + (0x4d40 + 4 * lane), 0);
		}
		wait_428c(channel);
		/* DRAM command ACT */
		write32(DEFAULT_MCHBAR + 0x4220 + (channel << 10), 0x0001f006);
		write32(DEFAULT_MCHBAR + 0x4230 + (channel << 10), 0x0028a004);
		write32(DEFAULT_MCHBAR + 0x4200 + (channel << 10),
			0x00060000 | (slotrank << 24));
		write32(DEFAULT_MCHBAR + 0x4210 + (channel << 10), 0x00000244);
		/* DRAM command WR */
		write32(DEFAULT_MCHBAR + 0x4224 + (channel << 10), 0x0001f201);
		write32(DEFAULT_MCHBAR + 0x4234 + (channel << 10), 0x08281064);
		write32(DEFAULT_MCHBAR + 0x4204 + (channel << 10),
			0x00000000 | (slotrank << 24));
		write32(DEFAULT_MCHBAR + 0x4214 + (channel << 10), 0x00000242);
		/* DRAM command RD */
		write32(DEFAULT_MCHBAR + 0x4228 + (channel << 10), 0x0001f105);
		write32(DEFAULT_MCHBAR + 0x4238 + (channel << 10), 0x04281064);
		write32(DEFAULT_MCHBAR + 0x4208 + (channel << 10),
			0x00000000 | (slotrank << 24));
		write32(DEFAULT_MCHBAR + 0x4218 + (channel << 10), 0x00000242);
		/* DRAM command PRE */
		write32(DEFAULT_MCHBAR + 0x422c + (channel << 10), 0x0001f002);
		write32(DEFAULT_MCHBAR + 0x423c + (channel << 10), 0x00280c01);
		write32(DEFAULT_MCHBAR + 0x420c + (channel << 10),
			0x00060400 | (slotrank << 24));
		write32(DEFAULT_MCHBAR + 0x421c + (channel << 10), 0x00000240);
		write32(DEFAULT_MCHBAR + 0x4284 + (channel << 10), 0x000c0001);
		wait_428c(channel);
		FOR_ALL_LANES
			if (read32(DEFAULT_MCHBAR + 0x4340 + (channel << 10) + 4 * lane)) {
				printk(BIOS_EMERG, "Mini channel test failed (2): %d, %d, %d\n",
				       channel, slotrank, lane);
				return MAKE_ERR;
			}
	}
	return 0;
}

static void set_scrambling_seed(ramctr_timing * ctrl)
{
	int channel;

	/* FIXME: we hardcode seeds. Do we need to use some PRNG for them?
	   I don't think so.  */
	static u32 seeds[NUM_CHANNELS][3] = {
		{0x00009a36, 0xbafcfdcf, 0x46d1ab68},
		{0x00028bfa, 0x53fe4b49, 0x19ed5483}
	};
	FOR_ALL_POPULATED_CHANNELS {
		MCHBAR32(0x4020 + 0x400 * channel) &= ~0x10000000;
		write32(DEFAULT_MCHBAR + 0x4034, seeds[channel][0]);
		write32(DEFAULT_MCHBAR + 0x403c, seeds[channel][1]);
		write32(DEFAULT_MCHBAR + 0x4038, seeds[channel][2]);
	}
}

static void set_4f8c(void)
{
	struct cpuid_result cpures;
	u32 cpu;

	cpures = cpuid(1);
	cpu = (cpures.eax);
	if (IS_SANDY_CPU(cpu) && (IS_SANDY_CPU_D0(cpu) || IS_SANDY_CPU_D1(cpu))) {
		MCHBAR32(0x4f8c) = 0x141D1519;
	} else {
		MCHBAR32(0x4f8c) = 0x551D1519;
	}
}

static void prepare_training(ramctr_timing * ctrl)
{
	int channel;

	FOR_ALL_POPULATED_CHANNELS {
		// Always drive command bus
		MCHBAR32(0x4004 + 0x400 * channel) |= 0x20000000;
	}

	udelay(1);

	FOR_ALL_POPULATED_CHANNELS {
		wait_428c(channel);
	}
}

static void set_4008c(ramctr_timing * ctrl)
{
	int channel, slotrank;
	u32 reg;
	FOR_ALL_POPULATED_CHANNELS {
		u32 b20, b4_8_12;
		int min_320c = 10000;
		int max_320c = -10000;

		FOR_ALL_POPULATED_RANKS {
			max_320c = max(ctrl->timings[channel][slotrank].val_320c, max_320c);
			min_320c = min(ctrl->timings[channel][slotrank].val_320c, min_320c);
		}

		if (max_320c - min_320c > 51)
			b20 = 0;
		else
			b20 = ctrl->ref_card_offset[channel];

		if (ctrl->reg_320c_range_threshold < max_320c - min_320c)
			b4_8_12 = 0x3330;
		else
			b4_8_12 = 0x2220;

		reg = read32(DEFAULT_MCHBAR + 0x400c + (channel << 10));
		write32(DEFAULT_MCHBAR + 0x400c + (channel << 10),
			(reg & 0xFFF0FFFF)
			| (ctrl->ref_card_offset[channel] << 16)
			| (ctrl->ref_card_offset[channel] << 18));
		write32(DEFAULT_MCHBAR + 0x4008 + (channel << 10),
			0x0a000000
			| (b20 << 20)
			| ((ctrl->ref_card_offset[channel] + 2) << 16)
			| b4_8_12);
	}
}

static void set_42a0(ramctr_timing * ctrl)
{
	int channel;
	FOR_ALL_POPULATED_CHANNELS {
		write32(DEFAULT_MCHBAR + (0x42a0 + 0x400 * channel),
			0x00001000 | ctrl->rankmap[channel]);
		MCHBAR32(0x4004 + 0x400 * channel) &= ~0x20000000;	// OK
	}
}

static int encode_5d10(int ns)
{
  return (ns + 499) / 500;
}

/* FIXME: values in this function should be hardware revision-dependent.  */
static void final_registers(ramctr_timing * ctrl)
{
	int channel;
	int t1_cycles = 0, t1_ns = 0, t2_ns;
	int t3_ns;
	u32 r32;

	write32(DEFAULT_MCHBAR + 0x4cd4, 0x00000046);

	write32(DEFAULT_MCHBAR + 0x400c, (read32(DEFAULT_MCHBAR + 0x400c) & 0xFFFFCFFF) | 0x1000);	// OK
	write32(DEFAULT_MCHBAR + 0x440c, (read32(DEFAULT_MCHBAR + 0x440c) & 0xFFFFCFFF) | 0x1000);	// OK
	write32(DEFAULT_MCHBAR + 0x4cb0, 0x00000740);
	write32(DEFAULT_MCHBAR + 0x4380, 0x00000aaa);	// OK
	write32(DEFAULT_MCHBAR + 0x4780, 0x00000aaa);	// OK
	write32(DEFAULT_MCHBAR + 0x4f88, 0x5f7003ff);	// OK
	write32(DEFAULT_MCHBAR + 0x5064, 0x00073000 | ctrl->reg_5064b0); // OK

	FOR_ALL_CHANNELS {
		switch (ctrl->rankmap[channel]) {
			/* Unpopulated channel.  */
		case 0:
			write32(DEFAULT_MCHBAR + 0x4384 + channel * 0x400, 0);
			break;
			/* Only single-ranked dimms.  */
		case 1:
		case 4:
		case 5:
			write32(DEFAULT_MCHBAR + 0x4384 + channel * 0x400, 0x373131);
			break;
			/* Dual-ranked dimms present.  */
		default:
			write32(DEFAULT_MCHBAR + 0x4384 + channel * 0x400, 0x9b6ea1);
			break;
		}
	}

	write32 (DEFAULT_MCHBAR + 0x5880, 0xca9171e5);
	write32 (DEFAULT_MCHBAR + 0x5888,
		 (read32 (DEFAULT_MCHBAR + 0x5888) & ~0xffffff) | 0xe4d5d0);
	write32 (DEFAULT_MCHBAR + 0x58a8, read32 (DEFAULT_MCHBAR + 0x58a8) & ~0x1f);
	write32 (DEFAULT_MCHBAR + 0x4294,
		 (read32 (DEFAULT_MCHBAR + 0x4294) & ~0x30000)
		 | (1 << 16));
	write32 (DEFAULT_MCHBAR + 0x4694,
		 (read32 (DEFAULT_MCHBAR + 0x4694) & ~0x30000)
		 | (1 << 16));

	MCHBAR32(0x5030) |= 1;	// OK
	MCHBAR32(0x5030) |= 0x80;	// OK
	MCHBAR32(0x5f18) = 0xfa;	// OK

	/* Find a populated channel.  */
	FOR_ALL_POPULATED_CHANNELS
		break;

	t1_cycles = ((read32(DEFAULT_MCHBAR + 0x4290 + channel * 0x400) >> 8) & 0xff);
	r32 = read32(DEFAULT_MCHBAR + 0x5064);
	if (r32 & 0x20000)
		t1_cycles += (r32 & 0xfff);
	t1_cycles += (read32(DEFAULT_MCHBAR + channel * 0x400 + 0x42a4) & 0xfff);
	t1_ns = t1_cycles * ctrl->tCK / 256 + 544;
	if (!(r32 & 0x20000))
		t1_ns += 500;

	t2_ns = 10 * ((read32(DEFAULT_MCHBAR + 0x5f10) >> 8) & 0xfff);
	if ( read32(DEFAULT_MCHBAR + 0x5f00) & 8 )
	{
		t3_ns = 10 * ((read32(DEFAULT_MCHBAR + 0x5f20) >> 8) & 0xfff);
		t3_ns += 10 * (read32(DEFAULT_MCHBAR + 0x5f18) & 0xff);
	}
	else
	{
		t3_ns = 500;
	}
	printk(BIOS_DEBUG, "t123: %d, %d, %d\n",
	       t1_ns, t2_ns, t3_ns);
	write32 (DEFAULT_MCHBAR + 0x5d10,
		 ((encode_5d10(t1_ns) + encode_5d10(t2_ns)) << 16)
		 | (encode_5d10(t1_ns) << 8)
		 | ((encode_5d10(t3_ns) + encode_5d10(t2_ns) + encode_5d10(t1_ns)) << 24)
		 | (read32(DEFAULT_MCHBAR + 0x5d10) & 0xC0C0C0C0)
		 | 0xc);
}

static void save_timings(ramctr_timing *ctrl)
{
	/* Save the MRC S3 restore data to cbmem */
	store_current_mrc_cache(ctrl, sizeof(*ctrl));
}

static void restore_timings(ramctr_timing * ctrl)
{
	int channel, slotrank, lane;

	FOR_ALL_POPULATED_CHANNELS
	    MCHBAR32(0x4004 + 0x400 * channel) =
		ctrl->tRRD
		| (ctrl->tRTP << 4)
		| (ctrl->tCKE << 8)
		| (ctrl->tWTR << 12)
		| (ctrl->tFAW << 16)
		| (ctrl->tWR << 24)
		| (ctrl->cmd_stretch[channel] << 30);

	udelay(1);

	FOR_ALL_POPULATED_CHANNELS {
		wait_428c(channel);
	}

	FOR_ALL_CHANNELS FOR_ALL_POPULATED_RANKS FOR_ALL_LANES {
		write32(DEFAULT_MCHBAR + 0x4080 + 0x400 * channel
			+ 4 * lane, 0);
	}

	FOR_ALL_POPULATED_CHANNELS
	    write32(DEFAULT_MCHBAR + 0x4008 + 0x400 * channel,
		    read32(DEFAULT_MCHBAR + 0x4008 +
			   0x400 * channel) | 0x8000000);

	FOR_ALL_POPULATED_CHANNELS {
		udelay (1);
		write32(DEFAULT_MCHBAR + 0x4020 + 0x400 * channel,
			read32(DEFAULT_MCHBAR + 0x4020 +
			       0x400 * channel) | 0x200000);
	}

	printram("CPE\n");

	write32(DEFAULT_MCHBAR + 0x3400, 0);
	write32(DEFAULT_MCHBAR + 0x4eb0, 0);

	printram("CP5b\n");

	FOR_ALL_POPULATED_CHANNELS {
		program_timings(ctrl, channel);
	}

	u32 reg, addr;

	while (!(MCHBAR32(0x5084) & 0x10000));
	do {
		reg = MCHBAR32(0x428c);
	} while ((reg & 0x14) == 0);

	// Set state of memory controller
	MCHBAR32(0x5030) = 0x116;
	MCHBAR32(0x4ea0) = 0;

	// Wait 500us
	udelay(500);

	FOR_ALL_CHANNELS {
		// Set valid rank CKE
		reg = 0;
		reg = (reg & ~0xf) | ctrl->rankmap[channel];
		addr = 0x400 * channel + 0x42a0;
		MCHBAR32(addr) = reg;

		// Wait 10ns for ranks to settle
		//udelay(0.01);

		reg = (reg & ~0xf0) | (ctrl->rankmap[channel] << 4);
		MCHBAR32(addr) = reg;

		// Write reset using a NOP
		write_reset(ctrl);
	}

	/* mrs commands. */
	dram_mrscommands(ctrl);

	printram("CP5c\n");

	write32(DEFAULT_MCHBAR + 0x3000, 0);

	FOR_ALL_CHANNELS {
		write32(DEFAULT_MCHBAR + (channel * 0x100) + 0xe3c,
			0 | (read32(DEFAULT_MCHBAR + (channel * 0x100) + 0xe3c) &
			     ~0x3f000000));
		udelay(2);
	}

	write32(DEFAULT_MCHBAR + 0x4ea8, 0);
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

void init_dram_ddr3(spd_raw_data *spds, int mobile, int min_tck,
	int s3resume)
{
	int me_uma_size;
	int cbmem_was_inited;
	ramctr_timing ctrl;
	int fast_boot;
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
	if ((pcie_read_config16(SOUTHBRIDGE, 0xa2) & 0xa0) == 0x20	/* 0x0004 */
	    && reg_5d10 && !s3resume) {
		write32(DEFAULT_MCHBAR + 0x5d10, 0);
		/* Need reset.  */
		outb(0x6, 0xcf9);

		halt();
	}

	memset(&ctrl, 0, sizeof(ctrl));

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
	if (ctrl_cached) {
		/* check SPD CRC16 to make sure the DIMMs haven't been replaced */
		fast_boot = verify_crc16_spds_ddr3(spds, ctrl_cached);
		if (!fast_boot)
			printk(BIOS_DEBUG, "Stored timings CRC16 mismatch.\n");
		if (!fast_boot && s3resume) {
			/* Failed S3 resume, reset to come up cleanly */
			outb(0x6, 0xcf9);
			halt();
		}
	} else
		fast_boot = 0;

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
		ctrl.mobile = mobile;
		ctrl.tCK = min_tck;

		/* Get DDR3 SPD data */
		dram_find_spds_ddr3(spds, &ctrl);

		err = try_init_dram_ddr3(&ctrl, fast_boot, s3resume, me_uma_size);
	}

	if (err && (ctrl.tCK < TCK_400MHZ)) {
		/* fallback: lower clock frequency */
		printk(BIOS_ERR, "RAM training failed, trying fallback.\n");
		printram("Decreasing clock frequency.\n");
		ctrl.tCK++;
		err = try_init_dram_ddr3(&ctrl, fast_boot, s3resume, me_uma_size);
	}

	if (err && (ctrl.tCK < TCK_400MHZ)) {
		/* fallback: lower clock frequency */
		printk(BIOS_ERR, "RAM training failed, trying fallback.\n");
		printram("Decreasing clock frequency.\n");
		ctrl.tCK++;
		err = try_init_dram_ddr3(&ctrl, fast_boot, s3resume, me_uma_size);
	}

	if (err) {
		/* fallback: disable failing channel */
		printk(BIOS_ERR, "RAM training failed, trying fallback.\n");
		printram("Disable failing channel.\n");

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

#define HOST_BRIDGE	PCI_DEVFN(0, 0)
#define DEFAULT_TCK	TCK_800MHZ

static unsigned int get_mem_min_tck(void)
{
	u32 reg32;
	u8 rev;
	const struct device *dev;
	const struct northbridge_intel_sandybridge_config *cfg = NULL;

	dev = dev_find_slot(0, HOST_BRIDGE);
	if (dev)
		cfg = dev->chip_info;

	/* If this is zero, it just means devicetree.cb didn't set it */
	if (!cfg || cfg->max_mem_clock_mhz == 0) {
		rev = pci_read_config8(PCI_DEV(0, 0, 0), PCI_DEVICE_ID);

		if ((rev & BASE_REV_MASK) == BASE_REV_SNB) {
			/* read Capabilities A Register DMFC bits */
			reg32 = pci_read_config32(PCI_DEV(0, 0, 0), CAPID0_A);
			reg32 &= 0x7;

			switch (reg32) {
			case 7: return TCK_533MHZ;
			case 6: return TCK_666MHZ;
			case 5: return TCK_800MHZ;
			/* reserved: */
			default:
				break;
			}
		} else {
			/* read Capabilities B Register DMFC bits */
			reg32 = pci_read_config32(PCI_DEV(0, 0, 0), CAPID0_B);
			reg32 = (reg32 >> 4) & 0x7;

			switch (reg32) {
			case 7: return TCK_533MHZ;
			case 6: return TCK_666MHZ;
			case 5: return TCK_800MHZ;
			case 4: return TCK_933MHZ;
			case 3: return TCK_1066MHZ;
			case 2: return TCK_1200MHZ;
			case 1: return TCK_1333MHZ;
			/* reserved: */
			default:
				break;
			}
		}
		return DEFAULT_TCK;
	} else {
		if (cfg->max_mem_clock_mhz >= 1066)
			return TCK_1066MHZ;
		else if (cfg->max_mem_clock_mhz >= 933)
			return TCK_933MHZ;
		else if (cfg->max_mem_clock_mhz >= 800)
			return TCK_800MHZ;
		else if (cfg->max_mem_clock_mhz >= 666)
			return TCK_666MHZ;
		else if (cfg->max_mem_clock_mhz >= 533)
			return TCK_533MHZ;
		else
			return TCK_400MHZ;
	}
}

#define DEFAULT_PCI_MMIO_SIZE 2048

static unsigned int get_mmio_size(void)
{
	const struct device *dev;
	const struct northbridge_intel_sandybridge_config *cfg = NULL;

	dev = dev_find_slot(0, HOST_BRIDGE);
	if (dev)
		cfg = dev->chip_info;

	/* If this is zero, it just means devicetree.cb didn't set it */
	if (!cfg || cfg->pci_mmio_size == 0)
		return DEFAULT_PCI_MMIO_SIZE;
	else
		return cfg->pci_mmio_size;
}

void perform_raminit(int s3resume)
{
	spd_raw_data spd[4];

	post_code(0x3a);

	memset (spd, 0, sizeof(spd));
	mainboard_get_spd(spd);

	timestamp_add_now(TS_BEFORE_INITRAM);

	init_dram_ddr3(spd, 1, get_mem_min_tck(), s3resume);
}
