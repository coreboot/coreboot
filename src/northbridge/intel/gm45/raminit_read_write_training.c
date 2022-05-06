/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <device/mmio.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include "gm45.h"

typedef struct {
	u32 addr[RANKS_PER_CHANNEL];
	unsigned int count;
} address_bunch_t;

/* Read Training. */
#define CxRDTy_MCHBAR(ch, bl)	(0x14b0 + ((ch) * 0x0100) + ((7 - (bl)) * 4))
#define CxRDTy_T_SHIFT		20
#define CxRDTy_T_MASK		(0xf << CxRDTy_T_SHIFT)
#define CxRDTy_T(t)		(((t) << CxRDTy_T_SHIFT) & CxRDTy_T_MASK)
#define CxRDTy_P_SHIFT		16
#define CxRDTy_P_MASK		(0x7 << CxRDTy_P_SHIFT)
#define CxRDTy_P(p)		(((p) << CxRDTy_P_SHIFT) & CxRDTy_P_MASK)
static const u32 read_training_schedule[] = {
	0xfefefefe, 0x7f7f7f7f, 0xbebebebe, 0xdfdfdfdf,
	0xeeeeeeee, 0xf7f7f7f7, 0xfafafafa, 0xfdfdfdfd,
	0x00000000, 0x81818181, 0x40404040, 0x21212121,
	0x10101010, 0x09090909, 0x04040404, 0x03030303,
	0x10101010, 0x11111111, 0xeeeeeeee, 0xefefefef,
	0x10101010, 0x11111111, 0xeeeeeeee, 0xefefefef,
	0x10101010, 0xefefefef, 0x10101010, 0xefefefef,
	0x10101010, 0xefefefef, 0x10101010, 0xefefefef,
	0x00000000, 0xffffffff, 0x00000000, 0xffffffff,
	0x00000000, 0xffffffff, 0x00000000, 0x00000000,
};
#define READ_TIMING_P_SHIFT	3
#define READ_TIMING_P_BOUND	(1 << READ_TIMING_P_SHIFT)
#define READ_TIMING_T_BOUND	14
typedef struct {
	int t;
	int p;
} read_timing_t;
static void print_read_timing(const int msg_lvl, const char *const msg,
			      const int lane, const int channel,
			      const read_timing_t *const timing)
{
	printk(msg_lvl, "%sbyte lane %d, ch %d: %d.%d\n",
	       msg, lane, channel, timing->t, timing->p);
}

