/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef RAMINIT_H
#define RAMINIT_H

#include "ironlake.h"

#define NUM_CHANNELS	2
#define NUM_SLOTS	2
#define NUM_RANKS	2

/* [REG_178][CHANNEL][2 * SLOT + RANK][LANE] */
typedef struct {
	u8 smallest;
	u8 largest;
} timing_bounds_t[2][2][2][9];

#define MRC_CACHE_VERSION 3

struct ram_training {
	/* [TM][CHANNEL][SLOT][RANK][LANE] */
	u16 lane_timings[4][2][2][2][9];
	u16 reg_178;
	u16 reg_10b;

	u8 reg178_center;
	u8 reg178_smallest;
	u8 reg178_largest;
	timing_bounds_t timing_bounds[2];
	u16 timing_offset[2][2][2][9];
	u16 timing2_offset[2][2][2][9];
	u16 timing2_bounds[2][2][2][9][2];
	u8 reg274265[2][3];	/* [CHANNEL][REGISTER] */
	u8 reg2ca9_bit0;
	u32 reg_6dc;
	u32 reg_6e8;
};

struct raminfo {
	u16 clock_speed_index;	/* clock_speed (REAL, not DDR) / 133.(3) - 3 */
	u16 fsb_frequency;	/* in 1.(1)/2 MHz.  */
	u8 is_x16_module[2][2];	/* [CHANNEL][SLOT] */
	u8 density[2][2];	/* [CHANNEL][SLOT] */
	u8 populated_ranks[2][2][2];	/* [CHANNEL][SLOT][RANK] */
	int rank_start[2][2][2];
	u8 cas_latency;
	u8 board_lane_delay[9];
	u8 use_ecc;
	u8 revision;
	u8 max_supported_clock_speed_index;
	u8 uma_enabled;
	u8 spd[2][2][151];	/* [CHANNEL][SLOT][BYTE]  */
	u8 silicon_revision;
	u8 populated_ranks_mask[2];
	u8 max_slots_used_in_channel;
	u8 mode4030[2];
	u16 avg4044[2];
	u16 max4048[2];
	unsigned int total_memory_mb;
	unsigned int interleaved_part_mb;
	unsigned int non_interleaved_part_mb;

	unsigned int memory_reserved_for_heci_mb;

	struct ram_training training;
	u32 last_500_command[2];

	u32 delay46_ps[2];
	u32 delay54_ps[2];
	u8 revision_flag_1;
	u8 some_delay_1_cycle_floor;
	u8 some_delay_2_halfcycles_ceil;
	u8 some_delay_3_ps_rounded;

	const struct ram_training *cached_training;
};

static inline unsigned int fsbcycle_ps(struct raminfo *info)
{
	return 900000 / info->fsb_frequency;
}

/* The time of DDR transfer in ps.  */
static inline unsigned int halfcycle_ps(struct raminfo *info)
{
	return 3750 / (info->clock_speed_index + 3);
}

/* Frequency in 1.(1)=10/9 MHz units. */
static inline unsigned int frequency_11(struct raminfo *info)
{
	return (info->clock_speed_index + 3) * 120;
}

void chipset_init(const int s3resume);
/* spd_addrmap is array of 4 elements:
   Channel 0 Slot 0
   Channel 0 Slot 1
   Channel 1 Slot 0
   Channel 1 Slot 1
   0 means "not present"
*/
void raminit(const int s3resume, const u8 *spd_addrmap);

u16 get_max_timing(struct raminfo *info, int channel);
void early_quickpath_init(const u8 x2ca8);
void late_quickpath_init(struct raminfo *info, const int s3resume);

#endif				/* RAMINIT_H */
