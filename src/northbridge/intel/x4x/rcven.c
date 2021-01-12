/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/mmio.h>
#include <console/console.h>
#include <delay.h>
#include "raminit.h"
#include "x4x.h"

#define MAX_COARSE 15
#define DQS_HIGH 1
#define DQS_LOW 0

#define RESET_CNTL(channel) (0x5d8 + channel * 0x400)

struct rec_timing {
	u8 medium;
	u8 coarse;
	u8 pi;
	u8 tap;
};

static inline void mfence(void)
{
	asm volatile("mfence":::);
}

static u8 sampledqs(u32 addr, u8 lane, u8 channel)
{
	u32 sample_offset = 0x400 * channel + 0x561 + lane * 4;

	/* Reset the DQS probe, on both channels? */
	for (u8 i = 0; i < TOTAL_CHANNELS; i++) {
		MCHBAR8(RESET_CNTL(i)) &= ~0x2;
		udelay(1);
		MCHBAR8(RESET_CNTL(i)) |= 0x2;
		udelay(1);
	}
	mfence();
	/* Read strobe */
	read32((u32 *)addr);
	mfence();
	return (MCHBAR8(sample_offset) >> 6) & 1;
}

static void program_timing(const struct rec_timing *timing, u8 channel,
			u8 lane)
{
	u32 reg32;
	u16 reg16;
	u8 reg8;

	printk(RAM_SPEW, "      Programming timings:"
		"Coarse: %d, Medium: %d, TAP: %d, PI: %d\n",
		timing->coarse, timing->medium, timing->tap, timing->pi);

	reg32 = MCHBAR32(0x400 * channel + 0x248);
	reg32 &= ~0xf0000;
	reg32 |= timing->coarse << 16;
	MCHBAR32(0x400 * channel + 0x248) = reg32;

	reg16 = MCHBAR16(0x400 * channel + 0x58c);
	reg16 &= ~(3 << (lane * 2));
	reg16 |= timing->medium << (lane * 2);
	MCHBAR16(0x400 * channel + 0x58c) = reg16;

	reg8 = MCHBAR8(0x400 * channel + 0x560 + lane * 4);
	reg8 &= ~0x7f;
	reg8 |= timing->tap;
	reg8 |= timing->pi << 4;
	MCHBAR8(0x400 * channel + 0x560 + lane * 4) = reg8;
}

static int increase_medium(struct rec_timing *timing)
{
	if (timing->medium < 3) {
		timing->medium++;
	} else if (timing->coarse < MAX_COARSE) {
		timing->medium = 0;
		timing->coarse++;
	} else {
		printk(BIOS_ERR, "Cannot increase medium any further.\n");
		return -1;
	}
	return 0;
}

static int decrease_medium(struct rec_timing *timing)
{
	if (timing->medium > 0) {
		timing->medium--;
	} else if (timing->coarse > 0) {
		timing->medium = 3;
		timing->coarse--;
	} else {
		printk(BIOS_ERR, "Cannot lower medium any further.\n");
		return -1;
	}
	return 0;
}

static int increase_tap(struct rec_timing *timing)
{
	if (timing->tap == 14) {
		if (increase_medium(timing))
			return -1;
		timing->tap = 0;
	} else {
		timing->tap++;
	}
	return 0;
}

static int decrease_tap(struct rec_timing *timing)
{
	if (timing->tap > 0) {
		timing->tap--;
	} else {
		if (decrease_medium(timing))
			return -1;
		timing->tap = 14;
	}
	return 0;
}

static int decr_coarse_low(u8 channel, u8 lane, u32 addr,
			struct rec_timing *timing)
{
	printk(RAM_DEBUG,
		"  Decreasing coarse until high to low transition is found\n");
	while (sampledqs(addr, lane, channel) != DQS_LOW) {
		if (timing->coarse == 0) {
			printk(BIOS_CRIT,
				"Couldn't find DQS-high 0 indicator, halt\n");
			return -1;
		}
		timing->coarse--;
		program_timing(timing, channel, lane);
	}
	printk(RAM_DEBUG, "    DQS low at coarse=%d medium=%d\n",
		timing->coarse, timing->medium);
	return 0;
}