static int normalize_read_timing(read_timing_t *const timing)
{
	while (timing->p >= READ_TIMING_P_BOUND) {
		timing->t++;
		timing->p -= READ_TIMING_P_BOUND;
	}
	while (timing->p < 0) {
		timing->t--;
		timing->p += READ_TIMING_P_BOUND;
	}
	if (timing->t < 0) {
		printk(BIOS_WARNING,
		       "Timing underflow during read training.\n");
		timing->t = 0;
		timing->p = 0;
		return -1;
	} else if (timing->t >= READ_TIMING_T_BOUND) {
		printk(BIOS_WARNING,
		       "Timing overflow during read training.\n");
		timing->t = READ_TIMING_T_BOUND - 1;
		timing->p = READ_TIMING_P_BOUND - 1;
		return -1;
	}
	return 0;
}
static int program_read_timing(const int ch, const int lane,
			       read_timing_t *const timing)
{
	if (normalize_read_timing(timing) < 0)
		return -1;

	u32 reg = mchbar_read32(CxRDTy_MCHBAR(ch, lane));
	reg &= ~(CxRDTy_T_MASK | CxRDTy_P_MASK);
	reg |= CxRDTy_T(timing->t) | CxRDTy_P(timing->p);
	mchbar_write32(CxRDTy_MCHBAR(ch, lane), reg);

	return 0;
}
/* Returns 1 on success, 0 on failure. */
static int read_training_test(const int channel, const int lane,
			      const address_bunch_t *const addresses)
{
	int i;

	const int lane_offset = lane & 4;
	const int lane_mask = 0xff << ((lane & ~4) << 3);

	for (i = 0; i < addresses->count; ++i) {
		unsigned int offset;
		for (offset = lane_offset; offset < 320; offset += 8) {
			const u32 read = read32p(addresses->addr[i] + offset);
			const u32 good = read_training_schedule[offset >> 3];
			if ((read & lane_mask) != (good & lane_mask))
				return 0;
		}
	}
	return 1;
}
static int read_training_find_lower(const int channel, const int lane,
				    const address_bunch_t *const addresses,
				    read_timing_t *const lower)
{
	/* Coarse search for good t. */
	program_read_timing(channel, lane, lower);
	while (!read_training_test(channel, lane, addresses)) {
		++lower->t;
		if (program_read_timing(channel, lane, lower) < 0)
			return -1;
	}

	/* Step back, then fine search for good p. */
	if (lower->t <= 0)
		/* Can't step back, zero is good. */
		return 0;

	--lower->t;
	program_read_timing(channel, lane, lower);
	while (!read_training_test(channel, lane, addresses)) {
		++lower->p;
		if (program_read_timing(channel, lane, lower) < 0)
			return -1;
	}

	return 0;
}
static int read_training_find_upper(const int channel, const int lane,
				    const address_bunch_t *const addresses,
				    read_timing_t *const upper)
{
	if (program_read_timing(channel, lane, upper) < 0)
		return -1;
	if (!read_training_test(channel, lane, addresses)) {
		printk(BIOS_WARNING,
		       "Read training failure: limits too narrow.\n");
		return -1;
	}
	/* Coarse search for bad t. */
	do {
		++upper->t;
		if (program_read_timing(channel, lane, upper) < 0)
			return -1;
	} while (read_training_test(channel, lane, addresses));
	/* Fine search for bad p. */
	--upper->t;
	program_read_timing(channel, lane, upper);
	while (read_training_test(channel, lane, addresses)) {
		++upper->p;
		if (program_read_timing(channel, lane, upper) < 0)
			return -1;
	}

	return 0;
}
static void read_training_per_lane(const int channel, const int lane,
				   const address_bunch_t *const addresses)
{
	read_timing_t lower, upper;

	mchbar_setbits32(CxRDTy_MCHBAR(channel, lane), 3 << 25);

	/*** Search lower bound. ***/

	/* Start at zero. */
	lower.t = 0;
	lower.p = 0;
	if (read_training_find_lower(channel, lane, addresses, &lower) < 0)
		die("Read training failure: lower bound.\n");
	print_read_timing(RAM_DEBUG, "Lower bound for ", lane, channel, &lower);

	/*** Search upper bound. ***/

	/* Start at lower + 1t. */
	upper.t = lower.t + 1;
	upper.p = lower.p;
	if (read_training_find_upper(channel, lane, addresses, &upper) < 0)
		/* Overflow on upper edge is not fatal. */
		printk(BIOS_WARNING, "Read training failure: upper bound.\n");
	print_read_timing(RAM_DEBUG, "Upper bound for ", lane, channel, &upper);

	/*** Calculate and program mean value. ***/

	lower.p += lower.t << READ_TIMING_P_SHIFT;
	upper.p += upper.t << READ_TIMING_P_SHIFT;
	const int mean_p = (lower.p + upper.p) >> 1;
	/* lower becomes the mean value. */
	lower.t = mean_p >> READ_TIMING_P_SHIFT;
	lower.p = mean_p & (READ_TIMING_P_BOUND - 1);
	program_read_timing(channel, lane, &lower);
	printk(RAM_DEBUG, "Final timings for ");
	print_read_timing(BIOS_DEBUG, "", lane, channel, &lower);
}
static void perform_read_training(const dimminfo_t *const dimms)
{
	int ch, i;

	FOR_EACH_POPULATED_CHANNEL(dimms, ch) {
		address_bunch_t addresses = { { 0, }, 0 };
		FOR_EACH_POPULATED_RANK_IN_CHANNEL(dimms, ch, i)
			addresses.addr[addresses.count++] =
				raminit_get_rank_addr(ch, i);

		for (i = 0; i < addresses.count; ++i) {
			/* Write test pattern. */
			unsigned int offset;
			for (offset = 0; offset < 320; offset += 4)
				write32p(addresses.addr[i] + offset,
					read_training_schedule[offset >> 3]);
		}

		for (i = 0; i < 8; ++i)
			read_training_per_lane(ch, i, &addresses);
	}
}
static void read_training_store_results(void)
{
	u8 bytes[TOTAL_CHANNELS * 8];
	int ch, i;

	/* Store one timing pair in one byte each. */
	FOR_EACH_CHANNEL(ch) {
		for (i = 0; i < 8; ++i) {
			const u32 bl_reg = mchbar_read32(CxRDTy_MCHBAR(ch, i));
			bytes[(ch * 8) + i] =
				(((bl_reg & CxRDTy_T_MASK) >> CxRDTy_T_SHIFT)
				 << 4) |
				((bl_reg & CxRDTy_P_MASK) >> CxRDTy_P_SHIFT);
		}
	}

	/* Store everything in CMOS above 128 bytes. */
	for (i = 0; i < (TOTAL_CHANNELS * 8); ++i)
		cmos_write(bytes[i], CMOS_READ_TRAINING + i);
}
static void read_training_restore_results(void)
{
	u8 bytes[TOTAL_CHANNELS * 8];
	int ch, i;

	/* Read from CMOS. */
	for (i = 0; i < (TOTAL_CHANNELS * 8); ++i)
		bytes[i] = cmos_read(CMOS_READ_TRAINING + i);

	/* Program restored results. */
	FOR_EACH_CHANNEL(ch) {
		for (i = 0; i < 8; ++i) {
			const int t = bytes[(ch * 8) + i] >> 4;
			const int p = bytes[(ch * 8) + i] & 7;
			u32 bl_reg = mchbar_read32(CxRDTy_MCHBAR(ch, i));
			bl_reg &= ~(CxRDTy_T_MASK | CxRDTy_P_MASK);
			bl_reg |= (3 << 25) | CxRDTy_T(t) | CxRDTy_P(p);
			mchbar_write32(CxRDTy_MCHBAR(ch, i), bl_reg);
			printk(BIOS_DEBUG, "Restored timings for byte lane "
			       "%d on channel %d: %d.%d\n", i, ch, t, p);
		}
	}
}
void raminit_read_training(const dimminfo_t *const dimms, const int s3resume)
{
	if (!s3resume) {
		perform_read_training(dimms);
		read_training_store_results();
	} else {
		read_training_restore_results();
	}
	raminit_reset_readwrite_pointers();
}

