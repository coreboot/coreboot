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

#ifndef RAMINIT_COMMON_H
#define RAMINIT_COMMON_H

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
	int sandybridge;

	/* DDR base_freq = 100 Mhz / 133 Mhz */
	u8 base_freq;

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
	u32 tCWL;
	u32 tCMD;
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
#define MAX_CAS 18
#define MIN_CAS 4

#define MAKE_ERR ((channel<<16)|(slotrank<<8)|1)
#define GET_ERR_CHANNEL(x) (x>>16)

#define MC_BIOS_REQ		0x5e00
#define MC_BIOS_DATA		0x5e04
#define PM_PDWN_Config		0x4cb0

u8 get_CWL(u32 tCK);
void dram_mrscommands(ramctr_timing * ctrl);
void program_timings(ramctr_timing * ctrl, int channel);
void dram_find_common_params(ramctr_timing *ctrl);
void dram_xover(ramctr_timing * ctrl);
void dram_timing_regs(ramctr_timing * ctrl);
void dram_dimm_mapping(ramctr_timing *ctrl);
void dram_dimm_set_mapping(ramctr_timing * ctrl);
void dram_zones(ramctr_timing * ctrl, int training);
unsigned int get_mem_min_tck(void);
void dram_memorymap(ramctr_timing * ctrl, int me_uma_size);
void dram_jedecreset(ramctr_timing * ctrl);
int read_training(ramctr_timing * ctrl);
int write_training(ramctr_timing * ctrl);
int command_training(ramctr_timing *ctrl);
int discover_edges(ramctr_timing *ctrl);
int discover_edges_write(ramctr_timing *ctrl);
int discover_timC_write(ramctr_timing *ctrl);
void normalize_training(ramctr_timing * ctrl);
void write_controller_mr(ramctr_timing * ctrl);
int channel_test(ramctr_timing *ctrl);
void set_scrambling_seed(ramctr_timing * ctrl);
void set_4f8c(void);
void prepare_training(ramctr_timing * ctrl);
void set_4008c(ramctr_timing * ctrl);
void set_42a0(ramctr_timing * ctrl);
void final_registers(ramctr_timing * ctrl);
void restore_timings(ramctr_timing * ctrl);

int try_init_dram_ddr3_sandy(ramctr_timing *ctrl, int fast_boot,
		int s3_resume, int me_uma_size);

int try_init_dram_ddr3_ivy(ramctr_timing *ctrl, int fast_boot,
		int s3_resume, int me_uma_size);

#endif