static int fine_search_dqs_high(u8 channel, u8 lane, u32 addr,
				struct rec_timing *timing)
{
	printk(RAM_DEBUG,
		"  Increasing TAP until low to high transition is found\n");
	/*
	 * We use a do while loop since it happens that the strobe read
	 * is inconsistent, with the strobe already high. The current
	 * code flow results in failure later when finding the preamble,
	 * at which DQS needs to be high is often not the case if TAP was
	 * not increased at least once here. Work around this by incrementing
	 * TAP at least once to guarantee searching for preamble start at
	 * DQS high.
	 * This seems to be the result of hysteresis on some settings, where
	 * the DQS probe is influenced by its previous value.
	 */
	if (sampledqs(addr, lane, channel) == DQS_HIGH) {
		printk(BIOS_WARNING,
			"DQS already HIGH... DQS probe is inconsistent!\n"
			"Continuing....\n");
	}
	do {
		if (increase_tap(timing)) {
			printk(BIOS_CRIT,
				"Could not find DQS-high on fine search.\n");
			return -1;
		}
		program_timing(timing, channel, lane);
	} while (sampledqs(addr, lane, channel) != DQS_HIGH);

	printk(RAM_DEBUG, "    DQS high at coarse=%d medium=%d tap:%d\n",
		timing->coarse, timing->medium, timing->tap);
	return 0;
}

static int find_dqs_low(u8 channel, u8 lane, u32 addr,
			struct rec_timing *timing)
{
	/* Look for DQS low, using quarter steps. */
	printk(RAM_DEBUG, "  Increasing medium until DQS LOW is found\n");
	while (sampledqs(addr, lane, channel) != DQS_LOW) {
		if (increase_medium(timing)) {
			printk(BIOS_CRIT,
				"Coarse > 15: DQS tuning failed, halt\n");
			return -1;
		}
		program_timing(timing, channel, lane);
	}
	printk(RAM_DEBUG, "    DQS low at coarse=%d medium=%d\n",
		timing->coarse, timing->medium);
	return 0;
}
static int find_dqs_high(u8 channel, u8 lane, u32 addr,
				struct rec_timing *timing)
{
	/* Look for DQS high, using quarter steps. */
	printk(RAM_DEBUG, "  Increasing medium until DQS HIGH is found\n");
	while (sampledqs(addr, lane, channel) != DQS_HIGH) {
		if (increase_medium(timing)) {
			printk(BIOS_CRIT,
				"Coarse > 16: DQS tuning failed, halt\n");
			return -1;
		}
		program_timing(timing, channel, lane);
	}
	printk(RAM_DEBUG, "    DQS high at coarse=%d medium=%d\n",
		timing->coarse, timing->medium);
	return 0;
}

static int find_dqs_edge_lowhigh(u8 channel, u8 lane,
				u32 addr, struct rec_timing *timing)
{
	/* Medium search for DQS high. */
	if (find_dqs_high(channel, lane, addr, timing))
		return -1;

	/* Go back and perform finer search. */
	if (decrease_medium(timing))
		return -1;
	program_timing(timing, channel, lane);
	if (fine_search_dqs_high(channel, lane, addr, timing) < 0)
		return -1;

	return 0;
}

static int find_preamble(u8 channel, u8 lane, u32 addr,
			struct rec_timing *timing)
{
	/* Add a quarter step */
	if (increase_medium(timing))
		return -1;
	program_timing(timing, channel, lane);
	/* Verify we are at high */
	if (sampledqs(addr, lane, channel) != DQS_HIGH) {
		printk(BIOS_CRIT, "Not at DQS high, d'oh\n");
		return -1;
	}

	/* Decrease coarse until LOW is found */
	if (decr_coarse_low(channel, lane, addr, timing))
		return -1;
	return 0;
}

