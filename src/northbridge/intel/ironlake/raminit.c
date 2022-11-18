/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <commonlib/helpers.h>
#include <string.h>
#include <arch/io.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <device/smbus_host.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/cache.h>
#include <cbmem.h>
#include <cf9_reset.h>
#include <ip_checksum.h>
#include <option.h>
#include <device/pci_def.h>
#include <device/device.h>
#include <halt.h>
#include <spd.h>
#include <timestamp.h>
#include <cpu/x86/mtrr.h>
#include <cpu/intel/speedstep.h>
#include <cpu/intel/turbo.h>
#include <mrc_cache.h>
#include <southbridge/intel/ibexpeak/me.h>
#include <southbridge/intel/common/pmbase.h>
#include <delay.h>
#include <types.h>

#include "chip.h"
#include "ironlake.h"
#include "raminit.h"
#include "raminit_tables.h"

#define NORTHBRIDGE PCI_DEV(0, 0, 0)
#define SOUTHBRIDGE PCI_DEV(0, 0x1f, 0)
#define GMA PCI_DEV (0, 0x2, 0x0)

#define FOR_ALL_RANKS					   \
  for (channel = 0; channel < NUM_CHANNELS; channel++)	   \
    for (slot = 0; slot < NUM_SLOTS; slot++)		   \
      for (rank = 0; rank < NUM_RANKS; rank++)

#define FOR_POPULATED_RANKS				   \
  for (channel = 0; channel < NUM_CHANNELS; channel++)	   \
    for (slot = 0; slot < NUM_SLOTS; slot++)		   \
      for (rank = 0; rank < NUM_RANKS; rank++)		   \
	if (info->populated_ranks[channel][slot][rank])

#define FOR_POPULATED_RANKS_BACKWARDS				\
  for (channel = NUM_CHANNELS - 1; channel >= 0; channel--)	\
    for (slot = 0; slot < NUM_SLOTS; slot++)			\
      for (rank = 0; rank < NUM_RANKS; rank++)			\
	if (info->populated_ranks[channel][slot][rank])

#include <lib.h>		/* Prototypes */

typedef struct _u128 {
	u64 lo;
	u64 hi;
} u128;

static void read128(u32 addr, u64 * out)
{
	u128 ret;
	u128 stor;
	asm volatile ("movdqu %%xmm0, %0\n"
		      "movdqa (%2), %%xmm0\n"
		      "movdqu %%xmm0, %1\n"
		      "movdqu %0, %%xmm0":"+m" (stor), "=m"(ret):"r"(addr));
	out[0] = ret.lo;
	out[1] = ret.hi;
}

/*
 * Ironlake memory I/O timings are located in scan chains, accessible
 * through MCHBAR register groups. Each channel has a scan chain, and
 * there's a global scan chain too. Each chain is broken into smaller
 * sections of N bits, where N <= 32. Each section allows reading and
 * writing a certain parameter. Each section contains N - 2 data bits
 * and two additional bits: a Mask bit, and a Halt bit.
 */

/* OK */
static void write_1d0(u32 val, u16 addr, int bits, int flag)
{
	mchbar_write32(0x1d0, 0);
	while (mchbar_read32(0x1d0) & (1 << 23))
		;
	mchbar_write32(0x1d4, (val & ((1 << bits) - 1)) | 2 << bits | flag << bits);
	mchbar_write32(0x1d0, 1 << 30 | addr);
	while (mchbar_read32(0x1d0) & (1 << 23))
		;
}

/* OK */
static u16 read_1d0(u16 addr, int split)
{
	u32 val;
	mchbar_write32(0x1d0, 0);
	while (mchbar_read32(0x1d0) & (1 << 23))
		;
	mchbar_write32(0x1d0, 1 << 31 | (((mchbar_read8(0x246) >> 2) & 3) + 0x361 - addr));
	while (mchbar_read32(0x1d0) & (1 << 23))
		;
	val = mchbar_read32(0x1d8);
	write_1d0(0, 0x33d, 0, 0);
	write_1d0(0, 0x33d, 0, 0);
	val &= ((1 << split) - 1);
	//  printk (BIOS_ERR, "R1D0C [%x] => %x\n", addr, val);
	return val;
}

static void sfence(void)
{
	asm volatile ("sfence");
}

static inline u16 get_lane_offset(int slot, int rank, int lane)
{
	return 0x124 * lane + ((lane & 4) ? 0x23e : 0) + 11 * rank + 22 * slot -
	    0x452 * (lane == 8);
}

static inline u16 get_timing_register_addr(int lane, int tm, int slot, int rank)
{
	const u16 offs[] = { 0x1d, 0xa8, 0xe6, 0x5c };
	return get_lane_offset(slot, rank, lane) + offs[(tm + 3) % 4];
}

static u32 gav_real(int line, u32 in)
{
	//  printk (BIOS_DEBUG, "%d: GAV: %x\n", line, in);
	return in;
}

#define gav(x) gav_real (__LINE__, (x))

/* Global allocation of timings_car */
timing_bounds_t timings_car[64];

/* OK */
static u16
read_500(struct raminfo *info, int channel, u16 addr, int split)
{
	u32 val;
	info->last_500_command[channel] = 1 << 31;
	mchbar_write32(0x500 + (channel << 10), 0);
	while (mchbar_read32(0x500 + (channel << 10)) & (1 << 23))
		;
	mchbar_write32(0x500 + (channel << 10),
		1 << 31 | (((mchbar_read8(0x246 + (channel << 10)) >> 2) & 3) + 0xb88 - addr));
	while (mchbar_read32(0x500 + (channel << 10)) & (1 << 23))
		;
	val = mchbar_read32(0x508 + (channel << 10));
	return val & ((1 << split) - 1);
}

/* OK */
static void
write_500(struct raminfo *info, int channel, u32 val, u16 addr, int bits,
	  int flag)
{
	if (info->last_500_command[channel] == 1 << 31) {
		info->last_500_command[channel] = 1 << 30;
		write_500(info, channel, 0, 0xb61, 0, 0);
	}
	mchbar_write32(0x500 + (channel << 10), 0);
	while (mchbar_read32(0x500 + (channel << 10)) & (1 << 23))
		;
	mchbar_write32(0x504 + (channel << 10),
		(val & ((1 << bits) - 1)) | 2 << bits | flag << bits);
	mchbar_write32(0x500 + (channel << 10), 1 << 30 | addr);
	while (mchbar_read32(0x500 + (channel << 10)) & (1 << 23))
		;
}

static void rmw_500(struct raminfo *info, int channel, u16 addr, int bits, u32 and, u32 or)
{
	const u32 val = read_500(info, channel, addr, bits) & and;
	write_500(info, channel, val | or, addr, bits, 1);
}

static int rw_test(int rank)
{
	const u32 mask = 0xf00fc33c;
	int ok = 0xff;
	int i;
	for (i = 0; i < 64; i++)
		write32p((rank << 28) | (i << 2), 0);
	sfence();
	for (i = 0; i < 64; i++)
		gav(read32p((rank << 28) | (i << 2)));
	sfence();
	for (i = 0; i < 32; i++) {
		u32 pat = (((mask >> i) & 1) ? 0xffffffff : 0);
		write32p((rank << 28) | (i << 3), pat);
		write32p((rank << 28) | (i << 3) | 4, pat);
	}
	sfence();
	for (i = 0; i < 32; i++) {
		u8 pat = (((mask >> i) & 1) ? 0xff : 0);
		int j;
		u32 val;
		gav(val = read32p((rank << 28) | (i << 3)));
		for (j = 0; j < 4; j++)
			if (((val >> (j * 8)) & 0xff) != pat)
				ok &= ~(1 << j);
		gav(val = read32p((rank << 28) | (i << 3) | 4));
		for (j = 0; j < 4; j++)
			if (((val >> (j * 8)) & 0xff) != pat)
				ok &= ~(16 << j);
	}
	sfence();
	for (i = 0; i < 64; i++)
		write32p((rank << 28) | (i << 2), 0);
	sfence();
	for (i = 0; i < 64; i++)
		gav(read32p((rank << 28) | (i << 2)));

	return ok;
}

static void
program_timings(struct raminfo *info, u16 base, int channel, int slot, int rank)
{
	int lane;
	for (lane = 0; lane < 8; lane++) {
		write_500(info, channel,
			  base +
			  info->training.
			  lane_timings[2][channel][slot][rank][lane],
			  get_timing_register_addr(lane, 2, slot, rank), 9, 0);
		write_500(info, channel,
			  base +
			  info->training.
			  lane_timings[3][channel][slot][rank][lane],
			  get_timing_register_addr(lane, 3, slot, rank), 9, 0);
	}
}

static void write_26c(int channel, u16 si)
{
	mchbar_write32(0x26c + (channel << 10), 0x03243f35);
	mchbar_write32(0x268 + (channel << 10), 0xcfc00000 | si << 9);
	mchbar_write16(0x2b9 + (channel << 10), si);
}

static void toggle_1d0_142_5ff(void)
{
	u32 reg32 = gav(read_1d0(0x142, 3));
	if (reg32 & (1 << 1))
		write_1d0(0, 0x142, 3, 1);

	mchbar_write8(0x5ff, 0);
	mchbar_write8(0x5ff, 1 << 7);
	if (reg32 & (1 << 1))
		write_1d0(0x2, 0x142, 3, 1);
}

static u32 get_580(int channel, u8 addr)
{
	u32 ret;
	toggle_1d0_142_5ff();
	mchbar_write32(0x580 + (channel << 10), 0x8493c012 | addr);
	mchbar_setbits8(0x580 + (channel << 10), 1 << 0);
	while (!((ret = mchbar_read32(0x580 + (channel << 10))) & (1 << 16)))
		;
	mchbar_clrbits8(0x580 + (channel << 10), 1 << 0);
	return ret;
}

#define RANK_SHIFT 28
#define CHANNEL_SHIFT 10

static void seq9(struct raminfo *info, int channel, int slot, int rank)
{
	int i, lane;

	for (i = 0; i < 2; i++)
		for (lane = 0; lane < 8; lane++)
			write_500(info, channel,
				  info->training.lane_timings[i +
							      1][channel][slot]
				  [rank][lane], get_timing_register_addr(lane,
									 i + 1,
									 slot,
									 rank),
				  9, 0);

	write_1d0(1, 0x103, 6, 1);
	for (lane = 0; lane < 8; lane++)
		write_500(info, channel,
			  info->training.
			  lane_timings[0][channel][slot][rank][lane],
			  get_timing_register_addr(lane, 0, slot, rank), 9, 0);

	for (i = 0; i < 2; i++) {
		for (lane = 0; lane < 8; lane++)
			write_500(info, channel,
				  info->training.lane_timings[i +
							      1][channel][slot]
				  [rank][lane], get_timing_register_addr(lane,
									 i + 1,
									 slot,
									 rank),
				  9, 0);
		gav(get_580(channel, ((i + 1) << 2) | (rank << 5)));
	}

	toggle_1d0_142_5ff();
	write_1d0(0x2, 0x142, 3, 1);

	for (lane = 0; lane < 8; lane++) {
		//      printk (BIOS_ERR, "before: %x\n", info->training.lane_timings[2][channel][slot][rank][lane]);
		info->training.lane_timings[2][channel][slot][rank][lane] =
		    read_500(info, channel,
			     get_timing_register_addr(lane, 2, slot, rank), 9);
		//printk (BIOS_ERR, "after: %x\n", info->training.lane_timings[2][channel][slot][rank][lane]);
		info->training.lane_timings[3][channel][slot][rank][lane] =
		    info->training.lane_timings[2][channel][slot][rank][lane] +
		    0x20;
	}
}

static int count_ranks_in_channel(struct raminfo *info, int channel)
{
	int slot, rank;
	int res = 0;
	for (slot = 0; slot < NUM_SLOTS; slot++)
		for (rank = 0; rank < NUM_SLOTS; rank++)
			res += info->populated_ranks[channel][slot][rank];
	return res;
}

static void
config_rank(struct raminfo *info, int s3resume, int channel, int slot, int rank)
{
	int add;

	write_1d0(0, 0x178, 7, 1);
	seq9(info, channel, slot, rank);
	program_timings(info, 0x80, channel, slot, rank);

	if (channel == 0)
		add = count_ranks_in_channel(info, 1);
	else
		add = 0;
	if (!s3resume)
		gav(rw_test(rank + add));
	program_timings(info, 0x00, channel, slot, rank);
	if (!s3resume)
		gav(rw_test(rank + add));
	if (!s3resume)
		gav(rw_test(rank + add));
	write_1d0(0, 0x142, 3, 1);
	write_1d0(0, 0x103, 6, 1);

	gav(get_580(channel, 0xc | (rank << 5)));
	gav(read_1d0(0x142, 3));

	mchbar_write8(0x5ff, 0);
	mchbar_write8(0x5ff, 1 << 7);
}

static void set_4cf(struct raminfo *info, int channel, u8 bit, u8 val)
{
	const u16 regtable[] = { 0x4cf, 0x659, 0x697 };

	val &= 1;
	for (int i = 0; i < ARRAY_SIZE(regtable); i++)
		rmw_500(info, channel, regtable[i], 4, ~(1 << bit), val << bit);
}

static void set_334(int zero)
{
	int j, k, channel;
	const u32 val3[] = { 0x2a2b2a2b, 0x26272627, 0x2e2f2e2f, 0x2a2b };
	u32 vd8[2][16];

	for (channel = 0; channel < NUM_CHANNELS; channel++) {
		for (j = 0; j < 4; j++) {
			u32 a = (j == 1) ? 0x29292929 : 0x31313131;
			u32 lmask = (j == 3) ? 0xffff : 0xffffffff;
			u16 c;
			if ((j == 0 || j == 3) && zero)
				c = 0;
			else if (j == 3)
				c = 0x5f;
			else
				c = 0x5f5f;

			for (k = 0; k < 2; k++) {
				mchbar_write32(0x138 + 8 * k, channel << 26 | j << 24);
				gav(vd8[1][(channel << 3) | (j << 1) | k] =
				    mchbar_read32(0x138 + 8 * k));
				gav(vd8[0][(channel << 3) | (j << 1) | k] =
				    mchbar_read32(0x13c + 8 * k));
			}

			mchbar_write32(0x334 + (channel << 10) + j * 0x44, zero ? 0 : val3[j]);
			mchbar_write32(0x32c + (channel << 10) + j * 0x44,
					zero ? 0 : 0x18191819 & lmask);
			mchbar_write16(0x34a + (channel << 10) + j * 0x44, c);
			mchbar_write32(0x33c + (channel << 10) + j * 0x44,
					zero ? 0 : a & lmask);
			mchbar_write32(0x344 + (channel << 10) + j * 0x44,
					zero ? 0 : a & lmask);
		}
	}

	mchbar_setbits32(0x130, 1 << 0);
	while (mchbar_read8(0x130) & 1)
		;
}

static void rmw_1d0(u16 addr, u32 and, u32 or, int split)
{
	u32 v;
	v = read_1d0(addr, split);
	write_1d0((v & and) | or, addr, split, 1);
}

static int find_highest_bit_set(u16 val)
{
	int i;
	for (i = 15; i >= 0; i--)
		if (val & (1 << i))
			return i;
	return -1;
}

static int find_lowest_bit_set32(u32 val)
{
	int i;
	for (i = 0; i < 32; i++)
		if (val & (1 << i))
			return i;
	return -1;
}

enum {
	DEVICE_TYPE = 2,
	MODULE_TYPE = 3,
	DENSITY = 4,
	RANKS_AND_DQ = 7,
	MEMORY_BUS_WIDTH = 8,
	TIMEBASE_DIVIDEND = 10,
	TIMEBASE_DIVISOR = 11,
	CYCLETIME = 12,

	CAS_LATENCIES_LSB = 14,
	CAS_LATENCIES_MSB = 15,
	CAS_LATENCY_TIME = 16,
	THERMAL_AND_REFRESH = 31,
	REFERENCE_RAW_CARD_USED = 62,
	RANK1_ADDRESS_MAPPING = 63
};

