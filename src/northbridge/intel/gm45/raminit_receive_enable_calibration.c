/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <device/mmio.h>
#include <console/console.h>
#include "gm45.h"

#define CxRECy_MCHBAR(x, y)	(0x14a0 + ((x) * 0x0100) + ((3 - (y)) * 4))
#define CxRECy_SHIFT_L		0
#define CxRECy_MASK_L		(3 << CxRECy_SHIFT_L)
#define CxRECy_SHIFT_H		16
#define CxRECy_MASK_H		(3 << CxRECy_SHIFT_H)
#define CxRECy_T_SHIFT		28
#define CxRECy_T_MASK		(0xf << CxRECy_T_SHIFT)
#define CxRECy_T(t)		(((t) << CxRECy_T_SHIFT) & CxRECy_T_MASK)
#define CxRECy_P_SHIFT		24
#define CxRECy_P_MASK		(0x7 << CxRECy_P_SHIFT)
#define CxRECy_P(p)		(((p) << CxRECy_P_SHIFT) & CxRECy_P_MASK)
#define CxRECy_PH_SHIFT		22
#define CxRECy_PH_MASK		(0x3 << CxRECy_PH_SHIFT)
#define CxRECy_PH(p)		(((p) << CxRECy_PH_SHIFT) & CxRECy_PH_MASK)
#define CxRECy_PM_SHIFT		20
#define CxRECy_PM_MASK		(0x3 << CxRECy_PM_SHIFT)
#define CxRECy_PM(p)		(((p) << CxRECy_PM_SHIFT) & CxRECy_PM_MASK)
#define CxRECy_TIMING_MASK	(CxRECy_T_MASK | CxRECy_P_MASK | \
				 CxRECy_PH_MASK | CxRECy_PM_MASK)

#define CxDRT3_C_SHIFT	7
#define CxDRT3_C_MASK	(0xf << CxDRT3_C_SHIFT)
#define CxDRT3_C(c)	(((c) << CxDRT3_C_SHIFT) & CxDRT3_C_MASK)
/* group to byte-lane mapping: (cardF X group X 2 per group) */
static const char bytelane_map[2][4][2] = {
/* A,B,C */{ { 0, 1 }, { 2, 3 }, { 4, 5 }, { 6, 7 } },
/*     F */{ { 0, 2 }, { 1, 3 }, { 4, 6 }, { 5, 7 } },
};

#define PH_BOUND	4
#define PH_STEP		2
#define PM_BOUND	3
#define C_BOUND	16
typedef struct {
	int c;
	int pre;
	int ph;
	int t;
	const int t_bound;
	int p;
	const int p_bound;
} rec_timing_t;
static void normalize_rec_timing(rec_timing_t *const timing)
{
	while (timing->p >= timing->p_bound) {
		timing->t++;
		timing->p -= timing->p_bound;
	}
	while (timing->p < 0) {
		timing->t--;
		timing->p += timing->p_bound;
	}
	while (timing->t >= timing->t_bound) {
		timing->ph += PH_STEP;
		timing->t -= timing->t_bound;
	}
	while (timing->t < 0) {
		timing->ph -= PH_STEP;
		timing->t += timing->t_bound;
	}
	while (timing->ph >= PH_BOUND) {
		timing->c++;
		timing->ph -= PH_BOUND;
	}
	while (timing->ph < 0) {
		timing->c--;
		timing->ph += PH_BOUND;
	}
	if (timing->c < 0 || timing->c >= C_BOUND)
		die("Timing under-/overflow during "
			"receive-enable calibration.\n");
}

static void rec_full_backstep(rec_timing_t *const timing)
{
	timing->c--;
}
static void rec_half_backstep(rec_timing_t *const timing)
{
	timing->ph -= PH_STEP;
}
static void rec_quarter_step(rec_timing_t *const timing)
{
	timing->t += (timing->t_bound) >> 1;
	timing->p += (timing->t_bound & 1) * (timing->p_bound >> 1);
}
static void rec_quarter_backstep(rec_timing_t *const timing)
{
	timing->t -= (timing->t_bound) >> 1;
	timing->p -= (timing->t_bound & 1) * (timing->p_bound >> 1);
}
static void rec_smallest_step(rec_timing_t *const timing)
{
	timing->p++;
}