/* Write Training. */
#define CxWRTy_T_SHIFT		28
#define CxWRTy_T_MASK		(0xf << CxWRTy_T_SHIFT)
#define CxWRTy_T(t)		(((t) << CxWRTy_T_SHIFT) & CxWRTy_T_MASK)
#define CxWRTy_P_SHIFT		24
#define CxWRTy_P_MASK		(0x7 << CxWRTy_P_SHIFT)
#define CxWRTy_P(p)		(((p) << CxWRTy_P_SHIFT) & CxWRTy_P_MASK)
#define CxWRTy_F_SHIFT		18
#define CxWRTy_F_MASK		(0x3 << CxWRTy_F_SHIFT)
#define CxWRTy_F(f)		(((f) << CxWRTy_F_SHIFT) & CxWRTy_F_MASK)
#define CxWRTy_D_SHIFT		16
#define CxWRTy_D_MASK		(0x3 << CxWRTy_D_SHIFT)
#define CxWRTy_BELOW_D		(0x3 << CxWRTy_D_SHIFT)
#define CxWRTy_ABOVE_D		(0x1 << CxWRTy_D_SHIFT)
static const u32 write_training_schedule[] = {
	0xffffffff, 0x00000000, 0xffffffff, 0x00000000,
	0xffffffff, 0x00000000, 0xffffffff, 0x00000000,
	0xffffffff, 0x00000000, 0xffffffff, 0x00000000,
	0xffffffff, 0x00000000, 0xffffffff, 0x00000000,
	0xefefefef, 0x10101010, 0xefefefef, 0x10101010,
	0xefefefef, 0x10101010, 0xefefefef, 0x10101010,
	0xefefefef, 0x10101010, 0xefefefef, 0x10101010,
	0xefefefef, 0x10101010, 0xefefefef, 0x10101010,
	0xefefefef, 0xeeeeeeee, 0x11111111, 0x10101010,
	0xefefefef, 0xeeeeeeee, 0x11111111, 0x10101010,
	0xefefefef, 0xeeeeeeee, 0x11111111, 0x10101010,
	0xefefefef, 0xeeeeeeee, 0x11111111, 0x10101010,
	0x03030303, 0x04040404, 0x09090909, 0x10101010,
	0x21212121, 0x40404040, 0x81818181, 0x00000000,
	0x03030303, 0x04040404, 0x09090909, 0x10101010,
	0x21212121, 0x40404040, 0x81818181, 0x00000000,
	0xfdfdfdfd, 0xfafafafa, 0xf7f7f7f7, 0xeeeeeeee,
	0xdfdfdfdf, 0xbebebebe, 0x7f7f7f7f, 0xfefefefe,
	0xfdfdfdfd, 0xfafafafa, 0xf7f7f7f7, 0xeeeeeeee,
	0xdfdfdfdf, 0xbebebebe, 0x7f7f7f7f, 0xfefefefe,
};
/* for raw card types A, B and C: MEM_CLOCK_1067MT? X group X lower/upper */
static const u32 write_training_bytelane_masks_abc[2][4][2] = {
	{ /* clock < MEM_CLOCK_1067MT */
		{ 0xffffffff, 0x00000000 }, { 0x00000000, 0x00000000 },
		{ 0x00000000, 0xffffffff }, { 0x00000000, 0x00000000 },
	},
	{ /* clock == MEM_CLOCK_1067MT */
		{ 0x0000ffff, 0x00000000 }, { 0xffff0000, 0x00000000 },
		{ 0x00000000, 0x0000ffff }, { 0x00000000, 0xffff0000 },
	},
};
/* for raw card type F: group X lower/upper */
static const u32 write_training_bytelane_masks_f[4][2] = {
	{ 0xff00ff00, 0x00000000 }, { 0x00ff00ff, 0x00000000 },
	{ 0x00000000, 0xff00ff00 }, { 0x00000000, 0x00ff00ff },
};
#define WRITE_TIMING_P_SHIFT	3
#define WRITE_TIMING_P_BOUND	(1 << WRITE_TIMING_P_SHIFT)
#define WRITE_TIMING_F_BOUND	4
typedef struct {
	int f;
	int t;
	const int t_bound;
	int p;
} write_timing_t;
static void print_write_timing(const int msg_lvl, const char *const msg,
			       const int group, const int channel,
			       const write_timing_t *const timing)
{
	printk(msg_lvl, "%sgroup %d, ch %d: %d.%d.%d\n",
	       msg, group, channel, timing->f, timing->t, timing->p);
}