static void calculate_timings(struct raminfo *info)
{
	unsigned int cycletime;
	unsigned int cas_latency_time;
	unsigned int supported_cas_latencies;
	unsigned int channel, slot;
	unsigned int clock_speed_index;
	unsigned int min_cas_latency;
	unsigned int cas_latency;
	unsigned int max_clock_index;

	/* Find common CAS latency  */
	supported_cas_latencies = 0x3fe;
	for (channel = 0; channel < NUM_CHANNELS; channel++)
		for (slot = 0; slot < NUM_SLOTS; slot++)
			if (info->populated_ranks[channel][slot][0])
				supported_cas_latencies &=
				    2 *
				    (info->
				     spd[channel][slot][CAS_LATENCIES_LSB] |
				     (info->
				      spd[channel][slot][CAS_LATENCIES_MSB] <<
				      8));

	max_clock_index = MIN(3, info->max_supported_clock_speed_index);

	cycletime = min_cycletime[max_clock_index];
	cas_latency_time = min_cas_latency_time[max_clock_index];

	for (channel = 0; channel < NUM_CHANNELS; channel++)
		for (slot = 0; slot < NUM_SLOTS; slot++)
			if (info->populated_ranks[channel][slot][0]) {
				unsigned int timebase;
				timebase =
				    1000 *
				    info->
				    spd[channel][slot][TIMEBASE_DIVIDEND] /
				    info->spd[channel][slot][TIMEBASE_DIVISOR];
				cycletime =
				    MAX(cycletime,
					timebase *
					info->spd[channel][slot][CYCLETIME]);
				cas_latency_time =
				    MAX(cas_latency_time,
					timebase *
					info->
					spd[channel][slot][CAS_LATENCY_TIME]);
			}
	if (cycletime > min_cycletime[0])
		die("RAM init: Decoded SPD DRAM freq is slower than the controller minimum!");
	for (clock_speed_index = 0; clock_speed_index < 3; clock_speed_index++) {
		if (cycletime == min_cycletime[clock_speed_index])
			break;
		if (cycletime > min_cycletime[clock_speed_index]) {
			clock_speed_index--;
			cycletime = min_cycletime[clock_speed_index];
			break;
		}
	}
	min_cas_latency = DIV_ROUND_UP(cas_latency_time, cycletime);
	cas_latency = 0;
	while (supported_cas_latencies) {
		cas_latency = find_highest_bit_set(supported_cas_latencies) + 3;
		if (cas_latency <= min_cas_latency)
			break;
		supported_cas_latencies &=
		    ~(1 << find_highest_bit_set(supported_cas_latencies));
	}

	if (cas_latency != min_cas_latency && clock_speed_index)
		clock_speed_index--;

	if (cas_latency * min_cycletime[clock_speed_index] > 20000)
		die("Couldn't configure DRAM");
	info->clock_speed_index = clock_speed_index;
	info->cas_latency = cas_latency;
}

static void program_base_timings(struct raminfo *info)
{
	unsigned int channel;
	unsigned int slot, rank, lane;
	unsigned int extended_silicon_revision;
	int i;

	extended_silicon_revision = info->silicon_revision;
	if (info->silicon_revision == 0)
		for (channel = 0; channel < NUM_CHANNELS; channel++)
			for (slot = 0; slot < NUM_SLOTS; slot++)
				if ((info->
				     spd[channel][slot][MODULE_TYPE] & 0xF) ==
				    3)
					extended_silicon_revision = 4;

	for (channel = 0; channel < NUM_CHANNELS; channel++) {
		for (slot = 0; slot < NUM_SLOTS; slot++)
			for (rank = 0; rank < NUM_SLOTS; rank++) {
				int card_timing_2;
				if (!info->populated_ranks[channel][slot][rank])
					continue;

				for (lane = 0; lane < 9; lane++) {
					int tm_reg;
					int card_timing;

					card_timing = 0;
					if ((info->
					     spd[channel][slot][MODULE_TYPE] &
					     0xF) == 3) {
						int reference_card;
						reference_card =
						    info->
						    spd[channel][slot]
						    [REFERENCE_RAW_CARD_USED] &
						    0x1f;
						if (reference_card == 3)
							card_timing =
							    u16_ffd1188[0][lane]
							    [info->
							     clock_speed_index];
						if (reference_card == 5)
							card_timing =
							    u16_ffd1188[1][lane]
							    [info->
							     clock_speed_index];
					}

					info->training.
					    lane_timings[0][channel][slot][rank]
					    [lane] =
					    u8_FFFD1218[info->
							clock_speed_index];
					info->training.
					    lane_timings[1][channel][slot][rank]
					    [lane] = 256;

					for (tm_reg = 2; tm_reg < 4; tm_reg++)
						info->training.
						    lane_timings[tm_reg]
						    [channel][slot][rank][lane]
						    =
						    u8_FFFD1240[channel]
						    [extended_silicon_revision]
						    [lane][2 * slot +
							   rank][info->
								 clock_speed_index]
						    + info->max4048[channel]
						    +
						    u8_FFFD0C78[channel]
						    [extended_silicon_revision]
						    [info->
						     mode4030[channel]][slot]
						    [rank][info->
							   clock_speed_index]
						    + card_timing;
					for (tm_reg = 0; tm_reg < 4; tm_reg++)
						write_500(info, channel,
							  info->training.
							  lane_timings[tm_reg]
							  [channel][slot][rank]
							  [lane],
							  get_timing_register_addr
							  (lane, tm_reg, slot,
							   rank), 9, 0);
				}

				card_timing_2 = 0;
				if (!(extended_silicon_revision != 4
				      || (info->
					  populated_ranks_mask[channel] & 5) ==
				      5)) {
					if ((info->
					     spd[channel][slot]
					     [REFERENCE_RAW_CARD_USED] & 0x1F)
					    == 3)
						card_timing_2 =
						    u16_FFFE0EB8[0][info->
								    clock_speed_index];
					if ((info->
					     spd[channel][slot]
					     [REFERENCE_RAW_CARD_USED] & 0x1F)
					    == 5)
						card_timing_2 =
						    u16_FFFE0EB8[1][info->
								    clock_speed_index];
				}

				for (i = 0; i < 3; i++)
					write_500(info, channel,
						  (card_timing_2 +
						   info->max4048[channel]
						   +
						   u8_FFFD0EF8[channel]
						   [extended_silicon_revision]
						   [info->
						    mode4030[channel]][info->
								       clock_speed_index]),
						  u16_fffd0c50[i][slot][rank],
						  8, 1);
				write_500(info, channel,
					  (info->max4048[channel] +
					   u8_FFFD0C78[channel]
					   [extended_silicon_revision][info->
								       mode4030
								       [channel]]
					   [slot][rank][info->
							clock_speed_index]),
					  u16_fffd0c70[slot][rank], 7, 1);
			}
		if (!info->populated_ranks_mask[channel])
			continue;
		for (i = 0; i < 3; i++)
			write_500(info, channel,
				  (info->max4048[channel] +
				   info->avg4044[channel]
				   +
				   u8_FFFD17E0[channel]
				   [extended_silicon_revision][info->
							       mode4030
							       [channel]][info->
									  clock_speed_index]),
				  u16_fffd0c68[i], 8, 1);
	}
}

/* The time of clock cycle in ps.  */
static unsigned int cycle_ps(struct raminfo *info)
{
	return 2 * halfcycle_ps(info);
}

/* Frequency in 0.1 MHz units. */
static unsigned int frequency_01(struct raminfo *info)
{
	return 100 * frequency_11(info) / 9;
}

static unsigned int ps_to_halfcycles(struct raminfo *info, unsigned int ps)
{
	return (frequency_11(info) * 2) * ps / 900000;
}

static unsigned int ns_to_cycles(struct raminfo *info, unsigned int ns)
{
	return (frequency_11(info)) * ns / 900;
}

static void compute_derived_timings(struct raminfo *info)
{
	unsigned int channel, slot, rank;
	int extended_silicon_revision;
	int some_delay_1_ps;
	int some_delay_2_ps;
	int some_delay_2_halfcycles_ceil;
	int some_delay_2_halfcycles_floor;
	int some_delay_3_ps;
	int some_delay_3_ps_rounded;
	int some_delay_1_cycle_ceil;
	int some_delay_1_cycle_floor;

	some_delay_3_ps_rounded = 0;
	extended_silicon_revision = info->silicon_revision;
	if (!info->silicon_revision)
		for (channel = 0; channel < NUM_CHANNELS; channel++)
			for (slot = 0; slot < NUM_SLOTS; slot++)
				if ((info->
				     spd[channel][slot][MODULE_TYPE] & 0xF) ==
				    3)
					extended_silicon_revision = 4;
	if (info->board_lane_delay[7] < 5)
		info->board_lane_delay[7] = 5;
	info->revision_flag_1 = 2;
	if (info->silicon_revision == 2 || info->silicon_revision == 3)
		info->revision_flag_1 = 0;
	if (info->revision < 16)
		info->revision_flag_1 = 0;

	if (info->revision < 8)
		info->revision_flag_1 = 0;
	if (info->revision >= 8 && (info->silicon_revision == 0
				    || info->silicon_revision == 1))
		some_delay_2_ps = 735;
	else
		some_delay_2_ps = 750;

	if (info->revision >= 0x10 && (info->silicon_revision == 0
				       || info->silicon_revision == 1))
		some_delay_1_ps = 3929;
	else
		some_delay_1_ps = 3490;

	some_delay_1_cycle_floor = some_delay_1_ps / cycle_ps(info);
	some_delay_1_cycle_ceil = some_delay_1_ps / cycle_ps(info);
	if (some_delay_1_ps % cycle_ps(info))
		some_delay_1_cycle_ceil++;
	else
		some_delay_1_cycle_floor--;
	info->some_delay_1_cycle_floor = some_delay_1_cycle_floor;
	if (info->revision_flag_1)
		some_delay_2_ps = halfcycle_ps(info) >> 6;
	some_delay_2_ps +=
	    MAX(some_delay_1_ps - 30,
		2 * halfcycle_ps(info) * (some_delay_1_cycle_ceil - 1) + 1000) +
	    375;
	some_delay_3_ps =
	    halfcycle_ps(info) - some_delay_2_ps % halfcycle_ps(info);
	if (info->revision_flag_1) {
		if (some_delay_3_ps >= 150) {
			const int some_delay_3_halfcycles =
			    (some_delay_3_ps << 6) / halfcycle_ps(info);
			some_delay_3_ps_rounded =
			    halfcycle_ps(info) * some_delay_3_halfcycles >> 6;
		}
	}
	some_delay_2_halfcycles_ceil =
	    (some_delay_2_ps + halfcycle_ps(info) - 1) / halfcycle_ps(info) -
	    2 * (some_delay_1_cycle_ceil - 1);
	if (info->revision_flag_1 && some_delay_3_ps < 150)
		some_delay_2_halfcycles_ceil++;
	some_delay_2_halfcycles_floor = some_delay_2_halfcycles_ceil;
	if (info->revision < 0x10)
		some_delay_2_halfcycles_floor =
		    some_delay_2_halfcycles_ceil - 1;
	if (!info->revision_flag_1)
		some_delay_2_halfcycles_floor++;
	/* FIXME: this variable is unused. Should it be used? */
	(void)some_delay_2_halfcycles_floor;
	info->some_delay_2_halfcycles_ceil = some_delay_2_halfcycles_ceil;
	info->some_delay_3_ps_rounded = some_delay_3_ps_rounded;
	if ((info->populated_ranks[0][0][0] && info->populated_ranks[0][1][0])
	    || (info->populated_ranks[1][0][0]
		&& info->populated_ranks[1][1][0]))
		info->max_slots_used_in_channel = 2;
	else
		info->max_slots_used_in_channel = 1;
	for (channel = 0; channel < NUM_CHANNELS; channel++)
		mchbar_write32(0x244 + (channel << 10),
			((info->revision < 8) ? 1 : 0x200) |
			((2 - info->max_slots_used_in_channel) << 17) |
			(channel << 21) |
			(info->some_delay_1_cycle_floor << 18) | 0x9510);
	if (info->max_slots_used_in_channel == 1) {
		info->mode4030[0] = (count_ranks_in_channel(info, 0) == 2);
		info->mode4030[1] = (count_ranks_in_channel(info, 1) == 2);
	} else {
		info->mode4030[0] = ((count_ranks_in_channel(info, 0) == 1) || (count_ranks_in_channel(info, 0) == 2)) ? 2 : 3;	/* 2 if 1 or 2 ranks */
		info->mode4030[1] = ((count_ranks_in_channel(info, 1) == 1)
				     || (count_ranks_in_channel(info, 1) ==
					 2)) ? 2 : 3;
	}
	for (channel = 0; channel < NUM_CHANNELS; channel++) {
		int max_of_unk;
		int min_of_unk_2;

		int i, count;
		int sum;

		if (!info->populated_ranks_mask[channel])
			continue;

		max_of_unk = 0;
		min_of_unk_2 = 32767;

		sum = 0;
		count = 0;
		for (i = 0; i < 3; i++) {
			int unk1;
			if (info->revision < 8)
				unk1 =
				    u8_FFFD1891[0][channel][info->
							    clock_speed_index]
				    [i];
			else if (!
				 (info->revision >= 0x10
				  || info->revision_flag_1))
				unk1 =
				    u8_FFFD1891[1][channel][info->
							    clock_speed_index]
				    [i];
			else
				unk1 = 0;
			for (slot = 0; slot < NUM_SLOTS; slot++)
				for (rank = 0; rank < NUM_RANKS; rank++) {
					int a = 0;
					int b = 0;

					if (!info->
					    populated_ranks[channel][slot]
					    [rank])
						continue;
					if (extended_silicon_revision == 4
					    && (info->
						populated_ranks_mask[channel] &
						5) != 5) {
						if ((info->
						     spd[channel][slot]
						     [REFERENCE_RAW_CARD_USED] &
						     0x1F) == 3) {
							a = u16_ffd1178[0]
							    [info->
							     clock_speed_index];
							b = u16_fe0eb8[0][info->
									  clock_speed_index];
						} else
						    if ((info->
							 spd[channel][slot]
							 [REFERENCE_RAW_CARD_USED]
							 & 0x1F) == 5) {
							a = u16_ffd1178[1]
							    [info->
							     clock_speed_index];
							b = u16_fe0eb8[1][info->
									  clock_speed_index];
						}
					}
					min_of_unk_2 = MIN(min_of_unk_2, a);
					min_of_unk_2 = MIN(min_of_unk_2, b);
					if (rank == 0) {
						sum += a;
						count++;
					}
					{
						int t;
						t = b +
						    u8_FFFD0EF8[channel]
						    [extended_silicon_revision]
						    [info->
						     mode4030[channel]][info->
									clock_speed_index];
						if (unk1 >= t)
							max_of_unk =
							    MAX(max_of_unk,
								unk1 - t);
					}
				}
			{
				int t =
				    u8_FFFD17E0[channel]
				    [extended_silicon_revision][info->
								mode4030
								[channel]]
				    [info->clock_speed_index] + min_of_unk_2;
				if (unk1 >= t)
					max_of_unk = MAX(max_of_unk, unk1 - t);
			}
		}

		if (count == 0)
			die("No memory ranks found for channel %u\n", channel);

		info->avg4044[channel] = sum / count;
		info->max4048[channel] = max_of_unk;
	}
}

static void jedec_read(struct raminfo *info,
		       int channel, int slot, int rank,
		       int total_rank, u8 addr3, unsigned int value)
{
	/* Handle mirrored mapping.  */
	if ((rank & 1) && (info->spd[channel][slot][RANK1_ADDRESS_MAPPING] & 1)) {
		addr3 = (addr3 & 0xCF) | ((addr3 & 0x10) << 1) | ((addr3 >> 1) & 0x10);
		value = (value & ~0x1f8) | ((value >> 1) & 0xa8) | ((value & 0xa8) << 1);
	}

	mchbar_clrsetbits8(0x271, 0x1f << 1, addr3);
	mchbar_clrsetbits8(0x671, 0x1f << 1, addr3);

	read32p((value << 3) | (total_rank << 28));

	mchbar_clrsetbits8(0x271, 0x1f << 1, 1 << 1);
	mchbar_clrsetbits8(0x671, 0x1f << 1, 1 << 1);

	read32p(total_rank << 28);
}