static void program_timing(int channel, int group,
			   rec_timing_t timings[][4])
{
	rec_timing_t *const timing = &timings[channel][group];

	normalize_rec_timing(timing);

	/* C value is per channel. */
	unsigned int mchbar = CxDRT3_MCHBAR(channel);
	mchbar_clrsetbits32(mchbar, CxDRT3_C_MASK, CxDRT3_C(timing->c));

	/* All other per group. */
	mchbar = CxRECy_MCHBAR(channel, group);
	u32 reg = mchbar_read32(mchbar);
	reg &= ~CxRECy_TIMING_MASK;
	reg |= CxRECy_T(timing->t) | CxRECy_P(timing->p) |
		CxRECy_PH(timing->ph) | CxRECy_PM(timing->pre);
	mchbar_write32(mchbar, reg);
}

static int read_dqs_level(const int channel, const int lane)
{
	unsigned int mchbar = 0x14f0 + (channel * 0x0100);
	mchbar_clrbits32(mchbar, 1 << 9);
	mchbar_setbits32(mchbar, 1 << 9);

	/* Read from this channel. */
	read32p(raminit_get_rank_addr(channel, 0));

	mchbar = 0x14b0 + (channel * 0x0100) + ((7 - lane) * 4);
	return mchbar_read32(mchbar) & (1 << 30);
}

static void find_dqs_low(const int channel, const int group,
			 rec_timing_t timings[][4], const char lane_map[][2])
{
	/* Look for DQS low, using quarter steps. */
	while (read_dqs_level(channel, lane_map[group][0]) ||
			read_dqs_level(channel, lane_map[group][1])) {
		rec_quarter_step(&timings[channel][group]);
		program_timing(channel, group, timings);
	}
}
static void find_dqs_high(const int channel, const int group,
			  rec_timing_t timings[][4], const char lane_map[][2])
{
	/* Look for _any_ DQS high, using quarter steps. */
	while (!read_dqs_level(channel, lane_map[group][0]) &&
			!read_dqs_level(channel, lane_map[group][1])) {
		rec_quarter_step(&timings[channel][group]);
		program_timing(channel, group, timings);
	}
}
static void find_dqs_edge_lowhigh(const int channel, const int group,
				  rec_timing_t timings[][4],
				  const char lane_map[][2])
{
	/* Advance beyond previous high to low transition. */
	timings[channel][group].t += 2;
	program_timing(channel, group, timings);

	/* Coarsely look for DQS high. */
	find_dqs_high(channel, group, timings, lane_map);

	/* Go back and perform finer search. */
	rec_quarter_backstep(&timings[channel][group]);
	program_timing(channel, group, timings);
	while (!read_dqs_level(channel, lane_map[group][0]) ||
			!read_dqs_level(channel, lane_map[group][1])) {
		rec_smallest_step(&timings[channel][group]);
		program_timing(channel, group, timings);
	}
}
static void find_preamble(const int channel, const int group,
			  rec_timing_t timings[][4], const char lane_map[][2])
{
	/* Look for DQS low, backstepping. */
	while (read_dqs_level(channel, lane_map[group][0]) ||
			read_dqs_level(channel, lane_map[group][1])) {
		rec_full_backstep(&timings[channel][group]);
		program_timing(channel, group, timings);
	}
}