static int normalize_write_timing(write_timing_t *const timing)
{
	while (timing->p >= WRITE_TIMING_P_BOUND) {
		timing->t++;
		timing->p -= WRITE_TIMING_P_BOUND;
	}
	while (timing->p < 0) {
		timing->t--;
		timing->p += WRITE_TIMING_P_BOUND;
	}
	while (timing->t >= timing->t_bound) {
		timing->f++;
		timing->t -= timing->t_bound;
	}
	while (timing->t < 0) {
		timing->f--;
		timing->t += timing->t_bound;
	}
	if (timing->f < 0) {
		printk(BIOS_WARNING,
		       "Timing underflow during write training.\n");
		timing->f = 0;
		timing->t = 0;
		timing->p = 0;
		return -1;
	} else if (timing->f >= WRITE_TIMING_F_BOUND) {
		printk(BIOS_WARNING,
		       "Timing overflow during write training.\n");
		timing->f = WRITE_TIMING_F_BOUND - 1;
		timing->t = timing->t_bound - 1;
		timing->p = WRITE_TIMING_P_BOUND - 1;
		return -1;
	}
	return 0;
}
static int program_write_timing(const int ch, const int group,
				write_timing_t *const timing, int memclk1067)
{
	/* MEM_CLOCK_1067MT? X lower/upper */
	const u32 d_bounds[2][2] = { { 1, 6 }, { 2, 9 } };

	if (normalize_write_timing(timing) < 0)
		return -1;

	const int f = timing->f;
	const int t = timing->t;
	const int p = (memclk1067 && (((t ==  9) && (timing->p >= 4)) ||
				       ((t == 10) && (timing->p < 4))))
		? 4 : timing->p;
	const int d =
		(t <= d_bounds[memclk1067][0]) ? CxWRTy_BELOW_D :
		((t >  d_bounds[memclk1067][1]) ? CxWRTy_ABOVE_D : 0);

	u32 reg = mchbar_read32(CxWRTy_MCHBAR(ch, group));
	reg &= ~(CxWRTy_T_MASK | CxWRTy_P_MASK | CxWRTy_F_MASK);
	reg &= ~CxWRTy_D_MASK;
	reg |= CxWRTy_T(t) | CxWRTy_P(p) | CxWRTy_F(f) | d;
	mchbar_write32(CxWRTy_MCHBAR(ch, group), reg);

	return 0;
}
/* Returns 1 on success, 0 on failure. */
static int write_training_test(const address_bunch_t *const addresses,
			       const u32 *const masks)
{
	int i, ret = 0;

	const u32 mmarb0 = mchbar_read32(0x0220);
	const u8  wrcctl = mchbar_read8(0x0218);
	mchbar_setbits32(0x0220, 0xf << 28);
	mchbar_setbits8(0x0218,  0x1 <<  4);

	for (i = 0; i < addresses->count; ++i) {
		const unsigned int addr = addresses->addr[i];
		unsigned int off;
		for (off = 0; off < 640; off += 8) {
			const u32 pattern = write_training_schedule[off >> 3];
			write32p(addr + off, pattern);
			write32p(addr + off + 4, pattern);
		}

		mchbar_setbits8(0x78, 1);

		for (off = 0; off < 640; off += 8) {
			const u32 good = write_training_schedule[off >> 3];
			const u32 read1 = read32p(addr + off);
			if ((read1 & masks[0]) != (good & masks[0]))
				goto _bad_timing_out;
			const u32 read2 = read32p(addr + off + 4);
			if ((read2 & masks[1]) != (good & masks[1]))
				goto _bad_timing_out;
		}
	}
	ret = 1;

_bad_timing_out:
	mchbar_write32(0x0220, mmarb0);
	mchbar_write8(0x0218, wrcctl);

	return ret;
}
static int write_training_find_lower(const int ch, const int group,
				     const address_bunch_t *const addresses,
				     const u32 masks[][2], const int memclk1067,
				     write_timing_t *const lower)
{
	program_write_timing(ch, group, lower, memclk1067);
	/* Coarse search for good t. */
	while (!write_training_test(addresses, masks[group])) {
		++lower->t;
		if (program_write_timing(ch, group, lower, memclk1067) < 0)
			return -1;
	}
	/* Step back, then fine search for good p. */
	if ((lower->f <= 0) && (lower->t <= 0))
		/* Can't step back, zero is good. */
		return 0;

	--lower->t;
	program_write_timing(ch, group, lower, memclk1067);
	while (!write_training_test(addresses, masks[group])) {
		++lower->p;
		if (program_write_timing(ch, group, lower, memclk1067) < 0)
			return -1;
	}

	return 0;
}
static int write_training_find_upper(const int ch, const int group,
				     const address_bunch_t *const addresses,
				     const u32 masks[][2], const int memclk1067,
				     write_timing_t *const upper)
{
	if (program_write_timing(ch, group, upper, memclk1067) < 0)
		return -1;
	if (!write_training_test(addresses, masks[group])) {
		printk(BIOS_WARNING,
		       "Write training failure; limits too narrow.\n");
		return -1;
	}
	/* Coarse search for bad t. */
	while (write_training_test(addresses, masks[group])) {
		++upper->t;
		if (program_write_timing(ch, group, upper, memclk1067) < 0)
			return -1;
	}
	/* Fine search for bad p. */
	--upper->t;
	program_write_timing(ch, group, upper, memclk1067);
	while (write_training_test(addresses, masks[group])) {
		++upper->p;
		if (program_write_timing(ch, group, upper, memclk1067) < 0)
			return -1;
	}

	return 0;
}
static void write_training_per_group(const int ch, const int group,
				     const address_bunch_t *const addresses,
				     const u32 masks[][2], const int memclk1067)
{
	const int t_bound = memclk1067 ? 12 : 11;
	write_timing_t lower = { 0, 0, t_bound, 0 },
		       upper = { 0, 0, t_bound, 0 };

	/*** Search lower bound. ***/

	/* Start at -1f from current values. */
	const u32 reg = mchbar_read32(CxWRTy_MCHBAR(ch, group));
	lower.t =  (reg >> 12) & 0xf;
	lower.p =  (reg >>  8) & 0x7;
	lower.f = ((reg >>  2) & 0x3) - 1;

	if (write_training_find_lower(ch, group, addresses,
				      masks, memclk1067, &lower) < 0)
		die("Write training failure: lower bound.\n");
	print_write_timing(RAM_DEBUG, "Lower bound for ", group, ch, &lower);

	/*** Search upper bound. ***/

	/* Start at lower + 3t. */
	upper.t = lower.t + 3;
	upper.p = lower.p;
	upper.f = lower.f;

	if (write_training_find_upper(ch, group, addresses,
				      masks, memclk1067, &upper) < 0)
		printk(BIOS_WARNING, "Write training failure: upper bound.\n");
	print_write_timing(RAM_DEBUG, "Upper bound for ", group, ch, &upper);

	/*** Calculate and program mean value. ***/

	lower.t += lower.f * lower.t_bound;
	lower.p += lower.t << WRITE_TIMING_P_SHIFT;
	upper.t += upper.f * upper.t_bound;
	upper.p += upper.t << WRITE_TIMING_P_SHIFT;
	/* lower becomes the mean value. */
	const int mean_p = (lower.p + upper.p) >> 1;
	lower.f = mean_p / (lower.t_bound << WRITE_TIMING_P_SHIFT);
	lower.t = (mean_p >> WRITE_TIMING_P_SHIFT) % lower.t_bound;
	lower.p = mean_p & (WRITE_TIMING_P_BOUND - 1);
	program_write_timing(ch, group, &lower, memclk1067);
	printk(RAM_DEBUG, "Final timings for ");
	print_write_timing(BIOS_DEBUG, "", group, ch, &lower);
}
static void perform_write_training(const int memclk1067,
				   const dimminfo_t *const dimms)
{
	const int cardF[] = { dimms[0].card_type == 0xf,
			      dimms[1].card_type == 0xf };
	int ch, r, group;

	address_bunch_t addr[2] = { { { 0, }, 0 }, { { 0, }, 0 }, };
	/* Add check if channel A is populated, i.e. if cardF[0] is valid.
	 * Otherwise we would write channel A registers when DIMM in channel B
	 * is of raw card type A, B or C (cardF[1] == 0) even if channel A is
	 * not populated.
	 * Needs raw card type A, B or C for testing. */
	if ((dimms[0].card_type != 0) && (cardF[0] == cardF[1])) {
		/* Common path for both channels. */
		FOR_EACH_POPULATED_RANK(dimms, ch, r)
			addr[0].addr[addr[0].count++] =
				raminit_get_rank_addr(ch, r);
	} else {
		FOR_EACH_POPULATED_RANK(dimms, ch, r)
			addr[ch].addr[addr[ch].count++] =
				raminit_get_rank_addr(ch, r);
	}

	FOR_EACH_CHANNEL(ch) if (addr[ch].count > 0) {
		const u32 (*const masks)[2] = (!cardF[ch])
			? write_training_bytelane_masks_abc[memclk1067]
			: write_training_bytelane_masks_f;
		for (group = 0; group < 4; ++group) {
			if (!masks[group][0] && !masks[group][1])
				continue;
			write_training_per_group(
				ch, group, &addr[ch], masks, memclk1067);
		}
	}
}
static void write_training_store_results(void)
{
	u8 bytes[TOTAL_CHANNELS * 4 * 2]; /* two bytes per group */
	int ch, i;

	/* Store one T/P pair in one, F in the other byte. */
	/* We could save six bytes by putting all F values in two bytes. */
	FOR_EACH_CHANNEL(ch) {
		for (i = 0; i < 4; ++i) {
			const u32 reg = mchbar_read32(CxWRTy_MCHBAR(ch, i));
			bytes[(ch * 8) + (i * 2)] =
				(((reg & CxWRTy_T_MASK)
				  >> CxWRTy_T_SHIFT) << 4) |
				((reg & CxWRTy_P_MASK) >> CxWRTy_P_SHIFT);
			bytes[(ch * 8) + (i * 2) + 1] =
				((reg & CxWRTy_F_MASK) >> CxWRTy_F_SHIFT);
		}
	}

	/* Store everything in CMOS above 128 bytes. */
	for (i = 0; i < (TOTAL_CHANNELS * 4 * 2); ++i)
		cmos_write(bytes[i], CMOS_WRITE_TRAINING + i);
}
static void write_training_restore_results(const int memclk1067)
{
	const int t_bound = memclk1067 ? 12 : 11;

	u8 bytes[TOTAL_CHANNELS * 4 * 2]; /* two bytes per group */
	int ch, i;

	/* Read from CMOS. */
	for (i = 0; i < (TOTAL_CHANNELS * 4 * 2); ++i)
		bytes[i] = cmos_read(CMOS_WRITE_TRAINING + i);

	/* Program with original program_write_timing(). */
	FOR_EACH_CHANNEL(ch) {
		for (i = 0; i < 4; ++i) {
			write_timing_t timing = { 0, 0, t_bound, 0 };
			timing.f = bytes[(ch * 8) + (i * 2) + 1] & 3;
			timing.t = bytes[(ch * 8) + (i * 2)] >> 4;
			timing.p = bytes[(ch * 8) + (i * 2)] & 7;
			program_write_timing(ch, i, &timing, memclk1067);
			printk(BIOS_DEBUG, "Restored timings for group %d "
					   "on channel %d: %d.%d.%d\n",
			       i, ch, timing.f, timing.t, timing.p);
		}
	}
}
void raminit_write_training(const mem_clock_t ddr3clock,
			    const dimminfo_t *const dimms,
			    const int s3resume)
{
	const int memclk1067 = ddr3clock == MEM_CLOCK_1067MT;

	if (!s3resume) {
		perform_write_training(memclk1067, dimms);
		write_training_store_results();
	} else {
		write_training_restore_results(memclk1067);
	}
	raminit_reset_readwrite_pointers();
}