enum {
	MR1_RZQ12 = 512,
	MR1_RZQ2 = 64,
	MR1_RZQ4 = 4,
	MR1_ODS34OHM = 2
};

enum {
	MR0_BT_INTERLEAVED = 8,
	MR0_DLL_RESET_ON = 256
};

enum {
	MR2_RTT_WR_DISABLED = 0,
	MR2_RZQ2 = 1 << 10
};

static void jedec_init(struct raminfo *info)
{
	int write_recovery;
	int channel, slot, rank;
	int total_rank;
	int dll_on;
	int self_refresh_temperature;
	int auto_self_refresh;

	auto_self_refresh = 1;
	self_refresh_temperature = 1;
	if (info->board_lane_delay[3] <= 10) {
		if (info->board_lane_delay[3] <= 8)
			write_recovery = info->board_lane_delay[3] - 4;
		else
			write_recovery = 5;
	} else {
		write_recovery = 6;
	}
	FOR_POPULATED_RANKS {
		auto_self_refresh &=
		    (info->spd[channel][slot][THERMAL_AND_REFRESH] >> 2) & 1;
		self_refresh_temperature &=
		    info->spd[channel][slot][THERMAL_AND_REFRESH] & 1;
	}
	if (auto_self_refresh == 1)
		self_refresh_temperature = 0;

	dll_on = ((info->silicon_revision != 2 && info->silicon_revision != 3)
		  || (info->populated_ranks[0][0][0]
		      && info->populated_ranks[0][1][0])
		  || (info->populated_ranks[1][0][0]
		      && info->populated_ranks[1][1][0]));

	total_rank = 0;

	for (channel = NUM_CHANNELS - 1; channel >= 0; channel--) {
		int rtt, rtt_wr = MR2_RTT_WR_DISABLED;
		int rzq_reg58e;

		if (info->silicon_revision == 2 || info->silicon_revision == 3) {
			rzq_reg58e = 64;
			rtt = MR1_RZQ2;
			if (info->clock_speed_index != 0) {
				rzq_reg58e = 4;
				if (info->populated_ranks_mask[channel] == 3)
					rtt = MR1_RZQ4;
			}
		} else {
			if ((info->populated_ranks_mask[channel] & 5) == 5) {
				rtt = MR1_RZQ12;
				rzq_reg58e = 64;
				rtt_wr = MR2_RZQ2;
			} else {
				rzq_reg58e = 4;
				rtt = MR1_RZQ4;
			}
		}

		mchbar_write16(0x588 + (channel << 10), 0);
		mchbar_write16(0x58a + (channel << 10), 4);
		mchbar_write16(0x58c + (channel << 10), rtt | MR1_ODS34OHM);
		mchbar_write16(0x58e + (channel << 10), rzq_reg58e | 0x82);
		mchbar_write16(0x590 + (channel << 10), 0x1282);

		for (slot = 0; slot < NUM_SLOTS; slot++)
			for (rank = 0; rank < NUM_RANKS; rank++)
				if (info->populated_ranks[channel][slot][rank]) {
					jedec_read(info, channel, slot, rank,
						   total_rank, 0x28,
						   rtt_wr | (info->
							     clock_speed_index
							     << 3)
						   | (auto_self_refresh << 6) |
						   (self_refresh_temperature <<
						    7));
					jedec_read(info, channel, slot, rank,
						   total_rank, 0x38, 0);
					jedec_read(info, channel, slot, rank,
						   total_rank, 0x18,
						   rtt | MR1_ODS34OHM);
					jedec_read(info, channel, slot, rank,
						   total_rank, 6,
						   (dll_on << 12) |
						   (write_recovery << 9)
						   | ((info->cas_latency - 4) <<
						      4) | MR0_BT_INTERLEAVED |
						   MR0_DLL_RESET_ON);
					total_rank++;
				}
	}
}

static void program_modules_memory_map(struct raminfo *info, int pre_jedec)
{
	unsigned int channel, slot, rank;
	unsigned int total_mb[2] = { 0, 0 };	/* total memory per channel in MB */
	unsigned int channel_0_non_interleaved;

	FOR_ALL_RANKS {
		if (info->populated_ranks[channel][slot][rank]) {
			total_mb[channel] +=
			    pre_jedec ? 256 : (256 << info->
					       density[channel][slot] >> info->
					       is_x16_module[channel][slot]);
			mchbar_write8(0x208 + rank + 2 * slot + (channel << 10),
				(pre_jedec ? (1 | ((1 + 1) << 1)) :
				(info->is_x16_module[channel][slot] |
				((info->density[channel][slot] + 1) << 1))) |
				0x80);
		}
		mchbar_write16(0x200 + (channel << 10) + 4 * slot + 2 * rank,
			total_mb[channel] >> 6);
	}

	info->total_memory_mb = total_mb[0] + total_mb[1];

	info->interleaved_part_mb =
	    pre_jedec ? 0 : 2 * MIN(total_mb[0], total_mb[1]);
	info->non_interleaved_part_mb =
	    total_mb[0] + total_mb[1] - info->interleaved_part_mb;
	channel_0_non_interleaved = total_mb[0] - info->interleaved_part_mb / 2;
	mchbar_write32(0x100, channel_0_non_interleaved | info->non_interleaved_part_mb << 16);
	if (!pre_jedec)
		mchbar_write16(0x104, info->interleaved_part_mb);
}

static void program_board_delay(struct raminfo *info)
{
	int cas_latency_shift;
	int some_delay_ns;
	int some_delay_3_half_cycles;

	unsigned int channel, i;
	int high_multiplier;
	int lane_3_delay;
	int cas_latency_derived;

	high_multiplier = 0;
	some_delay_ns = 200;
	some_delay_3_half_cycles = 4;
	cas_latency_shift = info->silicon_revision == 0
	    || info->silicon_revision == 1 ? 1 : 0;
	if (info->revision < 8) {
		some_delay_ns = 600;
		cas_latency_shift = 0;
	}
	{
		int speed_bit;
		speed_bit =
		    ((info->clock_speed_index > 1
		      || (info->silicon_revision != 2
			  && info->silicon_revision != 3))) ^ (info->revision >=
							       0x10);
		write_500(info, 0, speed_bit | ((!info->use_ecc) << 1), 0x60e,
			  3, 1);
		write_500(info, 1, speed_bit | ((!info->use_ecc) << 1), 0x60e,
			  3, 1);
		if (info->revision >= 0x10 && info->clock_speed_index <= 1
		    && (info->silicon_revision == 2
			|| info->silicon_revision == 3))
			rmw_1d0(0x116, 5, 2, 4);
	}
	mchbar_write32(0x120, 1 << (info->max_slots_used_in_channel + 28) | 0x188e7f9f);

	mchbar_write8(0x124, info->board_lane_delay[4] + (frequency_01(info) + 999) / 1000);
	mchbar_write16(0x125, 0x1360);
	mchbar_write8(0x127, 0x40);
	if (info->fsb_frequency < frequency_11(info) / 2) {
		unsigned int some_delay_2_half_cycles;
		high_multiplier = 1;
		some_delay_2_half_cycles = ps_to_halfcycles(info,
							    ((3 *
							      fsbcycle_ps(info))
							     >> 1) +
							    (halfcycle_ps(info)
							     *
							     reg178_min[info->
									clock_speed_index]
							     >> 6)
							    +
							    4 *
							    halfcycle_ps(info)
							    + 2230);
		some_delay_3_half_cycles =
		    MIN((some_delay_2_half_cycles +
			 (frequency_11(info) * 2) * (28 -
						     some_delay_2_half_cycles) /
			 (frequency_11(info) * 2 -
			  4 * (info->fsb_frequency))) >> 3, 7);
	}
	if (mchbar_read8(0x2ca9) & 1)
		some_delay_3_half_cycles = 3;
	for (channel = 0; channel < NUM_CHANNELS; channel++) {
		mchbar_setbits32(0x220 + (channel << 10), 0x18001117);
		mchbar_write32(0x224 + (channel << 10),
			(info->max_slots_used_in_channel - 1) |
			(info->cas_latency - 5 - info->clock_speed_index)
			<< 21 | (info->max_slots_used_in_channel +
			info->cas_latency - cas_latency_shift - 4) << 16 |
			(info->cas_latency - cas_latency_shift - 4) << 26 |
			(info->cas_latency - info->clock_speed_index +
			info->max_slots_used_in_channel - 6) << 8);
		mchbar_write32(0x228 + (channel << 10), info->max_slots_used_in_channel);
		mchbar_write8(0x239 + (channel << 10), 32);
		mchbar_write32(0x248 + (channel << 10), high_multiplier << 24 |
			some_delay_3_half_cycles << 25 | 0x840000);
		mchbar_write32(0x278 + (channel << 10), 0xc362042);
		mchbar_write32(0x27c + (channel << 10), 0x8b000062);
		mchbar_write32(0x24c + (channel << 10),
			(!!info->clock_speed_index) << 17 |
			((2 + info->clock_speed_index -
			(!!info->clock_speed_index))) << 12 | 0x10200);

		mchbar_write8(0x267 + (channel << 10), 4);
		mchbar_write16(0x272 + (channel << 10), 0x155);
		mchbar_clrsetbits32(0x2bc + (channel << 10), 0xffffff, 0x707070);

		write_500(info, channel,
			  ((!info->populated_ranks[channel][1][1])
			   | (!info->populated_ranks[channel][1][0] << 1)
			   | (!info->populated_ranks[channel][0][1] << 2)
			   | (!info->populated_ranks[channel][0][0] << 3)),
			  0x4c9, 4, 1);
	}

	mchbar_write8(0x2c4, (1 + (info->clock_speed_index != 0)) << 6 | 0xc);
	{
		u8 freq_divisor = 2;
		if (info->fsb_frequency == frequency_11(info))
			freq_divisor = 3;
		else if (2 * info->fsb_frequency < 3 * (frequency_11(info) / 2))
			freq_divisor = 1;
		else
			freq_divisor = 2;
		mchbar_write32(0x2c0, freq_divisor << 11 | 0x6009c400);
	}

	if (info->board_lane_delay[3] <= 10) {
		if (info->board_lane_delay[3] <= 8)
			lane_3_delay = info->board_lane_delay[3];
		else
			lane_3_delay = 10;
	} else {
		lane_3_delay = 12;
	}
	cas_latency_derived = info->cas_latency - info->clock_speed_index + 2;
	if (info->clock_speed_index > 1)
		cas_latency_derived++;
	for (channel = 0; channel < NUM_CHANNELS; channel++) {
		mchbar_write32(0x240 + (channel << 10),
			((info->clock_speed_index == 0) * 0x11000) |
			0x1002100 | (2 + info->clock_speed_index) << 4 |
			(info->cas_latency - 3));
		write_500(info, channel, (info->clock_speed_index << 1) | 1,
			  0x609, 6, 1);
		write_500(info, channel,
			  info->clock_speed_index + 2 * info->cas_latency - 7,
			  0x601, 6, 1);

		mchbar_write32(0x250 + (channel << 10),
			(lane_3_delay + info->clock_speed_index + 9) << 6 |
			info->board_lane_delay[7] << 2 |
			info->board_lane_delay[4] << 16 |
			info->board_lane_delay[1] << 25 |
			info->board_lane_delay[1] << 29 | 1);
		mchbar_write32(0x254 + (channel << 10),
			info->board_lane_delay[1] >> 3 |
			(info->board_lane_delay[8] + 4 * info->use_ecc) << 6 |
			0x80 | info->board_lane_delay[6] << 1 |
			info->board_lane_delay[2] << 28 |
			cas_latency_derived << 16 | 0x4700000);
		mchbar_write32(0x258 + (channel << 10),
			(info->board_lane_delay[5] + info->clock_speed_index + 9) << 12 |
			(info->clock_speed_index - info->cas_latency + 12) << 8 |
			info->board_lane_delay[2] << 17 |
			info->board_lane_delay[4] << 24 | 0x47);
		mchbar_write32(0x25c + (channel << 10),
			info->board_lane_delay[1] << 1 |
			info->board_lane_delay[0] << 8 | 0x1da50000);
		mchbar_write8(0x264 + (channel << 10), 0xff);
		mchbar_write8(0x5f8 + (channel << 10), cas_latency_shift << 3 | info->use_ecc);
	}

	program_modules_memory_map(info, 1);

	mchbar_clrsetbits16(0x610, 0xfe3c,
		MIN(ns_to_cycles(info, some_delay_ns) / 2, 127) << 9 | 0x3c);
	mchbar_setbits16(0x612, 1 << 8);
	mchbar_setbits16(0x214, 0x3e00);
	for (i = 0; i < 8; i++) {
		pci_write_config32(QPI_SAD, SAD_DRAM_RULE(i),
			       (info->total_memory_mb - 64) | !i | 2);
		pci_write_config32(QPI_SAD, SAD_INTERLEAVE_LIST(i), 0);
	}
}

#define DEFAULT_PCI_MMIO_SIZE 2048

static void program_total_memory_map(struct raminfo *info)
{
	unsigned int tom, tolud, touud;
	unsigned int quickpath_reserved;
	unsigned int remap_base;
	unsigned int uma_base_igd;
	unsigned int uma_base_gtt;
	unsigned int mmio_size;
	int memory_remap;
	unsigned int memory_map[8];
	int i;
	unsigned int current_limit;
	unsigned int tseg_base;
	int uma_size_igd = 0, uma_size_gtt = 0;

	memset(memory_map, 0, sizeof(memory_map));

	if (info->uma_enabled) {
		u16 t = pci_read_config16(NORTHBRIDGE, GGC);
		gav(t);
		const int uma_sizes_gtt[16] =
		    { 0, 1, 0, 2, 0, 0, 0, 0, 0, 2, 3, 4, 42, 42, 42, 42 };
		/* Igd memory */
		const int uma_sizes_igd[16] = {
			0, 0, 0, 0, 0, 32, 48, 64, 128, 256, 96, 160, 224, 352,
			    256, 512
		};

		uma_size_igd = uma_sizes_igd[(t >> 4) & 0xF];
		uma_size_gtt = uma_sizes_gtt[(t >> 8) & 0xF];
	}

	mmio_size = DEFAULT_PCI_MMIO_SIZE;

	tom = info->total_memory_mb;
	if (tom == 4096)
		tom = 4032;
	touud = ALIGN_DOWN(tom - info->memory_reserved_for_heci_mb, 64);
	tolud = ALIGN_DOWN(MIN(4096 - mmio_size + ALIGN_UP(uma_size_igd + uma_size_gtt, 64)
			      , touud), 64);
	memory_remap = 0;
	if (touud - tolud > 64) {
		memory_remap = 1;
		remap_base = MAX(4096, touud);
		touud = touud - tolud + 4096;
	}
	if (touud > 4096)
		memory_map[2] = touud | 1;
	quickpath_reserved = 0;

	u32 t = pci_read_config32(QPI_SAD, 0x68);

	gav(t);

	if (t & 0x800) {
		u32 shift = t >> 20;
		if (shift == 0)
			die("Quickpath value is 0\n");
		quickpath_reserved = (u32)1 << find_lowest_bit_set32(shift);
	}

	if (memory_remap)
		touud -= quickpath_reserved;

	uma_base_igd = tolud - uma_size_igd;
	uma_base_gtt = uma_base_igd - uma_size_gtt;
	tseg_base = ALIGN_DOWN(uma_base_gtt, 64) - (CONFIG_SMM_TSEG_SIZE >> 20);
	if (!memory_remap)
		tseg_base -= quickpath_reserved;
	tseg_base = ALIGN_DOWN(tseg_base, 8);

	pci_write_config16(NORTHBRIDGE, TOLUD, tolud << 4);
	pci_write_config16(NORTHBRIDGE, TOM, tom >> 6);
	if (memory_remap) {
		pci_write_config16(NORTHBRIDGE, REMAPBASE, remap_base >> 6);
		pci_write_config16(NORTHBRIDGE, REMAPLIMIT, (touud - 64) >> 6);
	}
	pci_write_config16(NORTHBRIDGE, TOUUD, touud);

	if (info->uma_enabled) {
		pci_write_config32(NORTHBRIDGE, IGD_BASE, uma_base_igd << 20);
		pci_write_config32(NORTHBRIDGE, GTT_BASE, uma_base_gtt << 20);
	}
	pci_write_config32(NORTHBRIDGE, TSEG, tseg_base << 20);

	current_limit = 0;
	memory_map[0] = ALIGN_DOWN(uma_base_gtt, 64) | 1;
	memory_map[1] = 4096;
	for (i = 0; i < ARRAY_SIZE(memory_map); i++) {
		current_limit = MAX(current_limit, memory_map[i] & ~1);
		pci_write_config32(QPI_SAD, SAD_DRAM_RULE(i),
			       (memory_map[i] & 1) | ALIGN_DOWN(current_limit -
								1, 64) | 2);
		pci_write_config32(QPI_SAD, SAD_INTERLEAVE_LIST(i), 0);
	}
}