static void receive_enable_calibration(const int ddr_type,
				       const timings_t *const timings,
				       const dimminfo_t *const dimms)
{
	/* Override group to byte-lane mapping for raw card type F DIMMS. */
	static const char over_bytelane_map[2][4][2] = {
	/* A,B,C */{ { 0, 1 }, { 2, 3 }, { 4, 5 }, { 6, 7 } },
	/*     F */{ { 0, 0 }, { 3, 3 }, { 6, 6 }, { 5, 5 } },
	};

	const int cardF[] = {
		dimms[0].card_type == 0xf,
		dimms[1].card_type == 0xf,
	};

	const unsigned int t_bound =
		(timings->mem_clock == MEM_CLOCK_1067MT) ? 9
		: (ddr_type == DDR3) ? 12 : 15;
	const unsigned int p_bound =
		(timings->mem_clock == MEM_CLOCK_1067MT) ? 8 : 1;

	rec_timing_t rec_timings[2][4] = {
		{
			{ timings->CAS + 1, 0, 0, 0, t_bound, 0, p_bound },
			{ timings->CAS + 1, 0, 0, 0, t_bound, 0, p_bound },
			{ timings->CAS + 1, 0, 0, 0, t_bound, 0, p_bound },
			{ timings->CAS + 1, 0, 0, 0, t_bound, 0, p_bound }
		}, {
			{ timings->CAS + 1, 0, 0, 0, t_bound, 0, p_bound },
			{ timings->CAS + 1, 0, 0, 0, t_bound, 0, p_bound },
			{ timings->CAS + 1, 0, 0, 0, t_bound, 0, p_bound },
			{ timings->CAS + 1, 0, 0, 0, t_bound, 0, p_bound }
		}
	};

	int ch, group;
	FOR_EACH_POPULATED_CHANNEL(dimms, ch) {
		const char (*const map)[2] = over_bytelane_map[cardF[ch]];
		for (group = 0; group < 4; ++group) {
			program_timing(ch, group, rec_timings);
			find_dqs_low(ch, group, rec_timings, map);
			find_dqs_edge_lowhigh(ch, group, rec_timings, map);

			rec_quarter_step(&rec_timings[ch][group]);
			program_timing(ch, group, rec_timings);
			find_preamble(ch, group, rec_timings, map);
			find_dqs_edge_lowhigh(ch, group, rec_timings, map);
			rec_half_backstep(&rec_timings[ch][group]);
			normalize_rec_timing(&rec_timings[ch][group]);
			if (cardF[ch]) {
				rec_timings[ch][group].t++;
				program_timing(ch, group, rec_timings);
			}
		}
		int c_min = C_BOUND;
		for (group = 0; group < 4; ++group) {
			if (rec_timings[ch][group].c < c_min)
				c_min = rec_timings[ch][group].c;
		}
		for (group = 0; group < 4; ++group) {
			rec_timings[ch][group].pre =
				rec_timings[ch][group].c - c_min;
			rec_timings[ch][group].c = c_min;
			program_timing(ch, group, rec_timings);
			printk(RAM_DEBUG, "Final timings for ");
			printk(BIOS_DEBUG, "group %d, ch %d: %d.%d.%d.%d.%d\n",
			       group, ch,
			       rec_timings[ch][group].c,
			       rec_timings[ch][group].pre,
			       rec_timings[ch][group].ph,
			       rec_timings[ch][group].t,
			       rec_timings[ch][group].p);
		}
	}
}

void raminit_receive_enable_calibration(const int ddr_type,
					const timings_t *const timings,
					const dimminfo_t *const dimms)
{
	int ch;

	/* Setup group to byte-lane mapping. */
	FOR_EACH_POPULATED_CHANNEL(dimms, ch) {
		const char (*const map)[2] =
			bytelane_map[dimms[ch].card_type == 0xf];
		unsigned int group;
		for (group = 0; group < 4; ++group) {
			const unsigned int mchbar = CxRECy_MCHBAR(ch, group);
			u32 reg = mchbar_read32(mchbar);
			reg &= ~((3 << 16) | (1 << 8) | 3);
			reg |= (map[group][0] - group);
			reg |= (map[group][1] - group - 1) << 16;
			mchbar_write32(mchbar, reg);
		}
	}

	mchbar_setbits32(0x12a4, 1 << 31);
	mchbar_setbits32(0x13a4, 1 << 31);
	mchbar_clrsetbits32(0x14f0, 3 << 9, 1 << 9);
	mchbar_clrsetbits32(0x15f0, 3 << 9, 1 << 9);

	receive_enable_calibration(ddr_type, timings, dimms);

	mchbar_clrbits32(0x12a4, 1 << 31);
	mchbar_clrbits32(0x13a4, 1 << 31);
	raminit_reset_readwrite_pointers();
}