static int calibrate_receive_enable(u8 channel, u8 lane,
				u32 addr, struct rec_timing *timing)
{
	program_timing(timing, channel, lane);
	/* Set receive enable bit */
	MCHBAR16(0x400 * channel + 0x588) = (MCHBAR16(0x400 * channel + 0x588)
				& ~(3 << (lane * 2))) | (1 << (lane * 2));

	if (find_dqs_low(channel, lane, addr, timing))
		return -1;

	/* Advance a little further. */
	if (increase_medium(timing)) {
		/* A finer search could be implemented */
		printk(BIOS_WARNING, "Cannot increase medium further");
		return -1;
	}
	program_timing(timing, channel, lane);

	if (find_dqs_edge_lowhigh(channel, lane, addr, timing))
		return -1;

	/* Go back on fine search */
	if (decrease_tap(timing))
		return -1;
	timing->pi = 3;
	program_timing(timing, channel, lane);

	if (find_preamble(channel, lane, addr, timing))
		return -1;

	if (find_dqs_edge_lowhigh(channel, lane, addr, timing))
		return -1;
	if (decrease_tap(timing))
		return -1;
	timing->pi = 7;
	program_timing(timing, channel, lane);

	/* Unset receive enable bit */
	MCHBAR16(0x400 * channel + 0x588) = MCHBAR16(0x400 * channel + 0x588) &
		~(3 << (lane * 2));
	return 0;
}

void rcven(struct sysinfo *s)
{
	int rank;
	u8 channel, lane, reg8;
	/*
	 * Using the macros below the compiler warns about this possibly being
	 * unitialised.
	 */
	u32 addr = 0;
	struct rec_timing timing[TOTAL_BYTELANES];
	u8 mincoarse;

	printk(BIOS_DEBUG, "Starting DQS receiver enable calibration\n");

	MCHBAR8(0x5d8) = MCHBAR8(0x5d8) & ~0xc;
	MCHBAR8(0x9d8) = MCHBAR8(0x9d8) & ~0xc;
	MCHBAR8(0x5dc) = MCHBAR8(0x5dc) & ~0x80;
	FOR_EACH_POPULATED_CHANNEL(s->dimms, channel) {
		mincoarse = 0xff;
		/*
		 * Receive enable calibration happens on the first populated
		 * rank on each channel.
		 */
		FOR_EACH_POPULATED_RANK_IN_CHANNEL(s->dimms, channel, rank) {
			addr = test_address(channel, rank);
			break;
		}
		FOR_EACH_BYTELANE(lane) {
			printk(BIOS_DEBUG, "Channel %d, Lane %d addr=0x%08x\n",
				channel, lane, addr);
			timing[lane].coarse = (s->selected_timings.CAS + 1);
			switch (lane) {
			default:
			case 0:
			case 1:
				timing[lane].medium = 0;
				break;
			case 2:
			case 3:
				timing[lane].medium = 1;
				break;
			case 4:
			case 5:
				timing[lane].medium = 2;
				break;
			case 6:
			case 7:
				timing[lane].medium = 3;
				break;
			}
			timing[lane].tap = 0;
			timing[lane].pi = 0;

			if (calibrate_receive_enable(channel, lane, addr,
							&timing[lane]))
				die("Receive enable calibration failed\n");
			if (mincoarse > timing[lane].coarse)
				mincoarse = timing[lane].coarse;
		}
		printk(BIOS_DEBUG, "Found min coarse value = %d\n", mincoarse);
		s->rcven_t[channel].min_common_coarse = mincoarse;
		printk(BIOS_DEBUG, "Receive enable, final timings:\n");
		/* Normalise coarse */
		FOR_EACH_BYTELANE(lane) {
			if (timing[lane].coarse == 0)
				reg8 = 0;
			else
				reg8 = timing[lane].coarse - mincoarse;
			printk(BIOS_DEBUG, "ch %d lane %d: coarse offset: %d;"
				"medium: %d; tap: %d\n",
				channel, lane, reg8, timing[lane].medium,
				timing[lane].tap);
			s->rcven_t[channel].coarse_offset[lane] = reg8;
			s->rcven_t[channel].medium[lane] = timing[lane].medium;
			s->rcven_t[channel].tap[lane] = timing[lane].tap;
			s->rcven_t[channel].pi[lane] = timing[lane].pi;
			MCHBAR16(0x400 * channel + 0x5fa) =
				(MCHBAR16(0x400 * channel + 0x5fa) &
				~(3 << (lane * 2))) | (reg8 << (lane * 2));
		}
		/* simply use timing[0] to program mincoarse */
		timing[0].coarse = mincoarse;
		program_timing(&timing[0], channel, 0);
	}
}