static void collect_system_info(struct raminfo *info)
{
	u32 capid0[3];
	int i;
	unsigned int channel;

	for (i = 0; i < 3; i++) {
		capid0[i] = pci_read_config32(NORTHBRIDGE, CAPID0 | (i << 2));
		printk(BIOS_DEBUG, "CAPID0[%d] = 0x%08x\n", i, capid0[i]);
	}
	info->revision = pci_read_config8(NORTHBRIDGE, PCI_REVISION_ID);
	printk(BIOS_DEBUG, "Revision ID: 0x%x\n", info->revision);
	printk(BIOS_DEBUG, "Device ID: 0x%x\n", pci_read_config16(NORTHBRIDGE, PCI_DEVICE_ID));

	info->max_supported_clock_speed_index = (~capid0[1] & 7);

	if ((capid0[1] >> 11) & 1)
		info->uma_enabled = 0;
	else
		gav(info->uma_enabled =
		    pci_read_config8(NORTHBRIDGE, DEVEN) & 8);
	/* Unrecognised: [0000:fffd3d2d] 37f81.37f82 ! CPUID: eax: 00000001; ecx: 00000e00 => 00020655.00010800.029ae3ff.bfebfbff */
	info->silicon_revision = 0;

	if (capid0[2] & 2) {
		info->silicon_revision = 0;
		info->max_supported_clock_speed_index = 2;
		for (channel = 0; channel < NUM_CHANNELS; channel++)
			if (info->populated_ranks[channel][0][0]
			    && (info->spd[channel][0][MODULE_TYPE] & 0xf) ==
			    3) {
				info->silicon_revision = 2;
				info->max_supported_clock_speed_index = 1;
			}
	} else {
		switch (((capid0[2] >> 18) & 1) + 2 * ((capid0[1] >> 3) & 1)) {
		case 1:
		case 2:
			info->silicon_revision = 3;
			break;
		case 3:
			info->silicon_revision = 0;
			break;
		case 0:
			info->silicon_revision = 2;
			break;
		}
		switch (pci_read_config16(NORTHBRIDGE, PCI_DEVICE_ID)) {
		case 0x40:
			info->silicon_revision = 0;
			break;
		case 0x48:
			info->silicon_revision = 1;
			break;
		}
	}
}

static void write_training_data(struct raminfo *info)
{
	int tm, channel, slot, rank, lane;
	if (info->revision < 8)
		return;

	for (tm = 0; tm < 4; tm++)
		for (channel = 0; channel < NUM_CHANNELS; channel++)
			for (slot = 0; slot < NUM_SLOTS; slot++)
				for (rank = 0; rank < NUM_RANKS; rank++)
					for (lane = 0; lane < 9; lane++)
						write_500(info, channel,
							  info->
							  cached_training->
							  lane_timings[tm]
							  [channel][slot][rank]
							  [lane],
							  get_timing_register_addr
							  (lane, tm, slot,
							   rank), 9, 0);
	write_1d0(info->cached_training->reg_178, 0x178, 7, 1);
	write_1d0(info->cached_training->reg_10b, 0x10b, 6, 1);
}

static void dump_timings(struct raminfo *info)
{
	int channel, slot, rank, lane, i;
	printk(RAM_SPEW, "Timings:\n");
	FOR_POPULATED_RANKS {
		printk(RAM_SPEW, "channel %d, slot %d, rank %d\n", channel,
		       slot, rank);
		for (lane = 0; lane < 9; lane++) {
			printk(RAM_SPEW, "lane %d: ", lane);
			for (i = 0; i < 4; i++) {
				printk(RAM_SPEW, "%x (%x) ",
				       read_500(info, channel,
						get_timing_register_addr
						(lane, i, slot, rank),
						9),
				       info->training.
				       lane_timings[i][channel][slot][rank]
				       [lane]);
			}
			printk(RAM_SPEW, "\n");
		}
	}
	printk(RAM_SPEW, "[178] = %x (%x)\n", read_1d0(0x178, 7),
	       info->training.reg_178);
	printk(RAM_SPEW, "[10b] = %x (%x)\n", read_1d0(0x10b, 6),
	       info->training.reg_10b);
}

/* Read timings and other registers that need to be restored verbatim and
   put them to CBMEM.
 */
static void save_timings(struct raminfo *info)
{
	struct ram_training train;
	int channel, slot, rank, lane, i;

	train = info->training;
	FOR_POPULATED_RANKS for (lane = 0; lane < 9; lane++)
		for (i = 0; i < 4; i++)
			train.lane_timings[i][channel][slot][rank][lane] =
			    read_500(info, channel,
				     get_timing_register_addr(lane, i, slot,
							      rank), 9);
	train.reg_178 = read_1d0(0x178, 7);
	train.reg_10b = read_1d0(0x10b, 6);

	for (channel = 0; channel < NUM_CHANNELS; channel++) {
		u32 reg32;
		reg32 = mchbar_read32((channel << 10) + 0x274);
		train.reg274265[channel][0] = reg32 >> 16;
		train.reg274265[channel][1] = reg32 & 0xffff;
		train.reg274265[channel][2] = mchbar_read16((channel << 10) + 0x265) >> 8;
	}
	train.reg2ca9_bit0 = mchbar_read8(0x2ca9) & 1;
	train.reg_6dc = mchbar_read32(0x6dc);
	train.reg_6e8 = mchbar_read32(0x6e8);

	printk(RAM_SPEW, "[6dc] = %x\n", train.reg_6dc);
	printk(RAM_SPEW, "[6e8] = %x\n", train.reg_6e8);

	/* Save the MRC S3 restore data to cbmem */
	mrc_cache_stash_data(MRC_TRAINING_DATA, MRC_CACHE_VERSION,
			&train, sizeof(train));
}

static const struct ram_training *get_cached_training(void)
{
	return mrc_cache_current_mmap_leak(MRC_TRAINING_DATA,
					   MRC_CACHE_VERSION,
					   NULL);
}

static int have_match_ranks(struct raminfo *info, int channel, int ranks)
{
	int ranks_in_channel;
	ranks_in_channel = info->populated_ranks[channel][0][0]
	    + info->populated_ranks[channel][0][1]
	    + info->populated_ranks[channel][1][0]
	    + info->populated_ranks[channel][1][1];

	/* empty channel */
	if (ranks_in_channel == 0)
		return 1;

	if (ranks_in_channel != ranks)
		return 0;
	/* single slot */
	if (info->populated_ranks[channel][0][0] !=
	    info->populated_ranks[channel][1][0])
		return 1;
	if (info->populated_ranks[channel][0][1] !=
	    info->populated_ranks[channel][1][1])
		return 1;
	if (info->is_x16_module[channel][0] != info->is_x16_module[channel][1])
		return 0;
	if (info->density[channel][0] != info->density[channel][1])
		return 0;
	return 1;
}

static void read_4090(struct raminfo *info)
{
	int i, channel, slot, rank, lane;
	for (i = 0; i < 2; i++)
		for (slot = 0; slot < NUM_SLOTS; slot++)
			for (rank = 0; rank < NUM_RANKS; rank++)
				for (lane = 0; lane < 9; lane++)
					info->training.
					    lane_timings[0][i][slot][rank][lane]
					    = 32;

	for (i = 1; i < 4; i++)
		for (channel = 0; channel < NUM_CHANNELS; channel++)
			for (slot = 0; slot < NUM_SLOTS; slot++)
				for (rank = 0; rank < NUM_RANKS; rank++)
					for (lane = 0; lane < 9; lane++) {
						info->training.
						    lane_timings[i][channel]
						    [slot][rank][lane] =
						    read_500(info, channel,
							     get_timing_register_addr
							     (lane, i, slot,
							      rank), 9)
						    + (i == 1) * 11;	// !!!!
					}

}

static u32 get_etalon2(int flip, u32 addr)
{
	const u16 invmask[] = {
		0xaaaa, 0x6db6, 0x4924, 0xeeee, 0xcccc, 0x8888, 0x7bde, 0x739c,
		0x6318, 0x4210, 0xefbe, 0xcf3c, 0x8e38, 0x0c30, 0x0820
	};
	u32 ret;
	u32 comp4 = addr / 480;
	addr %= 480;
	u32 comp1 = addr & 0xf;
	u32 comp2 = (addr >> 4) & 1;
	u32 comp3 = addr >> 5;

	if (comp4)
		ret = 0x1010101 << (comp4 - 1);
	else
		ret = 0;
	if (flip ^ (((invmask[comp3] >> comp1) ^ comp2) & 1))
		ret = ~ret;

	return ret;
}

static void disable_cache_region(void)
{
	msr_t msr = {.lo = 0, .hi = 0 };

	wrmsr(MTRR_PHYS_BASE(3), msr);
	wrmsr(MTRR_PHYS_MASK(3), msr);
}

static void enable_cache_region(unsigned int base, unsigned int size)
{
	msr_t msr;
	msr.lo = base | MTRR_TYPE_WRPROT;
	msr.hi = 0;
	wrmsr(MTRR_PHYS_BASE(3), msr);
	msr.lo = ((~(ALIGN_DOWN(size + 4096, 4096) - 1) | MTRR_DEF_TYPE_EN)
		  & 0xffffffff);
	msr.hi = 0x0000000f;
	wrmsr(MTRR_PHYS_MASK(3), msr);
}

static void flush_cache(u32 start, u32 size)
{
	u32 end;
	u32 addr;

	end = start + (ALIGN_DOWN(size + 4096, 4096));
	for (addr = start; addr < end; addr += 64)
		clflush((void *)(uintptr_t)addr);
}

static void clear_errors(void)
{
	pci_write_config8(NORTHBRIDGE, 0xc0, 0x01);
}

static void write_testing(struct raminfo *info, int totalrank, int flip)
{
	int nwrites = 0;
	/* in 8-byte units.  */
	u32 offset;
	u8 *base;

	base = (u8 *)(uintptr_t)(totalrank << 28);
	for (offset = 0; offset < 9 * 480; offset += 2) {
		write32(base + offset * 8, get_etalon2(flip, offset));
		write32(base + offset * 8 + 4, get_etalon2(flip, offset));
		write32(base + offset * 8 + 8, get_etalon2(flip, offset + 1));
		write32(base + offset * 8 + 12, get_etalon2(flip, offset + 1));
		nwrites += 4;
		if (nwrites >= 320) {
			clear_errors();
			nwrites = 0;
		}
	}
}

static u8 check_testing(struct raminfo *info, u8 total_rank, int flip)
{
	u8 failmask = 0;
	int i;
	int comp1, comp2, comp3;
	u32 failxor[2] = { 0, 0 };

	enable_cache_region((total_rank << 28), 1728 * 5 * 4);

	for (comp3 = 0; comp3 < 9 && failmask != 0xff; comp3++) {
		for (comp1 = 0; comp1 < 4; comp1++)
			for (comp2 = 0; comp2 < 60; comp2++) {
				u32 re[4];
				u32 curroffset =
				    comp3 * 8 * 60 + 2 * comp1 + 8 * comp2;
				read128((total_rank << 28) | (curroffset << 3),
					(u64 *)re);
				failxor[0] |=
				    get_etalon2(flip, curroffset) ^ re[0];
				failxor[1] |=
				    get_etalon2(flip, curroffset) ^ re[1];
				failxor[0] |=
				    get_etalon2(flip, curroffset | 1) ^ re[2];
				failxor[1] |=
				    get_etalon2(flip, curroffset | 1) ^ re[3];
			}
		for (i = 0; i < 8; i++)
			if ((0xff << (8 * (i % 4))) & failxor[i / 4])
				failmask |= 1 << i;
	}
	disable_cache_region();
	flush_cache((total_rank << 28), 1728 * 5 * 4);
	return failmask;
}

const u32 seed1[0x18] = {
	0x3a9d5ab5, 0x576cb65b, 0x555773b6, 0x2ab772ee,
	0x555556ee, 0x3a9d5ab5, 0x576cb65b, 0x555773b6,
	0x2ab772ee, 0x555556ee, 0x5155a555, 0x5155a555,
	0x5155a555, 0x5155a555, 0x3a9d5ab5, 0x576cb65b,
	0x555773b6, 0x2ab772ee, 0x555556ee, 0x55d6b4a5,
	0x366d6b3a, 0x2ae5ddbb, 0x3b9ddbb7, 0x55d6b4a5,
};

static u32 get_seed2(int a, int b)
{
	const u32 seed2[5] = {
		0x55555555, 0x33333333, 0x2e555a55, 0x55555555,
		0x5b6db6db,
	};
	u32 r;
	r = seed2[(a + (a >= 10)) / 5];
	return b ? ~r : r;
}

static int make_shift(int comp2, int comp5, int x)
{
	const u8 seed3[32] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x38, 0x1c, 0x3c, 0x18, 0x38, 0x38,
		0x38, 0x38, 0x38, 0x38, 0x0f, 0x0f, 0x0f, 0x0f,
		0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
	};

	return (comp2 - ((seed3[comp5] >> (x & 7)) & 1)) & 0x1f;
}

static u32 get_etalon(int flip, u32 addr)
{
	u32 mask_byte = 0;
	int comp1 = (addr >> 1) & 1;
	int comp2 = (addr >> 3) & 0x1f;
	int comp3 = (addr >> 8) & 0xf;
	int comp4 = (addr >> 12) & 0xf;
	int comp5 = (addr >> 16) & 0x1f;
	u32 mask_bit = ~(0x10001 << comp3);
	u32 part1;
	u32 part2;
	int byte;

	part2 =
	    ((seed1[comp5] >>
	      make_shift(comp2, comp5,
			 (comp3 >> 3) | (comp1 << 2) | 2)) & 1) ^ flip;
	part1 =
	    ((seed1[comp5] >>
	      make_shift(comp2, comp5,
			 (comp3 >> 3) | (comp1 << 2) | 0)) & 1) ^ flip;

	for (byte = 0; byte < 4; byte++)
		if ((get_seed2(comp5, comp4) >>
		     make_shift(comp2, comp5, (byte | (comp1 << 2)))) & 1)
			mask_byte |= 0xff << (8 * byte);

	return (mask_bit & mask_byte) | (part1 << comp3) | (part2 <<
							    (comp3 + 16));
}

static void
write_testing_type2(struct raminfo *info, u8 totalrank, u8 region, u8 block,
		    char flip)
{
	int i;
	for (i = 0; i < 2048; i++)
		write32p((totalrank << 28) | (region << 25) | (block << 16) |
			 (i << 2), get_etalon(flip, (block << 16) | (i << 2)));
}

static u8
check_testing_type2(struct raminfo *info, u8 totalrank, u8 region, u8 block,
		    char flip)
{
	u8 failmask = 0;
	u32 failxor[2];
	int i;
	int comp1, comp2, comp3;

	failxor[0] = 0;
	failxor[1] = 0;

	enable_cache_region(totalrank << 28, 134217728);
	for (comp3 = 0; comp3 < 2 && failmask != 0xff; comp3++) {
		for (comp1 = 0; comp1 < 16; comp1++)
			for (comp2 = 0; comp2 < 64; comp2++) {
				u32 addr =
				    (totalrank << 28) | (region << 25) | (block
									  << 16)
				    | (comp3 << 12) | (comp2 << 6) | (comp1 <<
								      2);
				failxor[comp1 & 1] |=
				    read32p(addr) ^ get_etalon(flip, addr);
			}
		for (i = 0; i < 8; i++)
			if ((0xff << (8 * (i % 4))) & failxor[i / 4])
				failmask |= 1 << i;
	}
	disable_cache_region();
	flush_cache((totalrank << 28) | (region << 25) | (block << 16), 16384);
	return failmask;
}

static int check_bounded(unsigned short *vals, u16 bound)
{
	int i;

	for (i = 0; i < 8; i++)
		if (vals[i] < bound)
			return 0;
	return 1;
}

enum state {
	BEFORE_USABLE = 0, AT_USABLE = 1, AT_MARGIN = 2, COMPLETE = 3
};

static int validate_state(enum state *in)
{
	int i;
	for (i = 0; i < 8; i++)
		if (in[i] != COMPLETE)
			return 0;
	return 1;
}

static void
do_fsm(enum state *state, u16 *counter,
	u8 fail_mask, int margin, int uplimit,
	u8 *res_low, u8 *res_high, u8 val)
{
	int lane;

	for (lane = 0; lane < 8; lane++) {
		int is_fail = (fail_mask >> lane) & 1;
		switch (state[lane]) {
		case BEFORE_USABLE:
			if (!is_fail) {
				counter[lane] = 1;
				state[lane] = AT_USABLE;
				break;
			}
			counter[lane] = 0;
			state[lane] = BEFORE_USABLE;
			break;
		case AT_USABLE:
			if (!is_fail) {
				++counter[lane];
				if (counter[lane] >= margin) {
					state[lane] = AT_MARGIN;
					res_low[lane] = val - margin + 1;
					break;
				}
				state[lane] = 1;
				break;
			}
			counter[lane] = 0;
			state[lane] = BEFORE_USABLE;
			break;
		case AT_MARGIN:
			if (is_fail) {
				state[lane] = COMPLETE;
				res_high[lane] = val - 1;
			} else {
				counter[lane]++;
				state[lane] = AT_MARGIN;
				if (val == uplimit) {
					state[lane] = COMPLETE;
					res_high[lane] = uplimit;
				}
			}
			break;
		case COMPLETE:
			break;
		}
	}
}

static void
train_ram_at_178(struct raminfo *info, u8 channel, int slot, int rank,
		 u8 total_rank, u8 reg_178, int first_run, int niter,
		 timing_bounds_t * timings)
{
	int lane;
	enum state state[8];
	u16 count[8];
	u8 lower_usable[8];
	u8 upper_usable[8];
	unsigned short num_successfully_checked[8];
	u8 reg1b3;
	int i;

	for (i = 0; i < 8; i++)
		state[i] = BEFORE_USABLE;

	if (!first_run) {
		int is_all_ok = 1;
		for (lane = 0; lane < 8; lane++)
			if (timings[reg_178][channel][slot][rank][lane].
			    smallest ==
			    timings[reg_178][channel][slot][rank][lane].
			    largest) {
				timings[reg_178][channel][slot][rank][lane].
				    smallest = 0;
				timings[reg_178][channel][slot][rank][lane].
				    largest = 0;
				is_all_ok = 0;
			}
		if (is_all_ok) {
			for (i = 0; i < 8; i++)
				state[i] = COMPLETE;
		}
	}

	for (reg1b3 = 0; reg1b3 < 0x30 && !validate_state(state); reg1b3++) {
		u8 failmask = 0;
		write_1d0(reg1b3 ^ 32, 0x1b3, 6, 1);
		write_1d0(reg1b3 ^ 32, 0x1a3, 6, 1);
		failmask = check_testing(info, total_rank, 0);
		mchbar_setbits32(0xfb0, 3 << 16);
		do_fsm(state, count, failmask, 5, 47, lower_usable,
		       upper_usable, reg1b3);
	}

	if (reg1b3) {
		write_1d0(0, 0x1b3, 6, 1);
		write_1d0(0, 0x1a3, 6, 1);
		for (lane = 0; lane < 8; lane++) {
			if (state[lane] == COMPLETE) {
				timings[reg_178][channel][slot][rank][lane].
				    smallest =
				    lower_usable[lane] +
				    (info->training.
				     lane_timings[0][channel][slot][rank][lane]
				     & 0x3F) - 32;
				timings[reg_178][channel][slot][rank][lane].
				    largest =
				    upper_usable[lane] +
				    (info->training.
				     lane_timings[0][channel][slot][rank][lane]
				     & 0x3F) - 32;
			}
		}
	}

	if (!first_run) {
		for (lane = 0; lane < 8; lane++)
			if (state[lane] == COMPLETE) {
				write_500(info, channel,
					  timings[reg_178][channel][slot][rank]
					  [lane].smallest,
					  get_timing_register_addr(lane, 0,
								   slot, rank),
					  9, 1);
				write_500(info, channel,
					  timings[reg_178][channel][slot][rank]
					  [lane].smallest +
					  info->training.
					  lane_timings[1][channel][slot][rank]
					  [lane]
					  -
					  info->training.
					  lane_timings[0][channel][slot][rank]
					  [lane], get_timing_register_addr(lane,
									   1,
									   slot,
									   rank),
					  9, 1);
				num_successfully_checked[lane] = 0;
			} else
				num_successfully_checked[lane] = -1;

		do {
			u8 failmask = 0;
			for (i = 0; i < niter; i++) {
				if (failmask == 0xFF)
					break;
				failmask |=
				    check_testing_type2(info, total_rank, 2, i,
							0);
				failmask |=
				    check_testing_type2(info, total_rank, 3, i,
							1);
			}
			mchbar_setbits32(0xfb0, 3 << 16);
			for (lane = 0; lane < 8; lane++)
				if (num_successfully_checked[lane] != 0xffff) {
					if ((1 << lane) & failmask) {
						if (timings[reg_178][channel]
						    [slot][rank][lane].
						    largest <=
						    timings[reg_178][channel]
						    [slot][rank][lane].smallest)
							num_successfully_checked
							    [lane] = -1;
						else {
							num_successfully_checked
							    [lane] = 0;
							timings[reg_178]
							    [channel][slot]
							    [rank][lane].
							    smallest++;
							write_500(info, channel,
								  timings
								  [reg_178]
								  [channel]
								  [slot][rank]
								  [lane].
								  smallest,
								  get_timing_register_addr
								  (lane, 0,
								   slot, rank),
								  9, 1);
							write_500(info, channel,
								  timings
								  [reg_178]
								  [channel]
								  [slot][rank]
								  [lane].
								  smallest +
								  info->
								  training.
								  lane_timings
								  [1][channel]
								  [slot][rank]
								  [lane]
								  -
								  info->
								  training.
								  lane_timings
								  [0][channel]
								  [slot][rank]
								  [lane],
								  get_timing_register_addr
								  (lane, 1,
								   slot, rank),
								  9, 1);
						}
					} else
						num_successfully_checked[lane]
							++;
				}
		}
		while (!check_bounded(num_successfully_checked, 2))
			;

		for (lane = 0; lane < 8; lane++)
			if (state[lane] == COMPLETE) {
				write_500(info, channel,
					  timings[reg_178][channel][slot][rank]
					  [lane].largest,
					  get_timing_register_addr(lane, 0,
								   slot, rank),
					  9, 1);
				write_500(info, channel,
					  timings[reg_178][channel][slot][rank]
					  [lane].largest +
					  info->training.
					  lane_timings[1][channel][slot][rank]
					  [lane]
					  -
					  info->training.
					  lane_timings[0][channel][slot][rank]
					  [lane], get_timing_register_addr(lane,
									   1,
									   slot,
									   rank),
					  9, 1);
				num_successfully_checked[lane] = 0;
			} else
				num_successfully_checked[lane] = -1;

		do {
			int failmask = 0;
			for (i = 0; i < niter; i++) {
				if (failmask == 0xFF)
					break;
				failmask |=
				    check_testing_type2(info, total_rank, 2, i,
							0);
				failmask |=
				    check_testing_type2(info, total_rank, 3, i,
							1);
			}

			mchbar_setbits32(0xfb0, 3 << 16);
			for (lane = 0; lane < 8; lane++) {
				if (num_successfully_checked[lane] != 0xffff) {
					if ((1 << lane) & failmask) {
						if (timings[reg_178][channel]
						    [slot][rank][lane].
						    largest <=
						    timings[reg_178][channel]
						    [slot][rank][lane].
						    smallest) {
							num_successfully_checked
							    [lane] = -1;
						} else {
							num_successfully_checked
							    [lane] = 0;
							timings[reg_178]
							    [channel][slot]
							    [rank][lane].
							    largest--;
							write_500(info, channel,
								  timings
								  [reg_178]
								  [channel]
								  [slot][rank]
								  [lane].
								  largest,
								  get_timing_register_addr
								  (lane, 0,
								   slot, rank),
								  9, 1);
							write_500(info, channel,
								  timings
								  [reg_178]
								  [channel]
								  [slot][rank]
								  [lane].
								  largest +
								  info->
								  training.
								  lane_timings
								  [1][channel]
								  [slot][rank]
								  [lane]
								  -
								  info->
								  training.
								  lane_timings
								  [0][channel]
								  [slot][rank]
								  [lane],
								  get_timing_register_addr
								  (lane, 1,
								   slot, rank),
								  9, 1);
						}
					} else
						num_successfully_checked[lane]
							++;
				}
			}
		}
		while (!check_bounded(num_successfully_checked, 3))
			;

		for (lane = 0; lane < 8; lane++) {
			write_500(info, channel,
				  info->training.
				  lane_timings[0][channel][slot][rank][lane],
				  get_timing_register_addr(lane, 0, slot, rank),
				  9, 1);
			write_500(info, channel,
				  info->training.
				  lane_timings[1][channel][slot][rank][lane],
				  get_timing_register_addr(lane, 1, slot, rank),
				  9, 1);
			if (timings[reg_178][channel][slot][rank][lane].
			    largest <=
			    timings[reg_178][channel][slot][rank][lane].
			    smallest) {
				timings[reg_178][channel][slot][rank][lane].
				    largest = 0;
				timings[reg_178][channel][slot][rank][lane].
				    smallest = 0;
			}
		}
	}
}

static void set_10b(struct raminfo *info, u8 val)
{
	int channel;
	int slot, rank;
	int lane;

	if (read_1d0(0x10b, 6) == val)
		return;

	write_1d0(val, 0x10b, 6, 1);

	FOR_POPULATED_RANKS_BACKWARDS for (lane = 0; lane < 9; lane++) {
		u16 reg_500;
		reg_500 = read_500(info, channel,
				   get_timing_register_addr(lane, 0, slot,
							    rank), 9);
		if (val == 1) {
			if (lut16[info->clock_speed_index] <= reg_500)
				reg_500 -= lut16[info->clock_speed_index];
			else
				reg_500 = 0;
		} else {
			reg_500 += lut16[info->clock_speed_index];
		}
		write_500(info, channel, reg_500,
			  get_timing_register_addr(lane, 0, slot, rank), 9, 1);
	}
}

static void set_ecc(int onoff)
{
	int channel;
	for (channel = 0; channel < NUM_CHANNELS; channel++) {
		u8 t;
		t = mchbar_read8((channel << 10) + 0x5f8);
		if (onoff)
			t |= 1;
		else
			t &= ~1;
		mchbar_write8((channel << 10) + 0x5f8, t);
	}
}

static void set_178(u8 val)
{
	if (val >= 31)
		val = val - 31;
	else
		val = 63 - val;

	write_1d0(2 * val, 0x178, 7, 1);
}

static void
write_500_timings_type(struct raminfo *info, int channel, int slot, int rank,
		       int type)
{
	int lane;

	for (lane = 0; lane < 8; lane++)
		write_500(info, channel,
			  info->training.
			  lane_timings[type][channel][slot][rank][lane],
			  get_timing_register_addr(lane, type, slot, rank), 9,
			  0);
}

static void
try_timing_offsets(struct raminfo *info, int channel,
		   int slot, int rank, int totalrank)
{
	u16 count[8];
	enum state state[8];
	u8 lower_usable[8], upper_usable[8];
	int lane;
	int i;
	int flip = 1;
	int timing_offset;

	for (i = 0; i < 8; i++)
		state[i] = BEFORE_USABLE;

	memset(count, 0, sizeof(count));

	for (lane = 0; lane < 8; lane++)
		write_500(info, channel,
			  info->training.
			  lane_timings[2][channel][slot][rank][lane] + 32,
			  get_timing_register_addr(lane, 3, slot, rank), 9, 1);

	for (timing_offset = 0; !validate_state(state) && timing_offset < 64;
	     timing_offset++) {
		u8 failmask;
		write_1d0(timing_offset ^ 32, 0x1bb, 6, 1);
		failmask = 0;
		for (i = 0; i < 2 && failmask != 0xff; i++) {
			flip = !flip;
			write_testing(info, totalrank, flip);
			failmask |= check_testing(info, totalrank, flip);
		}
		do_fsm(state, count, failmask, 10, 63, lower_usable,
		       upper_usable, timing_offset);
	}
	write_1d0(0, 0x1bb, 6, 1);
	dump_timings(info);
	if (!validate_state(state))
		die("Couldn't discover DRAM timings (1)\n");

	for (lane = 0; lane < 8; lane++) {
		u8 bias = 0;

		if (info->silicon_revision) {
			int usable_length;

			usable_length = upper_usable[lane] - lower_usable[lane];
			if (usable_length >= 20) {
				bias = usable_length / 2 - 10;
				if (bias >= 2)
					bias = 2;
			}
		}
		write_500(info, channel,
			  info->training.
			  lane_timings[2][channel][slot][rank][lane] +
			  (upper_usable[lane] + lower_usable[lane]) / 2 - bias,
			  get_timing_register_addr(lane, 3, slot, rank), 9, 1);
		info->training.timing2_bounds[channel][slot][rank][lane][0] =
		    info->training.lane_timings[2][channel][slot][rank][lane] +
		    lower_usable[lane];
		info->training.timing2_bounds[channel][slot][rank][lane][1] =
		    info->training.lane_timings[2][channel][slot][rank][lane] +
		    upper_usable[lane];
		info->training.timing2_offset[channel][slot][rank][lane] =
		    info->training.lane_timings[2][channel][slot][rank][lane];
	}
}

static u8
choose_training(struct raminfo *info, int channel, int slot, int rank,
		int lane, timing_bounds_t * timings, u8 center_178)
{
	u16 central_weight;
	u16 side_weight;
	unsigned int sum = 0, count = 0;
	u8 span;
	u8 lower_margin, upper_margin;
	u8 reg_178;
	u8 result;

	span = 12;
	central_weight = 20;
	side_weight = 20;
	if (info->silicon_revision == 1 && channel == 1) {
		central_weight = 5;
		side_weight = 20;
		if ((info->
		     populated_ranks_mask[1] ^ (info->
						populated_ranks_mask[1] >> 2)) &
		    1)
			span = 18;
	}
	if ((info->populated_ranks_mask[0] & 5) == 5) {
		central_weight = 20;
		side_weight = 20;
	}
	if (info->clock_speed_index >= 2
	    && (info->populated_ranks_mask[0] & 5) == 5 && slot == 1) {
		if (info->silicon_revision == 1) {
			switch (channel) {
			case 0:
				if (lane == 1) {
					central_weight = 10;
					side_weight = 20;
				}
				break;
			case 1:
				if (lane == 6) {
					side_weight = 5;
					central_weight = 20;
				}
				break;
			}
		}
		if (info->silicon_revision == 0 && channel == 0 && lane == 0) {
			side_weight = 5;
			central_weight = 20;
		}
	}
	for (reg_178 = center_178 - span; reg_178 <= center_178 + span;
	     reg_178 += span) {
		u8 smallest;
		u8 largest;
		largest = timings[reg_178][channel][slot][rank][lane].largest;
		smallest = timings[reg_178][channel][slot][rank][lane].smallest;
		if (largest - smallest + 1 >= 5) {
			unsigned int weight;
			if (reg_178 == center_178)
				weight = central_weight;
			else
				weight = side_weight;
			sum += weight * (largest + smallest);
			count += weight;
		}
	}
	dump_timings(info);
	if (count == 0)
		die("Couldn't discover DRAM timings (2)\n");
	result = sum / (2 * count);
	lower_margin =
	    result - timings[center_178][channel][slot][rank][lane].smallest;
	upper_margin =
	    timings[center_178][channel][slot][rank][lane].largest - result;
	if (upper_margin < 10 && lower_margin > 10)
		result -= MIN(lower_margin - 10, 10 - upper_margin);
	if (upper_margin > 10 && lower_margin < 10)
		result += MIN(upper_margin - 10, 10 - lower_margin);
	return result;
}

#define STANDARD_MIN_MARGIN 5

static u8 choose_reg178(struct raminfo *info, timing_bounds_t * timings)
{
	u16 margin[64];
	int lane, rank, slot, channel;
	u8 reg178;
	int count = 0, sum = 0;

	for (reg178 = reg178_min[info->clock_speed_index];
	     reg178 < reg178_max[info->clock_speed_index];
	     reg178 += reg178_step[info->clock_speed_index]) {
		margin[reg178] = -1;
		FOR_POPULATED_RANKS_BACKWARDS for (lane = 0; lane < 8; lane++) {
			int curmargin =
			    timings[reg178][channel][slot][rank][lane].largest -
			    timings[reg178][channel][slot][rank][lane].
			    smallest + 1;
			if (curmargin < margin[reg178])
				margin[reg178] = curmargin;
		}
		if (margin[reg178] >= STANDARD_MIN_MARGIN) {
			u16 weight;
			weight = margin[reg178] - STANDARD_MIN_MARGIN;
			sum += weight * reg178;
			count += weight;
		}
	}
	dump_timings(info);
	if (count == 0)
		die("Couldn't discover DRAM timings (3)\n");

	u8 threshold;

	for (threshold = 30; threshold >= 5; threshold--) {
		int usable_length = 0;
		int smallest_fount = 0;
		for (reg178 = reg178_min[info->clock_speed_index];
		     reg178 < reg178_max[info->clock_speed_index];
		     reg178 += reg178_step[info->clock_speed_index])
			if (margin[reg178] >= threshold) {
				usable_length +=
				    reg178_step[info->clock_speed_index];
				info->training.reg178_largest =
				    reg178 -
				    2 * reg178_step[info->clock_speed_index];

				if (!smallest_fount) {
					smallest_fount = 1;
					info->training.reg178_smallest =
					    reg178 +
					    reg178_step[info->
							clock_speed_index];
				}
			}
		if (usable_length >= 0x21)
			break;
	}

	return sum / count;
}

static int check_cached_sanity(struct raminfo *info)
{
	int lane;
	int slot, rank;
	int channel;

	if (!info->cached_training)
		return 0;

	for (channel = 0; channel < NUM_CHANNELS; channel++)
		for (slot = 0; slot < NUM_SLOTS; slot++)
			for (rank = 0; rank < NUM_RANKS; rank++)
				for (lane = 0; lane < 8 + info->use_ecc; lane++) {
					u16 cached_value, estimation_value;
					cached_value =
					    info->cached_training->
					    lane_timings[1][channel][slot][rank]
					    [lane];
					if (cached_value >= 0x18
					    && cached_value <= 0x1E7) {
						estimation_value =
						    info->training.
						    lane_timings[1][channel]
						    [slot][rank][lane];
						if (estimation_value <
						    cached_value - 24)
							return 0;
						if (estimation_value >
						    cached_value + 24)
							return 0;
					}
				}
	return 1;
}

static int try_cached_training(struct raminfo *info)
{
	u8 saved_243[2];
	u8 tm;

	int channel, slot, rank, lane;
	int flip = 1;
	int i, j;

	if (!check_cached_sanity(info))
		return 0;

	info->training.reg178_center = info->cached_training->reg178_center;
	info->training.reg178_smallest = info->cached_training->reg178_smallest;
	info->training.reg178_largest = info->cached_training->reg178_largest;
	memcpy(&info->training.timing_bounds,
	       &info->cached_training->timing_bounds,
	       sizeof(info->training.timing_bounds));
	memcpy(&info->training.timing_offset,
	       &info->cached_training->timing_offset,
	       sizeof(info->training.timing_offset));

	write_1d0(2, 0x142, 3, 1);
	saved_243[0] = mchbar_read8(0x243);
	saved_243[1] = mchbar_read8(0x643);
	mchbar_write8(0x243, saved_243[0] | 2);
	mchbar_write8(0x643, saved_243[1] | 2);
	set_ecc(0);
	pci_write_config16(NORTHBRIDGE, 0xc8, 3);
	if (read_1d0(0x10b, 6) & 1)
		set_10b(info, 0);
	for (tm = 0; tm < 2; tm++) {
		int totalrank;

		set_178(tm ? info->cached_training->reg178_largest : info->
			cached_training->reg178_smallest);

		totalrank = 0;
		/* Check timing ranges. With i == 0 we check smallest one and with
		   i == 1 the largest bound. With j == 0 we check that on the bound
		   it still works whereas with j == 1 we check that just outside of
		   bound we fail.
		 */
		FOR_POPULATED_RANKS_BACKWARDS {
			for (i = 0; i < 2; i++) {
				for (lane = 0; lane < 8; lane++) {
					write_500(info, channel,
						  info->cached_training->
						  timing2_bounds[channel][slot]
						  [rank][lane][i],
						  get_timing_register_addr(lane,
									   3,
									   slot,
									   rank),
						  9, 1);

					if (!i)
						write_500(info, channel,
							  info->
							  cached_training->
							  timing2_offset
							  [channel][slot][rank]
							  [lane],
							  get_timing_register_addr
							  (lane, 2, slot, rank),
							  9, 1);
					write_500(info, channel,
						  i ? info->cached_training->
						  timing_bounds[tm][channel]
						  [slot][rank][lane].
						  largest : info->
						  cached_training->
						  timing_bounds[tm][channel]
						  [slot][rank][lane].smallest,
						  get_timing_register_addr(lane,
									   0,
									   slot,
									   rank),
						  9, 1);
					write_500(info, channel,
						  info->cached_training->
						  timing_offset[channel][slot]
						  [rank][lane] +
						  (i ? info->cached_training->
						   timing_bounds[tm][channel]
						   [slot][rank][lane].
						   largest : info->
						   cached_training->
						   timing_bounds[tm][channel]
						   [slot][rank][lane].
						   smallest) - 64,
						  get_timing_register_addr(lane,
									   1,
									   slot,
									   rank),
						  9, 1);
				}
				for (j = 0; j < 2; j++) {
					u8 failmask;
					u8 expected_failmask;
					char reg1b3;

					reg1b3 = (j == 1) + 4;
					reg1b3 =
					    j == i ? reg1b3 : (-reg1b3) & 0x3f;
					write_1d0(reg1b3, 0x1bb, 6, 1);
					write_1d0(reg1b3, 0x1b3, 6, 1);
					write_1d0(reg1b3, 0x1a3, 6, 1);

					flip = !flip;
					write_testing(info, totalrank, flip);
					failmask =
					    check_testing(info, totalrank,
							  flip);
					expected_failmask =
					    j == 0 ? 0x00 : 0xff;
					if (failmask != expected_failmask)
						goto fail;
				}
			}
			totalrank++;
		}
	}

	set_178(info->cached_training->reg178_center);
	if (info->use_ecc)
		set_ecc(1);
	write_training_data(info);
	write_1d0(0, 322, 3, 1);
	info->training = *info->cached_training;

	write_1d0(0, 0x1bb, 6, 1);
	write_1d0(0, 0x1b3, 6, 1);
	write_1d0(0, 0x1a3, 6, 1);
	mchbar_write8(0x243, saved_243[0]);
	mchbar_write8(0x643, saved_243[1]);

	return 1;

fail:
	FOR_POPULATED_RANKS {
		write_500_timings_type(info, channel, slot, rank, 1);
		write_500_timings_type(info, channel, slot, rank, 2);
		write_500_timings_type(info, channel, slot, rank, 3);
	}

	write_1d0(0, 0x1bb, 6, 1);
	write_1d0(0, 0x1b3, 6, 1);
	write_1d0(0, 0x1a3, 6, 1);
	mchbar_write8(0x243, saved_243[0]);
	mchbar_write8(0x643, saved_243[1]);

	return 0;
}

static void do_ram_training(struct raminfo *info)
{
	u8 saved_243[2];
	int totalrank = 0;
	u8 reg_178;
	int niter;

	timing_bounds_t *timings = timings_car;
	int lane, rank, slot, channel;
	u8 reg178_center;

	write_1d0(2, 0x142, 3, 1);
	saved_243[0] = mchbar_read8(0x243);
	saved_243[1] = mchbar_read8(0x643);
	mchbar_write8(0x243, saved_243[0] | 2);
	mchbar_write8(0x643, saved_243[1] | 2);
	switch (info->clock_speed_index) {
	case 0:
		niter = 5;
		break;
	case 1:
		niter = 10;
		break;
	default:
		niter = 19;
		break;
	}
	set_ecc(0);

	FOR_POPULATED_RANKS_BACKWARDS {
		int i;

		write_500_timings_type(info, channel, slot, rank, 0);

		write_testing(info, totalrank, 0);
		for (i = 0; i < niter; i++) {
			write_testing_type2(info, totalrank, 2, i, 0);
			write_testing_type2(info, totalrank, 3, i, 1);
		}
		pci_write_config8(NORTHBRIDGE, 0xc0, 0x01);
		totalrank++;
	}

	if (reg178_min[info->clock_speed_index] <
	    reg178_max[info->clock_speed_index])
		memset(timings[reg178_min[info->clock_speed_index]], 0,
		       sizeof(timings[0]) *
		       (reg178_max[info->clock_speed_index] -
			reg178_min[info->clock_speed_index]));
	for (reg_178 = reg178_min[info->clock_speed_index];
	     reg_178 < reg178_max[info->clock_speed_index];
	     reg_178 += reg178_step[info->clock_speed_index]) {
		totalrank = 0;
		set_178(reg_178);
		for (channel = NUM_CHANNELS - 1; channel >= 0; channel--)
			for (slot = 0; slot < NUM_SLOTS; slot++)
				for (rank = 0; rank < NUM_RANKS; rank++) {
					memset(&timings[reg_178][channel][slot]
					       [rank][0].smallest, 0, 16);
					if (info->
					    populated_ranks[channel][slot]
					    [rank]) {
						train_ram_at_178(info, channel,
								 slot, rank,
								 totalrank,
								 reg_178, 1,
								 niter,
								 timings);
						totalrank++;
					}
				}
	}

	reg178_center = choose_reg178(info, timings);

	FOR_POPULATED_RANKS_BACKWARDS for (lane = 0; lane < 8; lane++) {
		info->training.timing_bounds[0][channel][slot][rank][lane].
		    smallest =
		    timings[info->training.
			    reg178_smallest][channel][slot][rank][lane].
		    smallest;
		info->training.timing_bounds[0][channel][slot][rank][lane].
		    largest =
		    timings[info->training.
			    reg178_smallest][channel][slot][rank][lane].largest;
		info->training.timing_bounds[1][channel][slot][rank][lane].
		    smallest =
		    timings[info->training.
			    reg178_largest][channel][slot][rank][lane].smallest;
		info->training.timing_bounds[1][channel][slot][rank][lane].
		    largest =
		    timings[info->training.
			    reg178_largest][channel][slot][rank][lane].largest;
		info->training.timing_offset[channel][slot][rank][lane] =
		    info->training.lane_timings[1][channel][slot][rank][lane]
		    -
		    info->training.lane_timings[0][channel][slot][rank][lane] +
		    64;
	}

	if (info->silicon_revision == 1
	    && (info->
		populated_ranks_mask[1] ^ (info->
					   populated_ranks_mask[1] >> 2)) & 1) {
		int ranks_after_channel1;

		totalrank = 0;
		for (reg_178 = reg178_center - 18;
		     reg_178 <= reg178_center + 18; reg_178 += 18) {
			totalrank = 0;
			set_178(reg_178);
			for (slot = 0; slot < NUM_SLOTS; slot++)
				for (rank = 0; rank < NUM_RANKS; rank++) {
					if (info->
					    populated_ranks[1][slot][rank]) {
						train_ram_at_178(info, 1, slot,
								 rank,
								 totalrank,
								 reg_178, 0,
								 niter,
								 timings);
						totalrank++;
					}
				}
		}
		ranks_after_channel1 = totalrank;

		for (reg_178 = reg178_center - 12;
		     reg_178 <= reg178_center + 12; reg_178 += 12) {
			totalrank = ranks_after_channel1;
			set_178(reg_178);
			for (slot = 0; slot < NUM_SLOTS; slot++)
				for (rank = 0; rank < NUM_RANKS; rank++)
					if (info->
					    populated_ranks[0][slot][rank]) {
						train_ram_at_178(info, 0, slot,
								 rank,
								 totalrank,
								 reg_178, 0,
								 niter,
								 timings);
						totalrank++;
					}

		}
	} else {
		for (reg_178 = reg178_center - 12;
		     reg_178 <= reg178_center + 12; reg_178 += 12) {
			totalrank = 0;
			set_178(reg_178);
			FOR_POPULATED_RANKS_BACKWARDS {
				train_ram_at_178(info, channel, slot, rank,
						 totalrank, reg_178, 0, niter,
						 timings);
				totalrank++;
			}
		}
	}

	set_178(reg178_center);
	FOR_POPULATED_RANKS_BACKWARDS for (lane = 0; lane < 8; lane++) {
		u16 tm0;

		tm0 =
		    choose_training(info, channel, slot, rank, lane, timings,
				    reg178_center);
		write_500(info, channel, tm0,
			  get_timing_register_addr(lane, 0, slot, rank), 9, 1);
		write_500(info, channel,
			  tm0 +
			  info->training.
			  lane_timings[1][channel][slot][rank][lane] -
			  info->training.
			  lane_timings[0][channel][slot][rank][lane],
			  get_timing_register_addr(lane, 1, slot, rank), 9, 1);
	}

	totalrank = 0;
	FOR_POPULATED_RANKS_BACKWARDS {
		try_timing_offsets(info, channel, slot, rank, totalrank);
		totalrank++;
	}
	mchbar_write8(0x243, saved_243[0]);
	mchbar_write8(0x643, saved_243[1]);
	write_1d0(0, 0x142, 3, 1);
	info->training.reg178_center = reg178_center;
}

static void ram_training(struct raminfo *info)
{
	u16 saved_fc4;

	saved_fc4 = mchbar_read16(0xfc4);
	mchbar_write16(0xfc4, 0xffff);

	if (info->revision >= 8)
		read_4090(info);

	if (!try_cached_training(info))
		do_ram_training(info);
	if ((info->silicon_revision == 2 || info->silicon_revision == 3)
	    && info->clock_speed_index < 2)
		set_10b(info, 1);
	mchbar_write16(0xfc4, saved_fc4);
}

u16 get_max_timing(struct raminfo *info, int channel)
{
	int slot, rank, lane;
	u16 ret = 0;

	if ((mchbar_read8(0x2ca8) >> 2) < 1)
		return 384;

	if (info->revision < 8)
		return 256;

	for (slot = 0; slot < NUM_SLOTS; slot++)
		for (rank = 0; rank < NUM_RANKS; rank++)
			if (info->populated_ranks[channel][slot][rank])
				for (lane = 0; lane < 8 + info->use_ecc; lane++)
					ret = MAX(ret, read_500(info, channel,
								get_timing_register_addr
								(lane, 0, slot,
								 rank), 9));
	return ret;
}

static void dmi_setup(void)
{
	gav(dmibar_read8(0x254));
	dmibar_write8(0x254, 1 << 0);
	dmibar_write16(0x1b8, 0x18f2);
	mchbar_clrsetbits16(0x48, ~0, 1 << 1);

	dmibar_setbits32(0xd68, 1 << 27);

	outl((gav(inl(DEFAULT_GPIOBASE | 0x38)) & ~0x140000) | 0x400000,
	     DEFAULT_GPIOBASE | 0x38);
	gav(inb(DEFAULT_GPIOBASE | 0xe));	// = 0xfdcaff6e
}

void chipset_init(const int s3resume)
{
	u8 x2ca8;
	u16 ggc;
	u8 gfxsize;

	x2ca8 = mchbar_read8(0x2ca8);
	if ((x2ca8 & 1) || (x2ca8 == 8 && !s3resume)) {
		printk(BIOS_DEBUG, "soft reset detected, rebooting properly\n");
		mchbar_write8(0x2ca8, 0);
		system_reset();
	}

	dmi_setup();

	mchbar_write16(0x1170, 0xa880);
	mchbar_write8(0x11c1, 1 << 0);
	mchbar_write16(0x1170, 0xb880);
	mchbar_clrsetbits8(0x1210, ~0, 0x84);

	gfxsize = get_uint_option("gfx_uma_size", 0);	/* 0 for 32MB */

	ggc = 0xb00 | ((gfxsize + 5) << 4);

	pci_write_config16(NORTHBRIDGE, GGC, ggc | 2);

	u16 deven;
	deven = pci_read_config16(NORTHBRIDGE, DEVEN);	// = 0x3

	if (deven & 8) {
		mchbar_write8(0x2c30, 1 << 5);
		pci_read_config8(NORTHBRIDGE, 0x8);	// = 0x18
		mchbar_setbits16(0x2c30, 1 << 9);
		mchbar_write16(0x2c32, 0x434);
		mchbar_clrsetbits32(0x2c44, ~0, 0x1053687);
		pci_read_config8(GMA, MSAC);	// = 0x2
		pci_write_config8(GMA, MSAC, 0x2);
		RCBA8(0x2318);
		RCBA8(0x2318) = 0x47;
		RCBA8(0x2320);
		RCBA8(0x2320) = 0xfc;
	}

	mchbar_clrsetbits32(0x30, ~0, 0x40);

	pci_write_config16(NORTHBRIDGE, GGC, ggc);
	gav(RCBA32(0x3428));
	RCBA32(0x3428) = 0x1d;
}

static u8 get_bits_420(const u32 reg32)
{
	u8 val = 0;
	val |= (reg32 >> 4) & (1 << 0);
	val |= (reg32 >> 2) & (1 << 1);
	val |= (reg32 >> 0) & (1 << 2);
	return val;
}

void raminit(const int s3resume, const u8 *spd_addrmap)
{
	unsigned int channel, slot, lane, rank;
	struct raminfo info;
	u8 x2ca8;
	int cbmem_wasnot_inited;

	x2ca8 = mchbar_read8(0x2ca8);

	printk(RAM_DEBUG, "Scratchpad MCHBAR8(0x2ca8): 0x%04x\n", x2ca8);

	memset(&info, 0x5a, sizeof(info));

	info.last_500_command[0] = 0;
	info.last_500_command[1] = 0;

	info.board_lane_delay[0] = 0x14;
	info.board_lane_delay[1] = 0x07;
	info.board_lane_delay[2] = 0x07;
	info.board_lane_delay[3] = 0x08;
	info.board_lane_delay[4] = 0x56;
	info.board_lane_delay[5] = 0x04;
	info.board_lane_delay[6] = 0x04;
	info.board_lane_delay[7] = 0x05;
	info.board_lane_delay[8] = 0x10;

	info.training.reg_178 = 0;
	info.training.reg_10b = 0;

	/* Wait for some bit, maybe TXT clear. */
	while (!(read8((u8 *)0xfed40000) & (1 << 7)))
		;

	/* Wait for ME to be ready */
	intel_early_me_init();
	info.memory_reserved_for_heci_mb = intel_early_me_uma_size();

	/* before SPD */
	timestamp_add_now(101);

	if (!s3resume || 1) {	// possible error
		memset(&info.populated_ranks, 0, sizeof(info.populated_ranks));

		info.use_ecc = 1;
		for (channel = 0; channel < NUM_CHANNELS; channel++)
			for (slot = 0; slot < NUM_SLOTS; slot++) {
				int v;
				int try;
				int addr;
				const u8 useful_addresses[] = {
					DEVICE_TYPE,
					MODULE_TYPE,
					DENSITY,
					RANKS_AND_DQ,
					MEMORY_BUS_WIDTH,
					TIMEBASE_DIVIDEND,
					TIMEBASE_DIVISOR,
					CYCLETIME,
					CAS_LATENCIES_LSB,
					CAS_LATENCIES_MSB,
					CAS_LATENCY_TIME,
					0x11, 0x12, 0x13, 0x14, 0x15,
					0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b,
					    0x1c, 0x1d,
					THERMAL_AND_REFRESH,
					0x20,
					REFERENCE_RAW_CARD_USED,
					RANK1_ADDRESS_MAPPING,
					0x75, 0x76, 0x77, 0x78,
					0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e,
					    0x7f, 0x80, 0x81, 0x82, 0x83, 0x84,
					    0x85, 0x86, 0x87, 0x88,
					0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e,
					    0x8f, 0x90, 0x91, 0x92, 0x93, 0x94,
					    0x95
				};
				if (!spd_addrmap[2 * channel + slot])
					continue;
				for (try = 0; try < 5; try++) {
					v = smbus_read_byte(spd_addrmap[2 * channel + slot],
							    DEVICE_TYPE);
					if (v >= 0)
						break;
				}
				if (v < 0)
					continue;
				for (addr = 0;
				     addr <
				     ARRAY_SIZE(useful_addresses); addr++)
					gav(info.
					    spd[channel][0][useful_addresses
							    [addr]] =
					    smbus_read_byte(spd_addrmap[2 * channel + slot],
							    useful_addresses
							    [addr]));
				if (info.spd[channel][0][DEVICE_TYPE] != 11)
					die("Only DDR3 is supported");

				v = info.spd[channel][0][RANKS_AND_DQ];
				info.populated_ranks[channel][0][0] = 1;
				info.populated_ranks[channel][0][1] =
				    ((v >> 3) & 7);
				if (((v >> 3) & 7) > 1)
					die("At most 2 ranks are supported");
				if ((v & 7) == 0 || (v & 7) > 2)
					die("Only x8 and x16 modules are supported");
				if ((info.
				     spd[channel][slot][MODULE_TYPE] & 0xF) != 2
				    && (info.
					spd[channel][slot][MODULE_TYPE] & 0xF)
				    != 3)
					die("Registered memory is not supported");
				info.is_x16_module[channel][0] = (v & 7) - 1;
				info.density[channel][slot] =
				    info.spd[channel][slot][DENSITY] & 0xF;
				if (!
				    (info.
				     spd[channel][slot][MEMORY_BUS_WIDTH] &
				     0x18))
					info.use_ecc = 0;
			}

		gav(0x55);

		for (channel = 0; channel < NUM_CHANNELS; channel++) {
			int v = 0;
			for (slot = 0; slot < NUM_SLOTS; slot++)
				for (rank = 0; rank < NUM_RANKS; rank++)
					v |= info.
					    populated_ranks[channel][slot][rank]
					    << (2 * slot + rank);
			info.populated_ranks_mask[channel] = v;
		}

		gav(0x55);

		gav(pci_read_config32(NORTHBRIDGE, CAPID0 + 4));
	}

	/* after SPD  */
	timestamp_add_now(102);

	mchbar_clrbits8(0x2ca8, 1 << 1 | 1 << 0);

	collect_system_info(&info);
	calculate_timings(&info);

	if (!s3resume) {
		u8 reg8 = pci_read_config8(SOUTHBRIDGE, GEN_PMCON_2);
		if (x2ca8 == 0 && (reg8 & 0x80)) {
			/* Don't enable S4-assertion stretch. Makes trouble on roda/rk9.
			   reg8 = pci_read_config8(PCI_DEV(0, 0x1f, 0), 0xa4);
			   pci_write_config8(PCI_DEV(0, 0x1f, 0), 0xa4, reg8 | 0x08);
			 */

			/* Clear bit7. */

			pci_write_config8(SOUTHBRIDGE, GEN_PMCON_2,
				   (reg8 & ~(1 << 7)));

			printk(BIOS_INFO,
			       "Interrupted RAM init, reset required.\n");
			system_reset();
		}
	}

	if (!s3resume && x2ca8 == 0)
		pci_write_config8(SOUTHBRIDGE, GEN_PMCON_2,
			      pci_read_config8(SOUTHBRIDGE, GEN_PMCON_2) | 0x80);

	compute_derived_timings(&info);

	early_quickpath_init(&info, x2ca8);

	info.cached_training = get_cached_training();

	if (x2ca8 == 0)
		late_quickpath_init(&info, s3resume);

	mchbar_setbits32(0x2c80, 1 << 24);
	mchbar_write32(0x1804, mchbar_read32(0x1c04) & ~(1 << 27));

	mchbar_read8(0x2ca8);	// !!!!

	if (x2ca8 == 0) {
		mchbar_clrbits8(0x2ca8, 3);
		mchbar_write8(0x2ca8, mchbar_read8(0x2ca8) + 4);	// "+" or  "|"?
		/* This issues a CPU reset without resetting the platform */
		printk(BIOS_DEBUG, "Issuing a CPU reset\n");
		/* Write back the S3 state to PM1_CNT to let the reset CPU
		   know it also needs to take the s3 path. */
		if (s3resume)
			write_pmbase32(PM1_CNT, read_pmbase32(PM1_CNT)
				       | (SLP_TYP_S3 << 10));
		mchbar_setbits32(0x1af0, 1 << 4);
		halt();
	}

	mchbar_clrbits8(0x2ca8, 0);	// !!!!

	mchbar_clrbits32(0x2c80, 1 << 24);

	pci_write_config32(QPI_NON_CORE, MAX_RTIDS, 0x20220);

	{
		u8 x2c20 = (mchbar_read16(0x2c20) >> 8) & 3;
		u16 x2c10 = mchbar_read16(0x2c10);
		u16 value = mchbar_read16(0x2c00);
		if (x2c20 == 0 && (x2c10 & 0x300) == 0)
			value |= (1 << 7);
		else
			value &= ~(1 << 0);

		mchbar_write16(0x2c00, value);
	}

	udelay(1000);	// !!!!

	write_1d0(0, 0x33d, 0, 0);
	write_500(&info, 0, 0, 0xb61, 0, 0);
	write_500(&info, 1, 0, 0xb61, 0, 0);
	mchbar_write32(0x1a30, 0);
	mchbar_write32(0x1a34, 0);
	mchbar_write16(0x614, 0xb5b | (info.populated_ranks[1][0][0] * 0x404) |
		(info.populated_ranks[0][0][0] * 0xa0));
	mchbar_write16(0x616, 0x26a);
	mchbar_write32(0x134, 0x856000);
	mchbar_write32(0x160, 0x5ffffff);
	mchbar_clrsetbits32(0x114, ~0, 0xc2024440);	// !!!!
	mchbar_clrsetbits32(0x118, ~0, 0x4);	// !!!!
	for (channel = 0; channel < NUM_CHANNELS; channel++)
		mchbar_write32(0x260 + (channel << 10), 0x30809ff |
			(info.populated_ranks_mask[channel] & 3) << 20);
	for (channel = 0; channel < NUM_CHANNELS; channel++) {
		mchbar_write16(0x31c + (channel << 10), 0x101);
		mchbar_write16(0x360 + (channel << 10), 0x909);
		mchbar_write16(0x3a4 + (channel << 10), 0x101);
		mchbar_write16(0x3e8 + (channel << 10), 0x101);
		mchbar_write32(0x320 + (channel << 10), 0x29002900);
		mchbar_write32(0x324 + (channel << 10), 0);
		mchbar_write32(0x368 + (channel << 10), 0x32003200);
		mchbar_write16(0x352 + (channel << 10), 0x505);
		mchbar_write16(0x354 + (channel << 10), 0x3c3c);
		mchbar_write16(0x356 + (channel << 10), 0x1040);
		mchbar_write16(0x39a + (channel << 10), 0x73e4);
		mchbar_write16(0x3de + (channel << 10), 0x77ed);
		mchbar_write16(0x422 + (channel << 10), 0x1040);
	}

	write_1d0(0x4, 0x151, 4, 1);
	write_1d0(0, 0x142, 3, 1);
	rdmsr(0x1ac);	// !!!!
	write_500(&info, 1, 1, 0x6b3, 4, 1);
	write_500(&info, 1, 1, 0x6cf, 4, 1);

	rmw_1d0(0x21c, 0x38, 0, 6);

	write_1d0(((!info.populated_ranks[1][0][0]) << 1) | ((!info.
							      populated_ranks[0]
							      [0][0]) << 0),
		  0x1d1, 3, 1);
	for (channel = 0; channel < NUM_CHANNELS; channel++) {
		mchbar_write16(0x38e + (channel << 10), 0x5f5f);
		mchbar_write16(0x3d2 + (channel << 10), 0x5f5f);
	}

	set_334(0);

	program_base_timings(&info);

	mchbar_setbits8(0x5ff, 1 << 7);

	write_1d0(0x2, 0x1d5, 2, 1);
	write_1d0(0x20, 0x166, 7, 1);
	write_1d0(0x0, 0xeb, 3, 1);
	write_1d0(0x0, 0xf3, 6, 1);

	for (channel = 0; channel < NUM_CHANNELS; channel++) {
		u8 a = 0;
		if (info.populated_ranks[channel][0][1] && info.clock_speed_index > 1)
			a = 3;
		if (info.silicon_revision == 0 || info.silicon_revision == 1)
			a = 3;

		for (lane = 0; lane < 9; lane++) {
			const u16 addr = 0x125 + get_lane_offset(0, 0, lane);
			rmw_500(&info, channel, addr, 6, 0xf, a);
		}
	}

	if (s3resume) {
		if (!info.cached_training) {
			u32 reg32;
			printk(BIOS_ERR,
			       "Couldn't find training data. Rebooting\n");
			reg32 = inl(DEFAULT_PMBASE + 0x04);
			outl(reg32 & ~(7 << 10), DEFAULT_PMBASE + 0x04);
			full_reset();
		}
		int tm;
		info.training = *info.cached_training;
		for (tm = 0; tm < 4; tm++)
			for (channel = 0; channel < NUM_CHANNELS; channel++)
				for (slot = 0; slot < NUM_SLOTS; slot++)
					for (rank = 0; rank < NUM_RANKS; rank++)
						for (lane = 0; lane < 9; lane++)
							write_500(&info,
								  channel,
								  info.training.
								  lane_timings
								  [tm][channel]
								  [slot][rank]
								  [lane],
								  get_timing_register_addr
								  (lane, tm,
								   slot, rank),
								  9, 0);
		write_1d0(info.cached_training->reg_178, 0x178, 7, 1);
		write_1d0(info.cached_training->reg_10b, 0x10b, 6, 1);
	}

	mchbar_clrsetbits32(0x1f4, ~0, 1 << 17);	// !!!!
	mchbar_write32(0x1f0, 0x1d000200);
	mchbar_setbits8(0x1f0, 1 << 0);
	while (mchbar_read8(0x1f0) & 1)
		;

	program_board_delay(&info);

	mchbar_write8(0x5ff, 0);
	mchbar_write8(0x5ff, 1 << 7);
	mchbar_write8(0x5f4, 1 << 0);

	mchbar_clrbits32(0x130, 1 << 1);	// | 2 when ?
	while (mchbar_read32(0x130) & 1)
		;

	rmw_1d0(0x14b, 0x47, 0x30, 7);
	rmw_1d0(0xd6,  0x38, 7, 6);
	rmw_1d0(0x328, 0x38, 7, 6);

	for (channel = 0; channel < NUM_CHANNELS; channel++)
		set_4cf(&info, channel, 1, 0);

	rmw_1d0(0x116, 0xe,  0, 4);
	rmw_1d0(0xae,  0x3e, 0, 6);
	rmw_1d0(0x300, 0x3e, 0, 6);
	mchbar_clrbits16(0x356, 1 << 15);
	mchbar_clrbits16(0x756, 1 << 15);
	mchbar_clrbits32(0x140, 7 << 24);
	mchbar_clrbits32(0x138, 7 << 24);
	mchbar_write32(0x130, 0x31111301);
	/* Wait until REG130b0 is 1.  */
	while (mchbar_read32(0x130) & 1)
		;

	u8 value_a1;
	{
		const u8 val_xa1 = get_bits_420(read_1d0(0xa1, 6));	// = 0x1cf4040 // !!!!
		const u8 val_2f3 = get_bits_420(read_1d0(0x2f3, 6));	// = 0x10a4040 // !!!!
		value_a1 = val_xa1;
		rmw_1d0(0x320, 0x38, val_2f3, 6);
		rmw_1d0(0x14b, 0x78, val_xa1, 7);
		rmw_1d0(0xce,  0x38, val_xa1, 6);
	}

	for (channel = 0; channel < NUM_CHANNELS; channel++)
		set_4cf(&info, channel, 1, 1);

	rmw_1d0(0x116, 0xe, 1, 4);	// = 0x4040432 // !!!!
	{
		if ((mchbar_read32(0x144) & 0x1f) < 0x13)
			value_a1 += 2;
		else
			value_a1 += 1;

		if (value_a1 > 7)
			value_a1 = 7;

		write_1d0(2, 0xae, 6, 1);
		write_1d0(2, 0x300, 6, 1);
		write_1d0(value_a1, 0x121, 3, 1);
		rmw_1d0(0xd6,  0x38, 4, 6);
		rmw_1d0(0x328, 0x38, 4, 6);
	}

	for (channel = 0; channel < NUM_CHANNELS; channel++)
		set_4cf(&info, channel, 2, 0);

	mchbar_write32(0x130, 0x11111301 | info.populated_ranks[1][0][0] << 30 |
		info.populated_ranks[0][0][0] << 29);
	while (mchbar_read8(0x130) & 1)
		;

	{
		const u8 val_xa1 = get_bits_420(read_1d0(0xa1, 6));
		read_1d0(0x2f3, 6);		// = 0x10a4054 // !!!!
		rmw_1d0(0x21c, 0x38, 0, 6);
		rmw_1d0(0x14b, 0x78, val_xa1, 7);
	}

	for (channel = 0; channel < NUM_CHANNELS; channel++)
		set_4cf(&info, channel, 2, 1);

	set_334(1);

	mchbar_write8(0x1e8, 1 << 2);

	for (channel = 0; channel < NUM_CHANNELS; channel++) {
		write_500(&info, channel,
			  0x3 & ~(info.populated_ranks_mask[channel]), 0x6b7, 2,
			  1);
		write_500(&info, channel, 0x3, 0x69b, 2, 1);
	}
	mchbar_clrsetbits32(0x2d0, ~0xff0c01ff, 0x200000);
	mchbar_write16(0x6c0, 0x14a0);
	mchbar_clrsetbits32(0x6d0, ~0xff0000ff, 0x8000);
	mchbar_write16(0x232, 1 << 3);
	/* 0x40004 or 0 depending on ? */
	mchbar_clrsetbits32(0x234, 0x40004, 0x40004);
	mchbar_clrsetbits32(0x34, 0x7, 5);
	mchbar_write32(0x128, 0x2150d05);
	mchbar_write8(0x12c, 0x1f);
	mchbar_write8(0x12d, 0x56);
	mchbar_write8(0x12e, 0x31);
	mchbar_write8(0x12f, 0);
	mchbar_write8(0x271, 1 << 1);
	mchbar_write8(0x671, 1 << 1);
	mchbar_write8(0x1e8, 1 << 2);
	for (channel = 0; channel < NUM_CHANNELS; channel++)
		mchbar_write32(0x294 + (channel << 10),
			(info.populated_ranks_mask[channel] & 3) << 16);
	mchbar_clrsetbits32(0x134, ~0xfc01ffff, 0x10000);
	mchbar_clrsetbits32(0x134, ~0xfc85ffff, 0x850000);
	for (channel = 0; channel < NUM_CHANNELS; channel++)
		mchbar_clrsetbits32(0x260 + (channel << 10), 0xf << 20, 1 << 27 |
			(info.populated_ranks_mask[channel] & 3) << 20);

	if (!s3resume)
		jedec_init(&info);

	int totalrank = 0;
	for (channel = 0; channel < NUM_CHANNELS; channel++)
		for (slot = 0; slot < NUM_SLOTS; slot++)
			for (rank = 0; rank < NUM_RANKS; rank++)
				if (info.populated_ranks[channel][slot][rank]) {
					jedec_read(&info, channel, slot, rank,
						   totalrank, 0xa, 0x400);
					totalrank++;
				}

	mchbar_write8(0x12c, 0x9f);

	mchbar_clrsetbits8(0x271, 0x3e, 0x0e);
	mchbar_clrsetbits8(0x671, 0x3e, 0x0e);

	if (!s3resume) {
		for (channel = 0; channel < NUM_CHANNELS; channel++) {
			mchbar_write32(0x294 + (channel << 10),
				(info.populated_ranks_mask[channel] & 3) << 16);
			mchbar_write16(0x298 + (channel << 10),
				info.populated_ranks[channel][0][0] |
				info.populated_ranks[channel][0][1] << 5);
			mchbar_write32(0x29c + (channel << 10), 0x77a);
		}
		mchbar_clrsetbits32(0x2c0, ~0, 0x6009cc00);	// !!!!

		{
			u8 a, b;
			a = mchbar_read8(0x243);
			b = mchbar_read8(0x643);
			mchbar_write8(0x243, a | 2);
			mchbar_write8(0x643, b | 2);
		}

		write_1d0(7, 0x19b, 3, 1);
		write_1d0(7, 0x1c0, 3, 1);
		write_1d0(4, 0x1c6, 4, 1);
		write_1d0(4, 0x1cc, 4, 1);
		rmw_1d0(0x151, 0xf, 0x4, 4);
		mchbar_write32(0x584, 0xfffff);
		mchbar_write32(0x984, 0xfffff);

		for (channel = 0; channel < NUM_CHANNELS; channel++)
			for (slot = 0; slot < NUM_SLOTS; slot++)
				for (rank = 0; rank < NUM_RANKS; rank++)
					if (info.
					    populated_ranks[channel][slot]
					    [rank])
						config_rank(&info, s3resume,
							    channel, slot,
							    rank);

		mchbar_write8(0x243, 1);
		mchbar_write8(0x643, 1);
	}

	/* was == 1 but is common */
	pci_write_config16(NORTHBRIDGE, 0xc8, 3);
	write_26c(0, 0x820);
	write_26c(1, 0x820);
	mchbar_setbits32(0x130, 1 << 1);
	/* end */

	if (s3resume) {
		for (channel = 0; channel < NUM_CHANNELS; channel++) {
			mchbar_write32(0x294 + (channel << 10),
				(info.populated_ranks_mask[channel] & 3) << 16);
			mchbar_write16(0x298 + (channel << 10),
				info.populated_ranks[channel][0][0] |
				info.populated_ranks[channel][0][1] << 5);
			mchbar_write32(0x29c + (channel << 10), 0x77a);
		}
		mchbar_clrsetbits32(0x2c0, ~0, 0x6009cc00);	// !!!!
	}

	mchbar_clrbits32(0xfa4, 1 << 24 | 1 << 1);
	mchbar_write32(0xfb0, 0x2000e019);

	/* Before training. */
	timestamp_add_now(103);

	if (!s3resume)
		ram_training(&info);

	/* After training. */
	timestamp_add_now(104);

	dump_timings(&info);

	program_modules_memory_map(&info, 0);
	program_total_memory_map(&info);

	if (info.non_interleaved_part_mb != 0 && info.interleaved_part_mb != 0)
		mchbar_write8(0x111, 0 << 2 | 1 << 5 | 1 << 6 | 0 << 7);
	else if (have_match_ranks(&info, 0, 4) && have_match_ranks(&info, 1, 4))
		mchbar_write8(0x111, 3 << 2 | 1 << 5 | 0 << 6 | 1 << 7);
	else if (have_match_ranks(&info, 0, 2) && have_match_ranks(&info, 1, 2))
		mchbar_write8(0x111, 3 << 2 | 1 << 5 | 0 << 6 | 0 << 7);
	else
		mchbar_write8(0x111, 3 << 2 | 1 << 5 | 1 << 6 | 0 << 7);

	mchbar_clrbits32(0xfac, 1 << 31);
	mchbar_write32(0xfb4, 0x4800);
	mchbar_write32(0xfb8, (info.revision < 8) ? 0x20 : 0x0);
	mchbar_write32(0xe94, 0x7ffff);
	mchbar_write32(0xfc0, 0x80002040);
	mchbar_write32(0xfc4, 0x701246);
	mchbar_clrbits8(0xfc8, 0x70);
	mchbar_setbits32(0xe5c, 1 << 24);
	mchbar_clrsetbits32(0x1a70, 3 << 20, 2 << 20);
	mchbar_write32(0x50, 0x700b0);
	mchbar_write32(0x3c, 0x10);
	mchbar_clrsetbits8(0x1aa8, 0x3f, 0xa);
	mchbar_setbits8(0xff4, 1 << 1);
	mchbar_clrsetbits32(0xff8, 0xe008, 0x1020);

	mchbar_write32(0xd00, IOMMU_BASE2 | 1);
	mchbar_write32(0xd40, IOMMU_BASE1 | 1);
	mchbar_write32(0xdc0, IOMMU_BASE4 | 1);

	write32p(IOMMU_BASE1 | 0xffc, 0x80000000);
	write32p(IOMMU_BASE2 | 0xffc, 0xc0000000);
	write32p(IOMMU_BASE4 | 0xffc, 0x80000000);

	{
		u32 eax;

		eax = info.fsb_frequency / 9;
		mchbar_clrsetbits32(0xfcc, 0x3ffff,
			(eax * 0x280) | (eax * 0x5000) | eax | 0x40000);
		mchbar_write32(0x20, 0x33001);
	}

	for (channel = 0; channel < NUM_CHANNELS; channel++) {
		mchbar_clrbits32(0x220 + (channel << 10), 0x7770);
		if (info.max_slots_used_in_channel == 1)
			mchbar_setbits16(0x237 + (channel << 10), 0x0201);
		else
			mchbar_clrbits16(0x237 + (channel << 10), 0x0201);

		mchbar_setbits8(0x241 + (channel << 10), 1 << 0);

		if (info.clock_speed_index <= 1 && (info.silicon_revision == 2
			|| info.silicon_revision == 3))
			mchbar_setbits32(0x248 + (channel << 10), 0x00102000);
		else
			mchbar_clrbits32(0x248 + (channel << 10), 0x00102000);
	}

	mchbar_setbits32(0x115, 1 << 24);

	{
		u8 al;
		al = 0xd;
		if (!(info.silicon_revision == 0 || info.silicon_revision == 1))
			al += 2;
		al |= ((1 << (info.max_slots_used_in_channel - 1)) - 1) << 4;
		mchbar_write32(0x210, al << 16 | 0x20);
	}

	for (channel = 0; channel < NUM_CHANNELS; channel++) {
		mchbar_write32(0x288 + (channel << 10), 0x70605040);
		mchbar_write32(0x28c + (channel << 10), 0xfffec080);
		mchbar_write32(0x290 + (channel << 10), 0x282091c |
			(info.max_slots_used_in_channel - 1) << 0x16);
	}
	u32 reg1c;
	pci_read_config32(NORTHBRIDGE, 0x40);	// = DEFAULT_EPBAR | 0x001 // OK
	reg1c = epbar_read32(EPVC1RCAP);	// = 0x8001 // OK
	pci_read_config32(NORTHBRIDGE, 0x40);	// = DEFAULT_EPBAR | 0x001 // OK
	epbar_write32(EPVC1RCAP, reg1c);	// OK
	mchbar_read8(0xe08);	// = 0x0
	pci_read_config32(NORTHBRIDGE, 0xe4);	// = 0x316126
	mchbar_setbits8(0x1210, 1 << 1);
	mchbar_write32(0x1200, 0x8800440);
	mchbar_write32(0x1204, 0x53ff0453);
	mchbar_write32(0x1208, 0x19002043);
	mchbar_write16(0x1214, 0x320);

	if (info.revision == 0x10 || info.revision == 0x11) {
		mchbar_write16(0x1214, 0x220);
		mchbar_setbits8(0x1210, 1 << 6);
	}

	mchbar_setbits8(0x1214, 1 << 2);
	mchbar_write8(0x120c, 1);
	mchbar_write8(0x1218, 3);
	mchbar_write8(0x121a, 3);
	mchbar_write8(0x121c, 3);
	mchbar_write16(0xc14, 0);
	mchbar_write16(0xc20, 0);
	mchbar_write32(0x1c, 0);

	/* revision dependent here.  */

	mchbar_setbits16(0x1230, 0x1f07);

	if (info.uma_enabled)
		mchbar_setbits32(0x11f4, 1 << 28);

	mchbar_setbits16(0x1230, 1 << 15);
	mchbar_setbits8(0x1214, 1 << 0);

	u8 bl, ebpb;
	u16 reg_1020;

	reg_1020 = mchbar_read32(0x1020);	// = 0x6c733c  // OK
	mchbar_write8(0x1070, 1);

	mchbar_write32(0x1000, 0x100);
	mchbar_write8(0x1007, 0);

	if (reg_1020 != 0) {
		mchbar_write16(0x1018, 0);
		bl = reg_1020 >> 8;
		ebpb = reg_1020 & 0xff;
	} else {
		ebpb = 0;
		bl = 8;
	}

	rdmsr(0x1a2);

	mchbar_write32(0x1014, 0xffffffff);

	mchbar_write32(0x1010, ((((ebpb + 0x7d) << 7) / bl) & 0xff) * !!reg_1020);

	mchbar_write8(0x101c, 0xb8);

	mchbar_clrsetbits8(0x123e, 0xf0, 0x60);
	if (reg_1020 != 0) {
		mchbar_clrsetbits32(0x123c, 0xf << 20, 0x6 << 20);
		mchbar_write8(0x101c, 0xb8);
	}

	const u64 heci_uma_addr =
	    ((u64)
	     ((((u64)pci_read_config16(NORTHBRIDGE, TOM)) << 6) -
	      info.memory_reserved_for_heci_mb)) << 20;

	setup_heci_uma(heci_uma_addr, info.memory_reserved_for_heci_mb);

	if (info.uma_enabled) {
		u16 ax;
		mchbar_setbits32(0x11b0, 1 << 14);
		mchbar_setbits32(0x11b4, 1 << 14);
		mchbar_setbits16(0x1190, 1 << 14);

		ax = mchbar_read16(0x1190) & 0xf00;	// = 0x480a  // OK
		mchbar_write16(0x1170, ax | (mchbar_read16(0x1170) & 0x107f) | 0x4080);
		mchbar_setbits16(0x1170, 1 << 12);

		udelay(1000);

		u16 ecx;
		for (ecx = 0xffff; ecx && (mchbar_read16(0x1170) & (1 << 12)); ecx--)
			;
		mchbar_clrbits16(0x1190, 1 << 14);
	}

	pci_write_config8(SOUTHBRIDGE, GEN_PMCON_2,
		      pci_read_config8(SOUTHBRIDGE, GEN_PMCON_2) & ~0x80);
	udelay(10000);
	mchbar_write16(0x2ca8, 1 << 3);

	udelay(1000);
	dump_timings(&info);
	cbmem_wasnot_inited = cbmem_recovery(s3resume);

	if (!s3resume)
		save_timings(&info);
	if (s3resume && cbmem_wasnot_inited) {
		printk(BIOS_ERR, "Failed S3 resume.\n");
		ram_check_nodie(1 * MiB);

		/* Failed S3 resume, reset to come up cleanly */
		full_reset();
	}
}
