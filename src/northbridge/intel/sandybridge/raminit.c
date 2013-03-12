/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#ifndef REAL
#define REAL 1
#endif

#if REAL
#include <console/console.h>
#include <string.h>
#include <arch/hlt.h>
#include <arch/io.h>
#include <cpu/x86/msr.h>
#include <arch/romcc_io.h>
#include <cbmem.h>
#include <arch/cbfs.h>
#include <cbfs.h>
#include <ip_checksum.h>
#include <pc80/mc146818rtc.h>
#include <device/pci_def.h>
#include <arch/cpu.h>
#include <spd.h>
#include "raminit.h"
#include "pei_data.h"
#endif

#if !REAL
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef u32 device_t;
#endif

#include "sandybridge.h"

#include "southbridge/intel/bd82x6x/me.h"

#if REAL
#include <delay.h>

/* Management Engine is in the southbridge */
#if CONFIG_CHROMEOS
#include <vendorcode/google/chromeos/chromeos.h>
#else
#define recovery_mode_enabled(x) 0
#endif
#endif


#define NORTHBRIDGE 0, 0, 0
#define SOUTHBRIDGE 0, 0x1f, 0
#define HECIDEV 0, 0x16, 0
#define HECIBAR 0x10

struct ram_training
{
  u16 lane_timings[4][2][2][2][9]; 
  u16 reg_178;
  u16 reg_10b;
};

#if !REAL
#include "raminit_fake.c"
#else

#include <lib.h> /* Prototypes */

static void
my_write_msr (u32 addr, u64 val)
{
  msr_t msr = { .lo = val, .hi = val >> 32 };

  wrmsr (addr, msr);
}

static inline void
write_acpi32 (u32 addr, u32 val)
{
  outl (val, DEFAULT_PMBASE | addr);
}

#if 0
static void
nvram_write (u8 addr, u8 val)
{
  cmos_write (val, addr);
}

static inline u8
nvram_read (u8 addr)
{
  return cmos_read (addr);
}
#endif

static inline void
write_acpi16 (u32 addr, u16 val)
{
  outw (val, DEFAULT_PMBASE | addr);
}

static inline u32
read_acpi32 (u32 addr)
{
  return inl (DEFAULT_PMBASE | addr);
}

static inline u16
read_acpi16 (u32 addr)
{
  return inw (DEFAULT_PMBASE | addr);
}

static inline u16
read_tco16 (u32 addr)
{
  return inw (DEFAULT_PMBASE | (0x60 + addr));
}

static inline u8
read_tco8 (u32 addr)
{
  return inb (DEFAULT_PMBASE | (0x60 + addr));
}

static inline void
write_tco16 (u32 addr, u16 val)
{
  outw (val, DEFAULT_PMBASE | (0x60 + addr));
}

static inline void
write_tco8 (u32 addr, u8 val)
{
  outb (val, DEFAULT_PMBASE | (0x60 + addr));
}

static inline void
write_mchbar32 (u32 addr, u32 val)
{
  MCHBAR32(addr) = val;
  //  udelay (1000);
}

static inline void
write_mchbar16 (u32 addr, u16 val)
{
  MCHBAR16(addr) = val;
  //udelay (1000);
}

static inline void
write_mchbar8 (u32 addr, u8 val)
{
  MCHBAR8(addr) = val;
  //udelay (1000);
}

static u32 mchgav (u32 addr, u32 val)
{
  //printk (BIOS_DEBUG, "MCH: [%x] => %x\n", addr, val);
  return val;
}

static u32 pcigav (u32 addr, u32 val)
{
  //printk (BIOS_DEBUG, "PCI: [%x] => %x\n", addr, val);
  return val;
}

static inline u32
read_mchbar32 (u32 addr)
{
  return mchgav (addr, MCHBAR32(addr));
}

static inline u32
read_mchbar32_bypass (u32 addr)
{
  return MCHBAR32(addr);
}

static inline u16
read_mchbar16 (u32 addr)
{
  return mchgav (addr, MCHBAR16(addr));
}

static inline u8
read_mchbar8 (u32 addr)
{
  return mchgav (addr, MCHBAR8(addr));
}

static inline u8
read_mchbar8_bypass (u32 addr)
{
  return MCHBAR8(addr);
}

static u32
pci_read32 (int bus, int dev, int func, u32 addr)
{
  return pcigav (addr, pci_read_config32(PCI_DEV(bus, dev, func), addr));
}

static u8
pci_read8 (int bus, int dev, int func, u32 addr)
{
  return pcigav (addr, pci_read_config8(PCI_DEV(bus, dev, func), addr));
}

static void
pci_write32 (int bus, int dev, int func, u32 addr, u32 val)
{
  return pci_write_config32(PCI_DEV(bus, dev, func), addr, val);
}

static void
pci_write16 (int bus, int dev, int func, u32 addr, u16 val)
{
  return pci_write_config16(PCI_DEV(bus, dev, func), addr, val);
}

static void
pci_write8 (int bus, int dev, int func, u32 addr, u8 val)
{
  return pci_write_config8(PCI_DEV(bus, dev, func), addr, val);
}

static u16
pci_read16 (int bus, int dev, int func, u32 addr)
{
  return pcigav (addr, pci_read_config16(PCI_DEV(bus, dev, func), addr));
}

static void
pci_mm_write8 (int bus, int dev, int func, u32 addr, u8 val)
{
  write8 (DEFAULT_PCIEXBAR | (bus << 20) | (dev << 15) | (func << 12) | addr, val);
}
static void
pci_mm_write16 (int bus, int dev, int func, u32 addr, u16 val)
{
  write16 (DEFAULT_PCIEXBAR | (bus << 20) | (dev << 15) | (func << 12) | addr, val);
}

static void
pci_mm_write32 (int bus, int dev, int func, u32 addr, u32 val)
{
  write32 (DEFAULT_PCIEXBAR | (bus << 20) | (dev << 15) | (func << 12) | addr, val);
}

static u32
pci_mm_read32 (int bus, int dev, int func, u32 addr)
{
  return pcigav (addr, read32 (DEFAULT_PCIEXBAR | (bus << 20) | (dev << 15) | (func << 12) | addr));
}

static u16
pci_mm_read16 (int bus, int dev, int func, u32 addr)
{
  return pcigav (addr, read16 (DEFAULT_PCIEXBAR | (bus << 20) | (dev << 15) | (func << 12) | addr));
}

static u8
pci_mm_read8 (int bus, int dev, int func, u32 addr)
{
  return pcigav (addr, read8 (DEFAULT_PCIEXBAR | (bus << 20) | (dev << 15) | (func << 12) | addr));
}

#if 0
static void clflush(u32 addr)
{
  asm volatile ("clflush (%0)" : : "r" (addr));
}

typedef struct _u128
{
  u64 lo;
  u64 hi;
} u128;

static void
read128 (u32 addr, u64 *out)
{
  u128 ret;
  u128 stor;
  asm volatile ("movdqu %%xmm0, %0\n"
                "movdqa (%2), %%xmm0\n"
		"movdqu %%xmm0, %1\n"
		"movdqu %0, %%xmm0":"+m"(stor),"=m"(ret):"r"(addr));
  out[0] = ret.lo;
  out[1] = ret.hi;
}
#endif

#endif

/* OK */
static void
write_1d0 (u32 val, u16 addr, int bits, int flag)
{
  write_mchbar32 (0x1d0, 0);
  while (read_mchbar32_bypass (0x1d0) & 0x800000);
  write_mchbar32 (0x1d4, (val & ((1 << bits) - 1)) | (2 << bits) | (flag << bits));
  write_mchbar32 (0x1d0, 0x40000000 | addr);
  while (read_mchbar32_bypass (0x1d0) & 0x800000);
}

/* OK */
static u16
read_1d0 (u16 addr, int split)
{
  u32 val;
  write_mchbar32 (0x1d0, 0);
  while (read_mchbar32_bypass (0x1d0) & 0x800000);
  write_mchbar32 (0x1d0, 0x80000000 | (((read_mchbar8_bypass (0x246) >> 2) & 3) + 0x361 - addr));
  while (read_mchbar32_bypass (0x1d0) & 0x800000);
  val = read_mchbar32_bypass (0x1d8);
  write_1d0 (0, 0x33d, 0, 0);
  write_1d0 (0, 0x33d, 0, 0);
  val &= ((1 << split) - 1);
  printk (BIOS_ERR, "R1D0C [%x] => %x\n", addr, val);
  return val;
}

static void
sfence (void)
{
#if REAL
  asm volatile ("sfence");
#endif
}


static inline u16
get_lane_offset (int slot, int rank, int lane)
{
  return 0x124 * lane + ((lane & 4) ? 0x23e : 0) + 11 * rank + 22 * slot - 0x452 * (lane == 8);
}

static inline u16
get_timing_register_addr (int lane, int tm, int slot, int rank)
{
  const u16 offs[] = { 0x1d, 0xa8, 0xe6, 0x5c };
  return get_lane_offset (slot, rank, lane) + offs[(tm + 3) % 4];
}


static u32 gav_real (int line, u32 in)
{
  printk (BIOS_DEBUG, "%d: GAV: %x\n", line, in);
  return in;
}

#define gav(x) gav_real (__LINE__, (x))

struct raminfo
{
  u16 clock_speed_index; /* clock_speed (REAL, not DDR) / 133.(3) - 3 */
  u16 some_base_frequency;
  u8 is_x16_module[2][2]; /* [CHANNEL][SLOT] */
  u8 density[2][2]; /* [CHANNEL][SLOT] */
  u8 populated_ranks[2][2][2]; /* [CHANNEL][SLOT][RANK] */
  int rank_start[2][2][2];
  u8 cas_latency;
  u8 board_lane_delay[9];
  u8 use_ecc;
  u8 revision;
  u8 max_supported_clock_speed_index;
  u8 uma_enabled;
  u8 spd[2][2][151]; /* [CHANNEL][SLOT][BYTE]  */
  u8 silicon_revision;
  u8 populated_ranks_mask[2];
  u8 max_slots_used_in_channel;
  u8 v4030[2];
  union {
    struct
    {
      u16 v4044[2];
      u16 v4048[2];
    };
    u8 u4044[2][2][2];
  };
  unsigned total_memory_mb;
  unsigned interleaved_part_mb;
  unsigned non_interleaved_part_mb;

  u32 heci_bar;
  u64 heci_uma_addr;
  unsigned memory_reserved_for_heci_mb;
  u32 v4086;

  struct ram_training training;
  u32 last_500_command[2];
};

static void
write_500 (struct raminfo *info, int channel, u32 val, u16 addr, int bits, int flag);

/* OK */
static u16
read_500_bypass (struct raminfo *info, int channel, u16 addr, int split)
{
  u32 val;
  info->last_500_command[channel] = 0x80000000;
  write_mchbar32 (0x500 + (channel << 10), 0);
  while (read_mchbar32_bypass (0x500 + (channel << 10)) & 0x800000);
  write_mchbar32 (0x500 + (channel << 10), 0x80000000 | (((read_mchbar8_bypass (0x246 + (channel << 10)) >> 2) & 3) + 0xb88 - addr));
  while (read_mchbar32_bypass (0x500 + (channel << 10)) & 0x800000);
  val = read_mchbar32_bypass (0x508 + (channel << 10));
  return val & ((1 << split) - 1);
}

static u16
read_500 (struct raminfo *info, int channel, u16 addr, int split)
{
  u16 ret;
  ret = read_500_bypass (info, channel, addr, split);
  printk (BIOS_ERR, "R500 [%d/%x] => %x\n", channel, addr, ret);
  return ret;
}

/* OK */
static void
write_500 (struct raminfo *info, int channel, u32 val, u16 addr, int bits, int flag)
{
  if (info->last_500_command[channel] == 0x80000000)
    {
      info->last_500_command[channel] = 0x40000000;
      write_500 (info, channel, 0, 0xb61, 0, 0);
    }
  write_mchbar32 (0x500 + (channel << 10), 0); 
  while (read_mchbar32 (0x500 + (channel << 10)) & 0x800000);
  write_mchbar32 (0x504 + (channel << 10), (val & ((1 << bits) - 1)) | (2 << bits) | (flag << bits));
  write_mchbar32 (0x500 + (channel << 10), 0x40000000 | addr);
  while (read_mchbar32 (0x500 + (channel << 10)) & 0x800000);
}

static int
rw_test (int rank)
{
  const u32 mask = 0xf00fc33c;
  int ok = 0xff;
  int i;
  for (i = 0; i < 64; i++)
    write32 ((rank << 28) | (i << 2), 0);
  sfence ();
  for (i = 0; i < 64; i++)
    gav (read32 ((rank << 28) | (i << 2)));
  sfence ();
  for (i = 0; i < 32; i++)
    {
      u32 pat = (((mask >> i) & 1) ? 0xffffffff : 0);
      write32 ((rank << 28) | (i << 3), pat);
      write32 ((rank << 28) | (i << 3) | 4, pat);
    }
  sfence ();
  for (i = 0; i < 32; i++)
    {
      u8 pat = (((mask >> i) & 1) ? 0xff : 0);
      int j;
      u32 val;
      gav (val = read32 ((rank << 28) | (i << 3)));
      for (j = 0; j < 4; j++)
	if (((val >> (j * 8)) & 0xff) != pat)
	  ok &= ~(1 << j);
      gav (val = read32 ((rank << 28) | (i << 3) | 4));
      for (j = 0; j < 4; j++)
	if (((val >> (j * 8)) & 0xff) != pat)
	  ok &= ~(16 << j);
    }
  sfence ();
  for (i = 0; i < 64; i++)
    write32 ((rank << 28) | (i << 2), 0);
  sfence ();
  for (i = 0; i < 64; i++)
    gav (read32 ((rank << 28) | (i << 2)));

  return ok;
}

static void
program_timings (struct raminfo *info, u16 base, int channel, int slot, int rank)
{
  int lane;
  for (lane = 0; lane < 8; lane++)
    {
      write_500 (info, channel, base + info->training.lane_timings[2][channel][slot][rank][lane],
		 get_timing_register_addr (lane, 2, slot, rank), 9, 0);
      write_500 (info, channel, base + info->training.lane_timings[3][channel][slot][rank][lane],
		 get_timing_register_addr (lane, 3, slot, rank), 9, 0);
    }
}

static void
write_26c (int channel, u16 si)
{
  write_mchbar32 (0x26c + (channel << 10), 0x03243f35);
  write_mchbar32 (0x268 + (channel << 10), 0xcfc00000 | (si << 9));
  write_mchbar16 (0x2b9 + (channel << 10), si);
}

static u32
get_580 (int channel, u8 addr)
{
  u32 ret;
  gav (read_1d0 (0x142, 3));
  write_mchbar8 (0x5ff, 0x0); /* OK */
  write_mchbar8 (0x5ff, 0x80); /* OK */
  write_mchbar32 (0x580 + (channel << 10), 0x8493c012 | addr);
  write_mchbar8 (0x580 + (channel << 10), read_mchbar8 (0x580 + (channel << 10)) | 1);
  while (!((ret = read_mchbar32 (0x580 + (channel << 10))) & 0x10000));
  write_mchbar8 (0x580 + (channel << 10), read_mchbar8 (0x580 + (channel << 10)) & ~1);
  return ret;
}

static void
seq8 (void)
{
  write_mchbar8 (0x243, 0x1);
  write_mchbar8 (0x643, 0x1);
}

const int cached_config = 0;

#define NUM_CHANNELS 2
#define NUM_SLOTS 2
#define NUM_RANKS 2
#define RANK_SHIFT 28
#define CHANNEL_SHIFT 10

#include "raminit_tables.c"

static void
seq9 (struct raminfo *info, int channel, int slot, int rank)
{
  int i, lane;

  for (i = 0; i < 2; i++)
    for (lane = 0; lane < 8; lane++)
      write_500 (info, channel, info->training.lane_timings[i + 1][channel][slot][rank][lane],
		 get_timing_register_addr (lane, i + 1, slot, rank), 9, 0);

  write_1d0 (1, 0x103, 6, 1);
  for (lane = 0; lane < 8; lane++)
    write_500 (info, channel, info->training.lane_timings[0][channel][slot][rank][lane], get_timing_register_addr (lane, 0, slot, rank), 9, 0);

  for (i = 0; i < 2; i++)
    {
      for (lane = 0; lane < 8; lane++)
	write_500 (info, channel, info->training.lane_timings[i + 1][channel][slot][rank][lane], get_timing_register_addr (lane, i + 1, slot, rank), 9, 0);
      gav (get_580 (channel, ((i + 1) << 2) | (rank << 5)));
    }

  gav (read_1d0 (0x142, 3)); // = 0x10408118
  write_mchbar8 (0x5ff, 0x0); /* OK */
  write_mchbar8 (0x5ff, 0x80); /* OK */
  write_1d0 (0x2, 0x142, 3, 1);
  for (lane = 0; lane < 8; lane++)
    {
      printk (BIOS_ERR, "before: %x\n", info->training.lane_timings[2][channel][slot][rank][lane]);
      info->training.lane_timings[2][channel][slot][rank][lane] = read_500 (info, channel, get_timing_register_addr (lane, 2, slot, rank), 9);
      printk (BIOS_ERR, "after: %x\n", info->training.lane_timings[2][channel][slot][rank][lane]);
      info->training.lane_timings[3][channel][slot][rank][lane] = info->training.lane_timings[2][channel][slot][rank][lane] + 0x20;
    }
}

static int
count_ranks_in_channel (struct raminfo *info, int channel)
{
  int slot, rank;
  int res = 0;
  for (slot = 0; slot < NUM_SLOTS; slot++)
    for (rank = 0; rank < NUM_SLOTS; rank++)
      res += info->populated_ranks[channel][slot][rank];
  return res;
}

static void
config_rank (struct raminfo *info, int s3resume, int channel, int slot, int rank)
{
  int add;

  write_1d0 (0, 0x178, 7, 1);
  seq9 (info, channel, slot, rank);
  program_timings (info, 0x80, channel, slot, rank);

  if (channel == 0)
    add = count_ranks_in_channel (info, 1);
  else
    add = 0;
  if (!s3resume)
    gav (rw_test (rank + add));
  program_timings (info, 0x00, channel, slot, rank);
  if (!s3resume)
    gav (rw_test (rank + add));
  if (!s3resume)
    gav (rw_test (rank + add));
  write_1d0 (0, 0x142, 3, 1);
  write_1d0 (0, 0x103, 6, 1);
	
  gav (get_580 (channel, 0xc | (rank << 5)));
  gav (read_1d0 (0x142, 3));
	
  write_mchbar8 (0x5ff, 0x0); /* OK */
  write_mchbar8 (0x5ff, 0x80); /* OK */
}

static void
set_4cf (struct raminfo *info, int channel, u8 val)
{
  gav (read_500 (info, channel, 0x4cf, 4)); // = 0xc2300cf9
  write_500 (info, channel, val, 0x4cf, 4, 1);
  gav (read_500 (info, channel, 0x659, 4)); // = 0x80300839
  write_500 (info, channel, val, 0x659, 4, 1);
  gav (read_500 (info, channel, 0x697, 4)); // = 0x80300839
  write_500 (info, channel, val, 0x697, 4, 1);
}

static void
set_334 (int zero)
{
  int j, k, channel;
  const u32 val3[] = { 0x2a2b2a2b, 0x26272627, 0x2e2f2e2f, 0x2a2b };
  u32 vd8[2][16];

  for (channel = 0; channel < NUM_CHANNELS; channel++)
    {
      for (j = 0; j < 4; j++)
	{
	  u32 a = (j == 1) ? 0x29292929 : 0x31313131;
	  u32 lmask = (j == 3) ? 0xffff : 0xffffffff;
	  u16 c;
	  if ((j == 0 || j == 3) && zero)
	    c = 0;
	  else if (j == 3)
	    c  = 0x5f;
	  else
	    c = 0x5f5f;

	  for (k = 0; k < 2; k++)
	    {
	      write_mchbar32 (0x138 + 8 * k, (channel << 26) | (j << 24));
	      gav (vd8[1][(channel << 3) | (j << 1) | k] = read_mchbar32 (0x138 + 8 * k));
	      gav (vd8[0][(channel << 3) | (j << 1) | k] = read_mchbar32 (0x13c + 8 * k));
	    }

	  write_mchbar32 (0x334 + (channel << 10) + (j * 0x44), zero ? 0 : val3[j]);
	  write_mchbar32 (0x32c + (channel << 10) + (j * 0x44), zero ? 0 : (0x18191819 & lmask));
	  write_mchbar16 (0x34a + (channel << 10) + (j * 0x44), c);
	  write_mchbar32 (0x33c + (channel << 10) + (j * 0x44), zero ? 0 : (a & lmask));
	  write_mchbar32 (0x344 + (channel << 10) + (j * 0x44), zero ? 0 : (a & lmask));
	}
    }

  write_mchbar32 (0x130, read_mchbar32 (0x130) | 1); /* OK */
  while (read_mchbar8 (0x130) & 1); /* OK */
}

static void
rmw_1d0 (u16 addr, u32 and, u32 or, int split, int flag)
{
  u32 v;
  v = read_1d0 (addr, split); 
  write_1d0 ((v & and) | or, addr, split, flag);
}

static int 
find_highest_bit_set (u16 val)
{
  int i;
  for (i = 15; i >= 0; i--)
    if (val & (1 << i))
      return i;
  return -1;
}

static int 
find_lowest_bit_set32 (u32 val)
{
  int i;
  for (i = 0; i < 32; i++)
    if (val & (1 << i))
      return i;
  return -1;
}

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

enum
  {
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

static void
calculate_timings (struct raminfo *info)
{
  unsigned cycletime;
  unsigned cas_latency_time;
  unsigned supported_cas_latencies;
  unsigned channel, slot;
  unsigned clock_speed_index;
  unsigned min_cas_latency;
  unsigned cas_latency;
  unsigned max_clock_index;

  /* Find common CAS latency  */
  supported_cas_latencies = 0x3fe;
  for (channel = 0; channel < NUM_CHANNELS; channel++)
    for (slot = 0; slot < NUM_SLOTS; slot++)
      if (info->populated_ranks[channel][slot][0])
	supported_cas_latencies &= 2 * (info->spd[channel][slot][CAS_LATENCIES_LSB] | (info->spd[channel][slot][CAS_LATENCIES_MSB] << 8));

  max_clock_index = min (3, info->max_supported_clock_speed_index);

  cycletime = min_cycletime[max_clock_index];
  cas_latency_time = min_cas_latency_time[max_clock_index];

  for (channel = 0; channel < NUM_CHANNELS; channel++)
    for (slot = 0; slot < NUM_SLOTS; slot++)
      if (info->populated_ranks[channel][slot][0])
	{
	  unsigned timebase;
	  timebase = 1000 * info->spd[channel][slot][TIMEBASE_DIVIDEND] / info->spd[channel][slot][TIMEBASE_DIVISOR];
	  cycletime = max (cycletime, timebase * info->spd[channel][slot][CYCLETIME]);
	  cas_latency_time = max (cas_latency_time, timebase * info->spd[channel][slot][CAS_LATENCY_TIME]);
	}
  for (clock_speed_index = 0; clock_speed_index < 3; clock_speed_index++)
    {
      if (cycletime == min_cycletime[clock_speed_index])
	break;
      if (cycletime > min_cycletime[clock_speed_index])
	{
	  clock_speed_index--;
	  cycletime = min_cycletime[clock_speed_index];
	  break;
	}
    }
  min_cas_latency = (cas_latency_time + cycletime - 1) / cycletime;
  cas_latency = 0;
  while (supported_cas_latencies)
    {
      cas_latency = find_highest_bit_set(supported_cas_latencies) + 3;
      if (cas_latency <= min_cas_latency)
        break;
      supported_cas_latencies &= ~(1 << find_highest_bit_set(supported_cas_latencies));
    }

  if (cas_latency != min_cas_latency && clock_speed_index)
    clock_speed_index--;

  if (cas_latency * min_cycletime[clock_speed_index] > 20000)
    die ("Couldn't configure DRAM");
  info->clock_speed_index = clock_speed_index;
  info->cas_latency = cas_latency;
}

#define s3oresume (s3resume && !REAL)

static void
program_base_timings (struct raminfo *info)
{
  unsigned channel;
  unsigned slot, rank, lane;
  unsigned extended_silicon_revision;
  int i;

  extended_silicon_revision = info->silicon_revision;
  if (info->silicon_revision == 0)
    for (channel = 0; channel < NUM_CHANNELS; channel++)
      for (slot = 0; slot < NUM_SLOTS; slot++)
	if ((info->spd[channel][slot][MODULE_TYPE] & 0xF) == 3)
	  extended_silicon_revision = 4;

  for (channel = 0; channel < NUM_CHANNELS; channel++)
    {
      for (slot = 0; slot < NUM_SLOTS; slot++)
	for (rank = 0; rank < NUM_SLOTS; rank++)
	  {
	    int card_timing_2;
	    if (!info->populated_ranks[channel][slot][rank])
	      continue;

	    for (lane = 0; lane < 9; lane++)
	      {
		int tm_reg;
		int card_timing;

		card_timing = 0;
		if ((info->spd[channel][slot][MODULE_TYPE] & 0xF) == 3)
		  {
		    int reference_card;
		    reference_card = info->spd[channel][slot][REFERENCE_RAW_CARD_USED] & 0x1f;
		    if (reference_card == 3)
		      card_timing = u16_ffd1188[0][lane][info->clock_speed_index];
		    if (reference_card == 5)
		      card_timing = u16_ffd1188[1][lane][info->clock_speed_index];
		  }

		info->training.lane_timings[0][channel][slot][rank][lane] = u8_FFFD1218[info->clock_speed_index];
		info->training.lane_timings[1][channel][slot][rank][lane] = 256;

		for (tm_reg = 2; tm_reg < 4; tm_reg++)
		  info->training.lane_timings[tm_reg][channel][slot][rank][lane] = u8_FFFD1240[channel][extended_silicon_revision][lane][2 * slot + rank][info->clock_speed_index]
		    + info->v4048[channel]
		    + u8_FFFD0C78[channel][extended_silicon_revision][info->v4030[channel]][slot][rank][info->clock_speed_index]
		    + card_timing;
		for (tm_reg = 0; tm_reg < 4; tm_reg++)
		    write_500 (info, channel, info->training.lane_timings[tm_reg][channel][slot][rank][lane],
			       get_timing_register_addr (lane, tm_reg, slot, rank),
			       9, 0);
	      }

	    card_timing_2 = 0;
	    if (!(extended_silicon_revision != 4
		  || (info->populated_ranks_mask[channel] & 5) == 5))
	      {
		if ((info->spd[channel][slot][REFERENCE_RAW_CARD_USED] & 0x1F) == 3)
		  card_timing_2 = u16_FFFE0EB8[0][info->clock_speed_index];
		if ((info->spd[channel][slot][REFERENCE_RAW_CARD_USED] & 0x1F) == 5)
		  card_timing_2 = u16_FFFE0EB8[1][info->clock_speed_index];
	      }

	    for (i = 0; i < 3; i++)
	      write_500 (info, channel, (card_timing_2 + info->v4048[channel]
				   + u8_FFFD0EF8[channel][extended_silicon_revision][info->v4030[channel]][info->clock_speed_index]),
			 u16_fffd0c50[i][slot][rank], 8, 1);
	    write_500 (info, channel, (info->v4048[channel] + u8_FFFD0C78[channel][extended_silicon_revision][info->v4030[channel]][slot][rank][info->clock_speed_index]),
		       u16_fffd0c70[slot][rank], 7, 1);
	  }
      if (!info->populated_ranks_mask[channel])
	continue;
      for (i = 0; i < 3; i++)
	write_500 (info, channel, (info->v4048[channel] + info->v4044[channel]
			     + u8_FFFD17E0[channel][extended_silicon_revision][info->v4030[channel]][info->clock_speed_index]),
		   u16_fffd0c68[i], 8, 1);
    }
}

/* The time of DDR transfer in ps.  */
static unsigned int
halfcycle_ps (struct raminfo *info)
{
  return 3750 / (info->clock_speed_index + 3);
}

/* The time of clock cycle in ps.  */
static unsigned int
cycle_ps (struct raminfo *info)
{
  return 2 * halfcycle_ps (info);
}

/* Frequency in 1.(1)=10/9 MHz units. */
static unsigned
frequency_11 (struct raminfo *info)
{
  return (info->clock_speed_index + 3) * 120;
}

/* Frequency in 0.1 MHz units. */
static unsigned
frequency_01 (struct raminfo *info)
{
  return 100 * frequency_11 (info) / 9;
}

static unsigned
ps_to_halfcycles (struct raminfo *info, unsigned int ps)
{
  return (frequency_11 (info) * 2) * ps / 900000;
}

static unsigned
ns_to_cycles (struct raminfo *info, unsigned int ns)
{
  return (frequency_11 (info)) * ns / 900;
}

static void
compute_derived_timings(struct raminfo *info)
{
  unsigned channel, slot, rank;
  int extended_silicon_revision;
  int some_delay_1_ps;
  int some_delay_2_ps;
  int some_delay_2_halfcycles_ceil;
  int some_delay_2_halfcycles_floor;
  int some_delay_3_ps;
  int some_delay_3_halfcycles;
  int some_delay_3_ps_rounded;
  int some_delay_1_cycle_ceil;
  int some_delay_1_cycle_floor;

  some_delay_3_halfcycles = 0;
  some_delay_3_ps_rounded = 0;
  extended_silicon_revision = info->silicon_revision;
  if (!info->silicon_revision)
    for (channel = 0; channel < NUM_CHANNELS; channel++)
      for (slot = 0; slot < NUM_SLOTS; slot++)
	if ((info->spd[channel][slot][MODULE_TYPE] & 0xF) == 3)
	  extended_silicon_revision = 4;
  if (info->board_lane_delay[7] < 5)
    info->board_lane_delay[7] = 5;
  info->u4044[0][0][0] = 2;
  if ( info->silicon_revision == 2 || info->silicon_revision == 3 )
    info->u4044[0][0][0] = 0;
  if ( info->revision < 0x10u )
    info->u4044[0][0][0] = 0;
  info->u4044[0][1][0] = 0;
  info->u4044[1][1][1] = 0;
  if (info->revision < 8)
    info->u4044[0][0][0] = 0;
  if (info->revision >= 8 && (info->silicon_revision == 0
			      || info->silicon_revision == 1))
    {
      some_delay_1_ps = 3929;
      some_delay_2_ps = 735;
    }
  else
    {
      some_delay_1_ps = 3490;
      some_delay_2_ps = 750;
    }
  some_delay_1_cycle_floor = some_delay_1_ps / cycle_ps (info);
  some_delay_1_cycle_ceil = some_delay_1_ps / cycle_ps (info);
  if (some_delay_1_ps % cycle_ps (info))
    some_delay_1_cycle_ceil++;
  else
    some_delay_1_cycle_floor--;
  info->u4044[0][1][1] = some_delay_1_cycle_floor;
  if (info->u4044[0][0][0])
    some_delay_2_ps = halfcycle_ps (info) >> 6;
  some_delay_2_ps += max (some_delay_1_ps - 30, 2 * halfcycle_ps (info) * (some_delay_1_cycle_ceil - 1) + 1000) + 375;
  some_delay_3_ps = halfcycle_ps (info) - some_delay_2_ps % halfcycle_ps (info);
  if (info->u4044[0][0][0])
    {
      if (some_delay_3_ps < 150)
	some_delay_3_halfcycles = 0;
      else
	some_delay_3_halfcycles = (some_delay_3_ps << 6) / halfcycle_ps (info);
      some_delay_3_ps_rounded = halfcycle_ps (info) * some_delay_3_halfcycles >> 6;
    }
  some_delay_2_halfcycles_ceil = (some_delay_2_ps + halfcycle_ps (info) - 1) / halfcycle_ps (info) - 2 * (some_delay_1_cycle_ceil - 1);
  if (info->u4044[0][0][0] && some_delay_3_ps < 150)
    some_delay_2_halfcycles_ceil++;
  some_delay_2_halfcycles_floor = some_delay_2_halfcycles_ceil;
  if (info->revision < 0x10)
    some_delay_2_halfcycles_floor = some_delay_2_halfcycles_ceil - 1;
  if (!info->u4044[0][0][0])
    ++some_delay_2_halfcycles_floor;
  info->u4044[1][0][0] = some_delay_2_halfcycles_ceil;
  info->u4044[1][0][1] = some_delay_3_halfcycles;
  info->u4044[1][1][0] = some_delay_3_ps_rounded;
  info->u4044[0][0][1] = some_delay_2_halfcycles_floor;
  if ((info->populated_ranks[0][0][0] && info->populated_ranks[0][1][0])
       || (info->populated_ranks[1][0][0] && info->populated_ranks[1][1][0]))
    info->max_slots_used_in_channel = 2;
  else
    info->max_slots_used_in_channel = 1;
  for (channel = 0; channel < 2; channel++)
    write_mchbar32 (0x244 + (channel << 10), ((info->revision < 8) ? 1 : 0x200)
		    | ((2 - info->max_slots_used_in_channel) << 17) | (channel << 21) | (info->u4044[0][1][1] << 18) | 0x9510);
  if (info->max_slots_used_in_channel == 1)
    {
      info->v4030[0] = (count_ranks_in_channel (info, 0) == 2); 
      info->v4030[1] = (count_ranks_in_channel (info, 1) == 2);
    }
  else
    {
      info->v4030[0] = ((count_ranks_in_channel (info, 0) == 1) || (count_ranks_in_channel (info, 0) == 2)) ? 2 : 3; /* 2 if 1 or 2 ranks */
      info->v4030[1] = ((count_ranks_in_channel (info, 1) == 1) || (count_ranks_in_channel (info, 1) == 2)) ? 2 : 3;
    }
  for (channel = 0; channel < NUM_CHANNELS; channel++)
    {
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
      for (i = 0; i < 3; i++)
	{
	  int unk1;
	  if (info->revision < 8)
	    unk1 = u8_FFFD1891[0][channel][info->clock_speed_index][i];
	  else if (!(info->revision >= 0x10 || info->u4044[0][0][0]))
	    unk1 = u8_FFFD1891[1][channel][info->clock_speed_index][i];
	  else
	    unk1 = 0;
	  for (slot = 0; slot < NUM_SLOTS; slot++)
	    for (rank = 0; rank < NUM_RANKS; rank++)
	      {
		int a = 0;
		int b = 0;

		if (!info->populated_ranks[channel][slot][rank])
		  continue;
		if ( extended_silicon_revision == 4 && (info->populated_ranks_mask[channel] & 5) != 5 )
		  {
		    if ( (info->spd[channel][slot][REFERENCE_RAW_CARD_USED] & 0x1F) == 3 )
		      {
			a = u16_ffd1178[0][info->clock_speed_index];
			b = u16_fe0eb8[0][info->clock_speed_index];
		      }
		    else if ( (info->spd[channel][slot][REFERENCE_RAW_CARD_USED] & 0x1F) == 5 )
		      {
			a = u16_ffd1178[1][info->clock_speed_index];
			b = u16_fe0eb8[1][info->clock_speed_index];
		      }
		  }
		min_of_unk_2 = min (min_of_unk_2, a);
		min_of_unk_2 = min (min_of_unk_2, b);
		if (rank == 0)
		  {
		    sum += a;
		    count++;
		  }
		{
		  int t;
		  t = b + u8_FFFD0EF8[channel][extended_silicon_revision][info->v4030[channel]][info->clock_speed_index];
		  if (unk1 >= t)
		    max_of_unk = max (max_of_unk, unk1 - t);
		}
	      }
	  {
	    int t = u8_FFFD17E0[channel][extended_silicon_revision][info->v4030[channel]][info->clock_speed_index] + min_of_unk_2;
	    if (unk1 >= t)
	      max_of_unk = max (max_of_unk, unk1 - t);
	  }
	}

      info->u4044[0][channel][0] = sum / count;
      info->u4044[1][channel][0] = max_of_unk;
    }
}

static void jedec_read(struct raminfo *info, 
		       int channel, int slot, int rank,
		       int total_rank, u8 addr3, unsigned int value)
{
  /* Handle mirrored mapping.  */
  if ((rank & 1) && (info->spd[channel][slot][RANK1_ADDRESS_MAPPING] & 1))
    addr3 = (addr3 & 0xCF) | ((addr3 & 0x10) << 1) | ((addr3 >> 1) & 0x10);
  write_mchbar8 (0x271, addr3 | (read_mchbar8 (0x271) & 0xC1));
  write_mchbar8 (0x671, addr3 | (read_mchbar8 (0x671) & 0xC1));

  /* Handle mirrored mapping.  */
  if ((rank & 1) && (info->spd[channel][slot][RANK1_ADDRESS_MAPPING] & 1))
    value = (value & ~0x1f8) | ((value >> 1) & 0xA8) | ((value & 0xA8) << 1);

  read32 ((value << 3) | (total_rank << 28));

  write_mchbar8 (0x271, (read_mchbar8 (0x271) & 0xC3) | 2);
  write_mchbar8 (0x671, (read_mchbar8 (0x671) & 0xC3) | 2);

  read32 (total_rank << 28);
}

enum
  {
    MR1_RZQ12 = 512,
    MR1_RZQ2 = 64,
    MR1_RZQ4 = 4,
    MR1_ODS34OHM  = 2
  };

enum
  {
    MR0_BT_INTERLEAVED = 8,
    MR0_DLL_RESET_ON = 256
  };

enum
  {
    MR2_RTT_WR_DISABLED = 0,
    MR2_RZQ2 = 1 << 10
  };

static void 
jedec_init (struct raminfo *info)
{
  int write_recovery;
  int channel, slot, rank;
  int total_rank;
  int dll_on;
  int self_refresh_temperature;
  int auto_self_refresh;

  auto_self_refresh = 1;
  self_refresh_temperature = 1;
  if (info->board_lane_delay[3] <= 10)
    {
      if (info->board_lane_delay[3] <= 8)
	write_recovery = info->board_lane_delay[3] - 4;
      else
	write_recovery = 5;
    }
  else
    {
      write_recovery = 6;
    }
  for (channel = 0; channel < NUM_CHANNELS; channel++)
    for (slot = 0; slot < NUM_CHANNELS; slot++)
      for (rank = 0; rank < NUM_RANKS; rank++)
	if (info->populated_ranks[channel][slot][rank])
	  {
	    auto_self_refresh &= (info->spd[channel][slot][THERMAL_AND_REFRESH] >> 2) & 1;
	    self_refresh_temperature &= info->spd[channel][slot][THERMAL_AND_REFRESH] & 1;
	  }
  if (auto_self_refresh == 1)
    self_refresh_temperature = 0;

  dll_on = ((info->silicon_revision != 2 && info->silicon_revision != 3)
		 || (info->populated_ranks[0][0][0] && info->populated_ranks[0][1][0])
		 || (info->populated_ranks[1][0][0] && info->populated_ranks[1][1][0]));

  total_rank = 0;

  for (channel = NUM_CHANNELS - 1; channel >= 0; channel--) 
    {
      int rtt, rtt_wr = MR2_RTT_WR_DISABLED;
      int rzq_reg58e;

      if (info->silicon_revision == 2 || info->silicon_revision == 3)
	{
	  rzq_reg58e = 64;
	  rtt = MR1_RZQ2;
	  if (info->clock_speed_index != 0)
	    {
	      rzq_reg58e = 4;
	      if (info->populated_ranks_mask[channel] == 3)
		rtt = MR1_RZQ4;
	    }
	}
      else
	{
	  if ((info->populated_ranks_mask[channel] & 5) == 5)
	    {
	      rtt = MR1_RZQ12;
	      rzq_reg58e = 64;
	      rtt_wr = MR2_RZQ2;
	    }
	  else
	    {
	      rzq_reg58e = 4;
	      rtt = MR1_RZQ4;
	    }
	}

      write_mchbar16 (0x588 + (channel << 10), 0x0);
      write_mchbar16 (0x58a + (channel << 10), 0x4);
      write_mchbar16 (0x58c + (channel << 10), rtt | MR1_ODS34OHM);
      write_mchbar16 (0x58e + (channel << 10), rzq_reg58e | 0x82);
      write_mchbar16 (0x590 + (channel << 10), 0x1282);

      for (slot = 0; slot < NUM_SLOTS; slot++)
	for (rank = 0; rank < NUM_RANKS; rank++)
	  if (info->populated_ranks[channel][slot][rank])
	    {
	      jedec_read (info, channel, slot, rank,
			 total_rank, 0x28,
			  rtt_wr | (info->clock_speed_index << 3)
			  | (auto_self_refresh << 6) | (self_refresh_temperature << 7));
	      jedec_read (info, channel, slot, rank, total_rank, 0x38, 0);
	      jedec_read (info, channel, slot, rank,
			  total_rank, 0x18, rtt | MR1_ODS34OHM);
	      jedec_read (info, channel, slot, rank,
			  total_rank, 6,
			  (dll_on << 12) | (write_recovery << 9)
			  | ((info->cas_latency - 4) << 4) | MR0_BT_INTERLEAVED | MR0_DLL_RESET_ON);
	      total_rank++;
	    }
    }
}

static void pm_wait (u16 us)
{
  u32 base = read_acpi32 (8);
  u32 ticks = (us * 358) / 100;
  while (((read_acpi32 (8) - base) & 0xffffff) < ticks);
}

static void program_modules_memory_map (struct raminfo *info, int pre_jedec)
{
  unsigned channel, slot, rank;
  unsigned int total_mb[2] = { 0, 0 }; /* total memory per channel in MB */
  unsigned int channel_0_non_interleaved;

  for (channel = 0; channel < NUM_CHANNELS; channel++)
    for (slot = 0; slot < NUM_SLOTS; slot++)
      for (rank = 0; rank < NUM_RANKS; rank++)
	{
	  if (info->populated_ranks[channel][slot][rank])
	    {
	      total_mb[channel] += pre_jedec ? 256 : (256 << info->density[channel][slot] >> info->is_x16_module[channel][slot]);
#if REAL
	      printk (BIOS_ERR, "cap = %d MiB\n", 256 << info->density[channel][slot] >> info->is_x16_module[channel][slot]);
#endif
	      write_mchbar8 (0x208 + rank +  2 * slot + (channel << 10), 
			     (pre_jedec ? (1 | ((1 + 1) << 1))  : (info->is_x16_module[channel][slot] | ((info->density[channel][slot] + 1) << 1))) | 0x80);
	    }
	  write_mchbar16 (0x200 + (channel << 10) + 4 * slot + 2 * rank, total_mb[channel] >> 6);
	}

  info->total_memory_mb = total_mb[0] + total_mb[1];

  info->interleaved_part_mb = pre_jedec ? 0 : 2 * min (total_mb[0], total_mb[1]);
  info->non_interleaved_part_mb = total_mb[0] + total_mb[1] - info->interleaved_part_mb;
  channel_0_non_interleaved = total_mb[0] - info->interleaved_part_mb / 2;
#if REAL
  printk (BIOS_ERR, "%s: %x, %x\n", (pre_jedec ? "pre-JEDEC" : "JEDEC"), channel_0_non_interleaved | (info->non_interleaved_part_mb << 16), info->interleaved_part_mb);
#endif
  write_mchbar32 (0x100, channel_0_non_interleaved | (info->non_interleaved_part_mb << 16));
  if (!pre_jedec)
    write_mchbar16 (0x104, info->interleaved_part_mb);
}

static void program_board_delay(struct raminfo *info)
{
  int cas_latency_shift;
  int some_delay_ns;
  int some_delay_3_half_cycles;

  unsigned channel, i;
  int high_multiplier;
  int lane_3_delay;
  int cas_latency_derived;

  high_multiplier = 0;
  some_delay_ns = 200;
  some_delay_3_half_cycles = 4;
  cas_latency_shift = info->silicon_revision == 0 || info->silicon_revision == 1 ? 1 : 0;
  if (info->revision < 8)
    {
      some_delay_ns = 600;
      cas_latency_shift = 0;
    }
  {
    int speed_bit;
    speed_bit = ((info->clock_speed_index > 1 || (info->silicon_revision != 2 && info->silicon_revision != 3))) ^ (info->revision >= 0x10);
    write_500 (info, 0, speed_bit | ((!info->use_ecc) << 1), 0x60e, 3, 1);
    write_500 (info, 1, speed_bit | ((!info->use_ecc) << 1), 0x60e, 3, 1);
    if (info->revision >= 0x10 && info->clock_speed_index <= 1 && (info->silicon_revision == 2 || info->silicon_revision == 3))
      rmw_1d0 (0x116, 5, 2, 4, 1);
  }
  write_mchbar32 (0x120, (1 << (info->max_slots_used_in_channel + 28)) | 0x188e7f9f);

  write_mchbar8 (0x124, info->board_lane_delay[4] + ((frequency_01 (info) + 999) / 1000)); 
  write_mchbar16 (0x125, 0x1360);
  write_mchbar8 (0x127, 0x40);
  if ((info->some_base_frequency >> 4) < frequency_11 (info) / 2)
    {
      unsigned some_delay_2_half_cycles;
      high_multiplier = 1;
      some_delay_2_half_cycles = ps_to_halfcycles (info, 
						   ((3 * 900000 / (info->some_base_frequency >> 4)) >> 1) + (halfcycle_ps (info) * lut4041[info->clock_speed_index] >> 6)
						   + 4 * halfcycle_ps (info)
						   + 2230);
      some_delay_3_half_cycles = min ((some_delay_2_half_cycles + (frequency_11 (info) * 2) * (28 - some_delay_2_half_cycles) /
				       (frequency_11 (info) * 2 - 4 * (info->some_base_frequency >> 4))) >> 3, 7);
    }
  if (read_mchbar8 (0x2ca9) & 1)
    some_delay_3_half_cycles = 3;
  for (channel = 0; channel < NUM_CHANNELS; channel++)
    {
      write_mchbar32 (0x220 + (channel << 10), read_mchbar32 (0x220 + (channel << 10)) | 0x18001117);
      write_mchbar32 (0x224 + (channel << 10), (info->max_slots_used_in_channel - 1)
		      | ((info->cas_latency
			  - 5 - info->clock_speed_index) << 21)
		      | ((info->max_slots_used_in_channel + info->cas_latency - cas_latency_shift - 4) << 16)
		      | ((info->cas_latency - cas_latency_shift - 4) << 26)
		      | ((info->cas_latency - info->clock_speed_index + info->max_slots_used_in_channel - 6) << 8));
      write_mchbar32 (0x228 + (channel << 10), info->max_slots_used_in_channel);
      write_mchbar8 (0x239 + (channel << 10), 32);
      write_mchbar32 (0x248 + (channel << 10), (high_multiplier << 24) | (some_delay_3_half_cycles << 25) | 0x840000);
      write_mchbar32 (0x278 + (channel << 10), 0xc362042);
      write_mchbar32 (0x27c + (channel << 10), 0x8b000062);
      write_mchbar32 (0x24c + (channel << 10), ((!!info->clock_speed_index) << 17) | (((2 + info->clock_speed_index - (!!info->clock_speed_index))) << 12) | 0x10200);

      write_mchbar8 (0x267 + (channel << 10), 0x4);
      write_mchbar16 (0x272 + (channel << 10), 0x155);
      write_mchbar32 (0x2bc + (channel << 10),
		      (read_mchbar32 (0x2bc + (channel << 10)) & 0xFF000000)
		      | 0x707070);

      write_500 (info, channel,
		 ((!info->populated_ranks[channel][1][1])
		  | (!info->populated_ranks[channel][1][0] << 1)
		  | (!info->populated_ranks[channel][0][1] << 2)
		  | (!info->populated_ranks[channel][0][0] << 3)),
		 0x4c9, 4, 1);
    }

  write_mchbar8 (0x2c4, ((1 + (info->clock_speed_index != 0)) << 6) | 0xC);
  {
    u8 freq_divisor = 2; 
    if ((info->some_base_frequency >> 4) == frequency_11 (info))
      freq_divisor = 3;
    else if ( (info->some_base_frequency >> 3) < 3 * (frequency_11 (info) / 2) )
      freq_divisor = 1;
    else
      freq_divisor = 2;
    write_mchbar32 (0x2c0, (freq_divisor << 11) | 0x6009c400);
  }

  if ( info->board_lane_delay[3] <= 10 )
    {
      if ( info->board_lane_delay[3] <= 8 )
	lane_3_delay = info->board_lane_delay[3];
      else
	lane_3_delay = 10;
    }
  else
    {
      lane_3_delay = 12;
    }
  cas_latency_derived = info->cas_latency - info->clock_speed_index + 2;
  if (info->clock_speed_index > 1)
    cas_latency_derived++;
  for (channel = 0; channel < NUM_CHANNELS; channel++)
    {
      write_mchbar32 (0x240 + (channel << 10), ((info->clock_speed_index == 0) * 0x11000) | 0x1002100 | ((2 + info->clock_speed_index) << 4) | (info->cas_latency - 3));
      write_500 (info, channel, (info->clock_speed_index << 1) | 1, 0x609, 6, 1);
      write_500 (info, channel, info->clock_speed_index + 2 * info->cas_latency - 7, 0x601, 6, 1);
		
      write_mchbar32 (0x250 + (channel << 10), ((lane_3_delay + info->clock_speed_index + 9) << 6)
		      | (info->board_lane_delay[7] << 2) | (info->board_lane_delay[4] << 16) | (info->board_lane_delay[1] << 25) | (info->board_lane_delay[1] << 29) | 1);
      write_mchbar32 (0x254 + (channel << 10), (info->board_lane_delay[1] >> 3) | ((info->board_lane_delay[8] + 4 * info->use_ecc) << 6) | 0x80 |
		      (info->board_lane_delay[6] << 1) | (info->board_lane_delay[2] << 28) | (cas_latency_derived << 16) | 0x4700000);
      write_mchbar32 (0x258 + (channel << 10), ((info->board_lane_delay[5] + info->clock_speed_index + 9) << 12) | ((info->clock_speed_index - info->cas_latency + 12) << 8)
		      | (info->board_lane_delay[2] << 17) | (info->board_lane_delay[4] << 24) | 0x47);
      write_mchbar32 (0x25c + (channel << 10), (info->board_lane_delay[1] << 1) | (info->board_lane_delay[0] << 8) | 0x1DA50000);
      write_mchbar8 (0x264 + (channel << 10), 0xff);
      write_mchbar8 (0x5f8 + (channel << 10), (cas_latency_shift << 3) | info->use_ecc);
    }

  program_modules_memory_map (info, 1);

  write_mchbar16 (0x610, (min (ns_to_cycles (info, some_delay_ns) / 2, 127) << 9)
		  | (read_mchbar16(0x610) & 0x1C3) | 0x3C);
  write_mchbar16 (0x612, read_mchbar16(0x612) | 0x100);
  write_mchbar16 (0x214, read_mchbar16 (0x214) | 0x3E00);
  for (i = 0; i < 8; i++)
    {
      pci_mm_write32 (QUICKPATH_BUS, 0, 1, 0x80 + 4 * i, (info->total_memory_mb - 64) | !i | 2);
      pci_mm_write32 (QUICKPATH_BUS, 0, 1, 0xc0 + 4 * i, 0);
    }
}

#define ALIGN_DOWN(addr, align) \
	((addr) & ~((typeof (addr)) align - 1))
#define ALIGN_UP(addr, align) \
	((addr + (typeof (addr)) align - 1) & ~((typeof (addr)) align - 1))

#define BETTER_MEMORY_MAP 0

static void program_total_memory_map(struct raminfo *info)
{
  unsigned int TOM, TOLUD, TOUUD;
  unsigned int quickpath_reserved;
  unsigned int REMAPbase;
  unsigned int uma_base_igd;
  unsigned int uma_base_gtt;
  int memory_remap;
  unsigned int memory_map[8];
  int i;
  unsigned int current_limit;
  unsigned int tseg_base;
  int uma_size_igd = 0, uma_size_gtt = 0;

  memset (memory_map, 0, sizeof (memory_map));

#if REAL
  if (info->uma_enabled)
    {
      u16 t = pci_mm_read16 (NORTHBRIDGE, D0F0_GGC);
      gav (t);
      const int uma_sizes_gtt[16] = { 0, 1, 0, 2, 0, 0, 0, 0, 0, 2, 3, 4, 42, 42, 42, 42 };
      /* Igd memory */
      const int uma_sizes_igd[16] = 
	{
	  0, 0, 0, 0, 0, 32, 48, 64, 128, 256, 96, 160, 224, 352, 256, 512 
	};

      uma_size_igd = uma_sizes_igd[(t >> 4) & 0xF];
      uma_size_gtt = uma_sizes_gtt[(t >> 8) & 0xF];
    }
#endif

  TOM = info->total_memory_mb;
  if (TOM == 4096)
    TOM = 4032;
  TOUUD = ALIGN_DOWN (TOM - info->memory_reserved_for_heci_mb, 64);
  TOLUD = ALIGN_DOWN (min (3072
			   + ALIGN_UP(uma_size_igd + uma_size_gtt, 64)
			   , TOUUD), 64);
  memory_remap = 0;
  if (TOUUD - TOLUD > 64)
    {
      memory_remap = 1;
      REMAPbase = max (4096, TOUUD);
      TOUUD = TOUUD - TOLUD + 4096;
    }
  if (TOUUD > 4096)
    memory_map[2] = TOUUD | 1;
  quickpath_reserved = 0;

  {
    u32 t;

    gav (t = pci_mm_read32 (QUICKPATH_BUS, 0, 1, 0x68));
    if (t & 0x800)
      quickpath_reserved = (1 << find_lowest_bit_set32 (t >> 20));
  }
  if (memory_remap)
    TOUUD -= quickpath_reserved;

#if !REAL
  if (info->uma_enabled)
    {
      u16 t = pci_mm_read16 (NORTHBRIDGE, D0F0_GGC);
      gav (t);
      const int uma_sizes_gtt[16] = { 0, 1, 0, 2, 0, 0, 0, 0, 0, 2, 3, 4, 42, 42, 42, 42 };
      /* Igd memory */
      const int uma_sizes_igd[16] = 
	{
	  0, 0, 0, 0, 0, 32, 48, 64, 128, 256, 96, 160, 224, 352, 256, 512 
	};

      uma_size_igd = uma_sizes_igd[(t >> 4) & 0xF];
      uma_size_gtt = uma_sizes_gtt[(t >> 8) & 0xF];
    }
#endif

  uma_base_igd = TOLUD - uma_size_igd;
  uma_base_gtt = uma_base_igd - uma_size_gtt;
  tseg_base = ALIGN_DOWN (uma_base_gtt, 64) - (CONFIG_SMM_TSEG_SIZE >> 20);
  if (!memory_remap)
    tseg_base -= quickpath_reserved;
  tseg_base = ALIGN_DOWN (tseg_base, 8);

  printk (BIOS_ERR, "TOLUD=%d, TOUUD=%d\n", TOLUD, TOUUD);
  printk (BIOS_ERR, "tseg_base=%x\n", tseg_base);

  pci_mm_write16 (NORTHBRIDGE, D0F0_TOLUD, TOLUD << 4);
  pci_mm_write16 (NORTHBRIDGE, D0F0_TOM, TOM >> 6);
  if (memory_remap)
    {
      pci_mm_write16 (NORTHBRIDGE, D0F0_REMAPBASE, REMAPbase >> 6);
      pci_mm_write16 (NORTHBRIDGE, D0F0_REMAPLIMIT, (TOUUD - 64) >> 6);
    }
  pci_mm_write16 (NORTHBRIDGE, D0F0_TOUUD, TOUUD);

  if (info->uma_enabled)
    {
      pci_mm_write32 (NORTHBRIDGE, D0F0_IGD_BASE, uma_base_igd << 20);
      pci_mm_write32 (NORTHBRIDGE, D0F0_GTT_BASE, uma_base_gtt << 20);
    }
  pci_mm_write32 (NORTHBRIDGE, TSEG, tseg_base << 20);

  current_limit = 0;
  memory_map[0] = ALIGN_DOWN (uma_base_gtt, 64) | 1;
  memory_map[1] = 4096;
  for (i = 0; i < ARRAY_SIZE (memory_map); i++)
    {
      current_limit = max (current_limit, memory_map[i] & ~1);
      pci_mm_write32 (QUICKPATH_BUS, 0, 1, 4 * i + 0x80, (memory_map[i] & 1) | ALIGN_DOWN (current_limit - 1, 64) | 2);
      pci_mm_write32 (QUICKPATH_BUS, 0, 1, 4 * i + 0xc0, 0);
    }
}

static void
collect_system_info (struct raminfo *info)
{
  u32 capid0[3];
  int i;
  unsigned channel;

  /* Wait for some bit, maybe TXT clear. */
  while (!(read8(0xfed40000) & (1 << 7))) ;

  if (!info->heci_bar)
    gav (info->heci_bar = pci_mm_read32 (HECIDEV, HECIBAR) & 0xFFFFFFF8);
  if (!info->memory_reserved_for_heci_mb)
    {
      /* Wait for ME to be ready */
      intel_early_me_init ();
      info->memory_reserved_for_heci_mb = intel_early_me_uma_size ();
    }

  for (i = 0; i < 3; i++)
    gav (capid0[i] = pci_mm_read32 (NORTHBRIDGE, D0F0_CAPID0 | (i << 2)));
  gav (info->revision = pci_mm_read8 (NORTHBRIDGE, PCI_REVISION_ID));
  info->max_supported_clock_speed_index = (~capid0[1] & 7);

  if ((capid0[1] >> 11) & 1)
    info->uma_enabled = 0;
  else
    gav (info->uma_enabled = pci_mm_read8 (NORTHBRIDGE, D0F0_DEVEN) & 8);
  /* Unrecognised: [0000:fffd3d2d] 37f81.37f82 ! CPUID: eax: 00000001; ecx: 00000e00 => 00020655.00010800.029ae3ff.bfebfbff*/
  info->silicon_revision = 0;

  if (capid0[2] & 2)
    {
      info->silicon_revision = 0;
      info->max_supported_clock_speed_index = 2;
      for (channel = 0; channel < NUM_CHANNELS; channel++)
	if (info->populated_ranks[channel][0][0] && (info->spd[channel][0][MODULE_TYPE] & 0xf) == 3)
	  {
	    info->silicon_revision = 2;
	    info->max_supported_clock_speed_index = 1;
	  }
    }
  else
    {
      switch (((capid0[2] >> 18) & 1) + 2 * ((capid0[1] >> 3) & 1) )
	{
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
      switch (pci_mm_read16 (NORTHBRIDGE, PCI_DEVICE_ID))
	{
	case 0x40:
	  info->silicon_revision = 0;
	  break;
	case 0x48:
	  info->silicon_revision = 1;
	  break;
	}
    }
}

static void 
enable_hpet (void)
{
#if REAL
  u32 reg32;

  /* Move HPET to default address 0xfed00000 and enable it */
  reg32 = RCBA32(HPTC);
  reg32 |= (1 << 7); // HPET Address Enable
  reg32 &= ~(3 << 0);
  RCBA32(HPTC) = reg32;
#else
  write32 (DEFAULT_RCBA | HPTC, 0x80);
#endif
}

static void
dump_timings (struct raminfo *info)
{
#if REAL
  int channel, slot, rank, lane, i;
  printk (BIOS_DEBUG, "Timings:\n");
  for (channel = 0; channel < NUM_CHANNELS; channel++)
    for (slot = 0; slot < NUM_SLOTS; slot++)
      for (rank = 0; rank < NUM_RANKS; rank++)
	{
	  if (!info->populated_ranks[channel][slot][rank])
	    continue;
	  printk (BIOS_DEBUG, "channel %d, slot %d, rank %d\n", channel, slot, rank);
	  for (lane = 0; lane < 9; lane++)
	    {
	      printk (BIOS_DEBUG, "lane %d: ", lane);
	      for (i = 0; i < 4; i++)
		{
		  printk (BIOS_DEBUG, "%x (%x) ", read_500_bypass (info, channel,
								   get_timing_register_addr (lane, i, slot, rank), 9),
			  info->training.lane_timings[i][channel][slot][rank][lane]);
		}
	      printk (BIOS_DEBUG, "\n");
	    }
	}
  printk (BIOS_ERR, "[178] = %x (%x)\n", read_1d0 (0x178, 7), info->training.reg_178);
  printk (BIOS_ERR, "[10b] = %x (%x)\n", read_1d0 (0x10b, 6), info->training.reg_10b);
#endif
}

static void
save_timings (struct raminfo *info)
{
#if CONFIG_EARLY_CBMEM_INIT
        struct ram_training train;
	struct mrc_data_container *mrcdata;
	int output_len = ALIGN(sizeof (train), 16);
	int channel, slot, rank, lane, i;

	memset (&train, 0, sizeof (train));
	for (channel = 0; channel < NUM_CHANNELS; channel++)
	  for (slot = 0; slot < NUM_SLOTS; slot++)
	    for (rank = 0; rank < NUM_RANKS; rank++)
	      if (info->populated_ranks[channel][slot][rank])
		for (lane = 0; lane < 9; lane++)
		  for (i = 0; i < 4; i++)
		    train.lane_timings[i][channel][slot][rank][lane] = read_500 (info, channel,
										 get_timing_register_addr (lane, i, slot, rank), 9);
	info->training.reg_178 = read_1d0 (0x178, 7);
	info->training.reg_10b = read_1d0 (0x10b, 6);

	/* Save the MRC S3 restore data to cbmem */
	cbmem_initialize();
	mrcdata = cbmem_add
		(CBMEM_ID_MRCDATA,
		 output_len + sizeof(struct mrc_data_container));

	printk(BIOS_DEBUG, "Relocate MRC DATA from %p to %p (%u bytes)\n",
	       &train, mrcdata, output_len);

	mrcdata->mrc_signature = MRC_DATA_SIGNATURE;
	mrcdata->mrc_data_size = output_len;
	mrcdata->reserved = 0;
	memcpy(mrcdata->mrc_data, &train,
	       sizeof (train));

	/* Zero the unused space in aligned buffer. */
	if (output_len > sizeof (train))
		memset(mrcdata->mrc_data+sizeof (train), 0,
		       output_len - sizeof (train));

	mrcdata->mrc_checksum = compute_ip_checksum(mrcdata->mrc_data,
						    mrcdata->mrc_data_size);
#endif
}

#if REAL
static const struct ram_training *
get_cached_training (void)
{
  struct mrc_data_container *cont;
  cont = find_current_mrc_cache();
  if (!cont)
    return 0;
  return (void *) cont->mrc_data;
}
#endif

/* FIXME: add timeout.  */
static void
wait_heci_ready (void)
{
  while (!(read32 (DEFAULT_HECIBAR | 0xc) & 8)); // = 0x8000000c
  write32 ((DEFAULT_HECIBAR | 0x4), (read32 (DEFAULT_HECIBAR | 0x4) & ~0x10) | 0xc);
}

/* FIXME: add timeout.  */
static void
wait_heci_cb_avail (int len)
{
  union
  {
    struct mei_csr csr;
    u32 raw;
  } csr;

  while (!(read32 (DEFAULT_HECIBAR | 0xc) & 8));

  printk (BIOS_ERR, "alive "__FILE__ ":%d\n", __LINE__);

  do
    csr.raw = read32 (DEFAULT_HECIBAR | 0x4);
  while (len > csr.csr.buffer_depth - (csr.csr.buffer_write_ptr - csr.csr.buffer_read_ptr));

  printk (BIOS_ERR, "alive "__FILE__ ":%d\n", __LINE__);

}

static void
send_heci_packet (struct mei_header *head, u32 *payload)
{
  int len = (head->length + 3) / 4;
  int i;

  printk (BIOS_ERR, "alive "__FILE__ ":%d\n", __LINE__);

  wait_heci_cb_avail (len + 1);

  printk (BIOS_ERR, "alive "__FILE__ ":%d\n", __LINE__);

  /* FIXME: handle leftovers correctly.  */
  write32 (DEFAULT_HECIBAR | 0, *(u32 *)head);
  for (i = 0; i < len - 1; i++)
    write32 (DEFAULT_HECIBAR | 0, payload[i]);
  printk (BIOS_ERR, "alive "__FILE__ ":%d\n", __LINE__);

  write32 (DEFAULT_HECIBAR | 0, payload[i] & ((1 << (8 * len)) - 1));
  printk (BIOS_ERR, "alive "__FILE__ ":%d\n", __LINE__);

  write32 (DEFAULT_HECIBAR | 0x4, read32 (DEFAULT_HECIBAR | 0x4) | 0x4);
  printk (BIOS_ERR, "alive "__FILE__ ":%d\n", __LINE__);

}

static void
send_heci_message (u8 *msg, int len, u8 hostaddress, u8 clientaddress)
{
  struct mei_header head;
  int maxlen;
  printk (BIOS_ERR, "alive "__FILE__ ":%d\n", __LINE__);

  wait_heci_ready ();

  printk (BIOS_ERR, "alive "__FILE__ ":%d\n", __LINE__);

  maxlen = (read32 (DEFAULT_HECIBAR | 0x4) >> 24) * 4 - 4;
  printk (BIOS_ERR, "alive "__FILE__ ":%d\n", __LINE__);

  while (len)
    {
      int cur = len;
      if (cur > maxlen)
	{
	  cur = maxlen;
	  head.is_complete = 0;
	}
      else
	head.is_complete = 1;
      head.length = cur;
      head.reserved = 0;
      head.client_address = clientaddress;
      head.host_address = hostaddress;
      send_heci_packet (&head, (u32 *) msg);
      len -= cur;
      msg += cur;
    }
}

/* FIXME: Add timeout.  */
static int
recv_heci_packet(struct raminfo *info, struct mei_header *head, u32 *packet, u32 *packet_size)
{
  union
  {
    struct mei_csr csr;
    u32 raw;
  } csr;
  int i = 0;

  write32 (DEFAULT_HECIBAR | 0x4, read32 (DEFAULT_HECIBAR | 0x4) | 2);
  do
    {
      csr.raw = read32 (DEFAULT_HECIBAR | 0xc);
#if !REAL
      if (i++ > 347)
	return -1;
#endif
    }
  while (csr.csr.buffer_write_ptr == csr.csr.buffer_read_ptr);
  *(u32 *)head = read32 (DEFAULT_HECIBAR | 0x8);
  if (!head->length)
    {
      write32 (DEFAULT_HECIBAR | 0x4, read32 (DEFAULT_HECIBAR | 0x4) | 2);
      *packet_size = 0;
      return 0;
    }
  if (head->length + 4 > 4 * csr.csr.buffer_depth || head->length > *packet_size)
    {
      *packet_size = 0;
      return -1;
    }

  do
    csr.raw = read32 (DEFAULT_HECIBAR | 0xc);
  while ((head->length + 3) >> 2 > csr.csr.buffer_write_ptr - csr.csr.buffer_read_ptr);

  for (i = 0; i < (head->length + 3) >> 2; i++)
    packet[i++] = read32 (DEFAULT_HECIBAR | 0x8);
  *packet_size = head->length;
  if (!csr.csr.ready)
    *packet_size = 0;
  write32 (DEFAULT_HECIBAR | 0x4, read32 (DEFAULT_HECIBAR | 0x4) | 4);
  return 0;
}

/* FIXME: Add timeout.  */
static int
recv_heci_message(struct raminfo *info, u32 *message, u32 *message_size)
{
  struct mei_header head;
  int current_position;

  current_position = 0;
  while (1)
  {
    u32 current_size;
    current_size = *message_size - current_position;
    if (recv_heci_packet(info, &head, message + (current_position >> 2), &current_size) == -1 )
      break;
    if (!current_size)
      break;
    current_position += current_size;
    if (head.is_complete)
      {
	*message_size = current_position;
	return 0;
      }

    if (current_position >= *message_size)
      break;
  }
  *message_size = 0;
  return -1;
}

static void
send_heci_uma_message(struct raminfo *info)
{
  struct uma_reply
  {
    u8 group_id;
    u8 command;
    u8 reserved;
    u8 result;
    u8 field2;
    u8 unk3[0x48 - 4 - 1];
  }__attribute__ ((packed)) reply;
  struct uma_message
  {
    u8 group_id;
    u8 cmd;
    u8 reserved;
    u8 result;
    u32 c2;
    u64 heci_uma_addr;
    u32 memory_reserved_for_heci_mb;
    u16 c3;
  } __attribute__ ((packed)) msg = {
    0, MKHI_SET_UMA, 0, 0,
    0x82,
    info->heci_uma_addr,
    info->memory_reserved_for_heci_mb,
    0
  };
  u32 reply_size;

  printk (BIOS_ERR, "alive "__FILE__ ":%d\n", __LINE__);

  send_heci_message ((u8 *) &msg, sizeof (msg), 0, 7);

  printk (BIOS_ERR, "alive "__FILE__ ":%d\n", __LINE__);

  reply_size = sizeof (reply);
  if (recv_heci_message (info, (u32 *) &reply, &reply_size) == -1)
    return;
  printk (BIOS_ERR, "alive "__FILE__ ":%d\n", __LINE__);

  if (reply.command == (MKHI_SET_UMA | (1 << 7)))
    info->v4086 = reply.field2;
  else
    die ("HECI init failed\n");
  printk (BIOS_ERR, "alive "__FILE__ ":%d v4086=%x\n\n", __LINE__, info->v4086);
}

static void
setup_heci_uma (struct raminfo *info)
{
  u32 reg44;

  /* trouble start */
  reg44 = pci_mm_read32 (HECIDEV, 0x44);  // = 0x80010020
  info->memory_reserved_for_heci_mb = 0;
  info->heci_uma_addr = 0;
  if (!((reg44 & 0x10000) && !(pci_mm_read32 (HECIDEV, 0x40) & 0x20)))
    return;

  info->heci_bar =  pci_mm_read32 (HECIDEV, 0x10) & 0xFFFFFFF0;
  info->memory_reserved_for_heci_mb = reg44 & 0x3f;
  info->heci_uma_addr = ((u64)((((u64) pci_mm_read16 (NORTHBRIDGE, D0F0_TOM)) << 6) - info->memory_reserved_for_heci_mb)) << 20;

  pci_mm_read32 (NORTHBRIDGE, DMIBAR);
  if (info->memory_reserved_for_heci_mb)
    {
      write32 (DEFAULT_DMIBAR | 0x14, read32 (DEFAULT_DMIBAR | 0x14) & ~0x80);
      write32 (DEFAULT_RCBA | 0x14, read32 (DEFAULT_RCBA | 0x14) & ~0x80);
      write32 (DEFAULT_DMIBAR | 0x20, read32 (DEFAULT_DMIBAR | 0x20) & ~0x80);
      write32 (DEFAULT_RCBA | 0x20, read32 (DEFAULT_RCBA | 0x20) & ~0x80);
      write32 (DEFAULT_DMIBAR | 0x2c, read32 (DEFAULT_DMIBAR | 0x2c) & ~0x80);
      write32 (DEFAULT_RCBA | 0x30, read32 (DEFAULT_RCBA | 0x30) & ~0x80);
      write32 (DEFAULT_DMIBAR | 0x38, read32 (DEFAULT_DMIBAR | 0x38) & ~0x80);
      write32 (DEFAULT_RCBA | 0x40, read32 (DEFAULT_RCBA | 0x40) & ~0x80);

      write32 (DEFAULT_RCBA | 0x40, 0x87000080); // OK
      write32 (DEFAULT_DMIBAR | 0x38, 0x87000080); // OK
      while (read16 (DEFAULT_RCBA | 0x46) & 2 && read16 (DEFAULT_DMIBAR | 0x3e) & 2);
    }
#if REAL
  printk (BIOS_ERR, __FILE__ ":%d, %x\n", __LINE__, read_mchbar32 (0x24));
#endif
    {
  /* trouble start */
  write_mchbar32 (0x24, 0x10000 + info->memory_reserved_for_heci_mb);
  printk (BIOS_ERR, __FILE__ ":%d\n", __LINE__);

  send_heci_uma_message(info);
  /* trouble end */

    }
  pci_mm_write32 (HECIDEV, 0x10, 0x0);
  pci_mm_write8 (HECIDEV, 0x4, 0x0);

}

#if 1
static int
have_match_ranks (struct raminfo *info, int channel, int ranks)
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
  if (info->populated_ranks[channel][0][0] != info->populated_ranks[channel][1][0])
    return 1;
  if (info->populated_ranks[channel][0][1] != info->populated_ranks[channel][1][1])
    return 1;
  if (info->is_x16_module[channel][0] != info->is_x16_module[channel][1])
    return 0;
  if (info->density[channel][0] != info->density[channel][1])
    return 0;
  return 1;
}
#endif

#define WTF1 1

#if REAL
void raminit (const int s3resume)
#else
void raminit (int s3resume)
#endif
{
  unsigned channel, slot, lane, rank;
  int i;

  struct raminfo info;

#ifdef LATER
  const dimminfo_t *const dimms = sysinfo->dimms;
  const timings_t *const timings = &sysinfo->selected_timings;
  const int sff = sysinfo->gfx_type == GMCH_GS45;

  int ch;
  u8 reg8;
  int i;

  report_platform_info();
#endif

  gav (0x55);

  outb (0x0, 0x62);

  u16 si;

#if ! REAL
  //Unrecognised: [ffff000:f917] 000b.0012   CPUID: eax: 0000000b; ecx: 00000000 => 00000001.00000002.00000100.00000000

  //Unrecognised: [ffff000:f927] 000b.0013   CPUID: eax: 0000000b; ecx: 00000001 => 00000004.00000004.00000201.00000000

  unsigned number_cores;
#if REAL
  struct cpuid_result result;
  unsigned threads_per_package, threads_per_core;

  /* Logical processors (threads) per core */
  result = cpuid_ext(0xb, 0);
  threads_per_core = result.ebx & 0xffff;

  /* Logical processors (threads) per package */
  result = cpuid_ext(0xb, 1);
  threads_per_package = result.ebx & 0xffff;

  if (threads_per_package == 0 || threads_per_core == 0 || threads_per_package % threads_per_core)
    number_cores = 1;
  else
    number_cores = threads_per_package / threads_per_core;
#else
  number_cores = 2;
#endif

  u8 al = nvram_read (0x4c);
  if (number_cores <= 1)
    si = 0;
  else if (!(al & 1))
    si = 1;
  else if (number_cores <= 2)
    si = 0;
  else if (!(al & 2))
    si = 2;
  else
    si = 0;
  if (!(nvram_read (0x55) & 2))
    si |= 0x100;
  printf ("%x\n", si);
#else
  /* bit 0 = disable multicore,
     bit 1 = disable quadcore,
     bit 8 = disable hyperthreading.  */
  si = 0;
#endif

  pci_write32 (0xff, 0x0, 0x0, 0x80,
	       (pci_read32 (0xff, 0x0, 0x0, 0x80) & 0xfffffefc) | 0x10000 | si);
  outb (0x1, 0x62);
  outb (0x4, 0x62);
  pci_write32 (SOUTHBRIDGE, RCBA, DEFAULT_RCBA | 1);
  gav (read32 (DEFAULT_RCBA | 0x3410));
  write32 (DEFAULT_RCBA | 0x3410, 0xc61);
  gav (read32 (DEFAULT_RCBA | 0x3410));
  pci_write32 (SOUTHBRIDGE, 0x40, 0x400);
  pci_write8 (SOUTHBRIDGE, 0x44, 0x80);

  u16 t4041 = read_tco16 (0x8);
  gav (t4041);
  write_tco16 (0x8, t4041);
  pci_write32 (SOUTHBRIDGE, 0xd0, 0x0);
  pci_write16 (SOUTHBRIDGE, 0x82, 0x3c01);

  u32 t4046 = pci_read32 (SOUTHBRIDGE, 0xdc);
  gav (t4046);
  pci_write32 (SOUTHBRIDGE, 0xdc, t4046);
  pci_write32 (0xff, 0x0, 0x1, 0x50, DEFAULT_PCIEXBAR | 1);
  pci_write32 (SOUTHBRIDGE, RCBA, DEFAULT_RCBA | 1);
  gav (read32 (DEFAULT_RCBA | 0x3410));
  write32 (DEFAULT_RCBA | 0x3410, 0xc61);

  pci_write32 (SOUTHBRIDGE, 0x84, 0x7c1601);
  pci_write32 (SOUTHBRIDGE, 0x8c, 0x1c1681);
  outb (0x2, 0x62);
#if !REAL

  my_write_msr (0x79, 0xffec1410);
  //Unrecognised: [ffff000:fc22] 00c0.00c1   Microcode Update: ERROR: Cannot fake write in a post-hook.

  //Unrecognised: [ffff000:fb6a] 00c8.00c9   CPUID: eax: 00000006; ecx: 0000008b => 00000005.00000002.00000001.00000000
#endif

  rdmsr (0xce);
  rdmsr (0x199);
  my_write_msr (0x199, 0x15);
  rdmsr (0x1a0);
  my_write_msr (0x1a0, 0x00850089);
  rdmsr (0xce);
  rdmsr (0x199);
  my_write_msr (0x199, 0x14);
  rdmsr (0x1a0);
  my_write_msr (0x1a0, 0x00850089);
  rdmsr (0x1f1);
  my_write_msr (0x1f1, 0x1);
  outb (0x5, 0x62);

  /*Unrecognised: [ffff000:fc9f] 00ed.00ee   LAPIC: [00000300] <= 000c4500

    Unrecognised: [ffff000:fc9f] 00ed.00ef   LAPIC: [00000300] => 000c0500
  */
  outb (0x3, 0x62);

  outb (0x0, 0x62);
  outb (0x2, 0x62);
  outb (0x2c, 0x62);
  outb (0x12, 0x62);
  outb (0x30, 0x62);
  /*Unrecognised: addr ff7ff7da val ff7ff856*/

  outb (0x13, 0x62);
  outb (0x28, 0x62);
  outb (0x29, 0x62);
  outb (0x17, 0x62);
  outb (0x27, 0x62);
  outb (0x4a, 0x62);
  /*Unrecognised: addr ff7ff7da val ff7ff856*/

  gav (pci_mm_read16 (SOUTHBRIDGE, 0x40)); // = 0x1001

  outb (0x11, 0x62);
  outb (0x40, 0x62);
  pci_write32 (NORTHBRIDGE, D0F0_MCHBAR_LO, DEFAULT_MCHBAR | 1);
  pci_write32 (0, 0x1f, 0x3, SMB_BASE, SMBUS_IO_BASE);
  pci_write32 (0, 0x1f, 0x3, HOSTC, 0x1);
  gav (pci_read16 (0, 0x1f, 0x3, 0x4)); // = 0x1
  pci_write16 (0, 0x1f, 0x3, 0x4, 0x1);
  pci_write32 (SOUTHBRIDGE, RCBA, DEFAULT_RCBA | 1);
  pci_mm_write32 (NORTHBRIDGE, D0F0_MCHBAR_LO, DEFAULT_MCHBAR | 1);
  pci_mm_write32 (NORTHBRIDGE, D0F0_DMIBAR_LO, DEFAULT_DMIBAR | 1);
  gav (pci_mm_read8 (HECIDEV, PCI_VENDOR_ID)); // = 0x86
  pci_write32 (SOUTHBRIDGE, RCBA, DEFAULT_RCBA | 1);
  gav (pci_read32 (SOUTHBRIDGE, 0x40)); // = 0x1001
  pci_write32 (SOUTHBRIDGE, 0x40, 0x1001);
  gav (pci_read8 (SOUTHBRIDGE, 0x44)); // = 0x80
  pci_write8 (SOUTHBRIDGE, 0x44, 0x80);
  gav (pci_read8 (SOUTHBRIDGE, 0xa6)); // = 0x2
  pci_write8 (SOUTHBRIDGE, 0xa6, 0x2);
  gav (pci_read32 (SOUTHBRIDGE, GPIOBASE)); // = DEFAULT_GPIOBASE | 1
  pci_write32 (SOUTHBRIDGE, GPIOBASE, DEFAULT_GPIOBASE | 1);
  gav (pci_read8 (SOUTHBRIDGE, 0x4c)); // = 0x10
  pci_write8 (SOUTHBRIDGE, 0x4c, 0x10);
#if !REAL
  gav (pci_read8 (SOUTHBRIDGE, 0x8)); // = 0x6
  gav (pci_read8 (SOUTHBRIDGE, 0x8)); // = 0x6
  gav (read32 (DEFAULT_RCBA | 0x3598));
  gav (pci_read32 (0, 0x1d, 0x0, 0xfc)); // = 0x20191708
  pci_write32 (0, 0x1d, 0x0, 0xfc, 0x20191708);
  gav (pci_read8 (SOUTHBRIDGE, 0x8)); // = 0x6
  gav (pci_read8 (SOUTHBRIDGE, 0x8)); // = 0x6
  gav (pci_read8 (0, 0x1d, 0x0, 0x88)); // = 0xa0
  pci_write8 (0, 0x1d, 0x0, 0x88, 0xa0);
  gav (pci_read32 (0, 0x1a, 0x0, 0xfc)); // = 0x20191708
  pci_write32 (0, 0x1a, 0x0, 0xfc, 0x20191708);
  gav (pci_read8 (SOUTHBRIDGE, 0x8)); // = 0x6
  gav (pci_read8 (SOUTHBRIDGE, 0x8)); // = 0x6
  gav (pci_read8 (0, 0x1a, 0x0, 0x88)); // = 0xa0
  pci_write8 (0, 0x1a, 0x0, 0x88, 0xa0);
  write8 (DEFAULT_RCBA | 0x14, 0x11);
  write16 (DEFAULT_RCBA | 0x50, 0x7654);
#endif

  enable_hpet ();
  u32 t4143 = read32 (0xfed00010);
  gav (t4143);
  write32 (0xfed00010, t4143 | 1);
  gav (read8 (DEFAULT_RCBA | 0x3428));
  write8 (DEFAULT_RCBA | 0x3428, 0x1d);
  pci_mm_write32 (0, 0x1f, 0x6, 0x40, 0x40000000);
  pci_mm_write32 (0, 0x1f, 0x6, 0x44, 0x0);
  gav (pci_mm_read32 (0, 0x1f, 0x6, 0x40)); // = 0x40000004
  pci_mm_write32 (0, 0x1f, 0x6, 0x40, 0x40000005);
  u16 t4;
  t4 = read16 (0x4000001a);
  gav (t4);
  write16 (0x4000001a, (t4 & ~0xf) | 0x10f0);
  gav (pci_mm_read32 (0, 0x1f, 0x6, 0x40)); // = 0x40000005
  pci_mm_write32 (0, 0x1f, 0x6, 0x40, 0x40000004);
  pci_mm_write32 (0, 0x1f, 0x6, 0x40, 0x0);
  gav (pci_read16 (SOUTHBRIDGE, 0x40)); // = 0x1001
  gav (pci_read16 (SOUTHBRIDGE, GPIOBASE)); // = DEFAULT_GPIOBASE | 1
  gav (read8 (DEFAULT_RCBA | 0x3414));
  gav (read_acpi16 (0x0));
#if !REAL
  u16 pm1cnt;
  gav (pm1cnt = read_acpi16 (0x4));
  s3resume = ((pm1cnt >> 10) & 7) == 5;
#endif
  if (s3resume)
    {
      pci_mm_read8 (SOUTHBRIDGE, 0xa2); // = 0xa0
      pci_mm_read8 (SOUTHBRIDGE, 0xa4); // = 0x9
      pci_mm_read8 (SOUTHBRIDGE, 0xa4); // = 0x9
      pci_mm_write8 (SOUTHBRIDGE, 0xa4, 0x9);
    }
  else
    {
      gav (pci_mm_read8 (SOUTHBRIDGE, 0xa4)); // = 0x5
      pci_mm_write8 (SOUTHBRIDGE, 0xa4, 0x5);
    }
  pci_mm_write8 (0, 0x3, 0x0, 0x4, 0x0);
  write16 (DEFAULT_RCBA | 0x3124, 0x2321);
  pci_mm_write8 (SOUTHBRIDGE, 0xdc, 0x0);

  pci_mm_write32 (SOUTHBRIDGE, RCBA, DEFAULT_RCBA | 1);
  gav (pci_mm_read16 (SOUTHBRIDGE, 0x80)); // = 0x10
  pci_mm_write16 (SOUTHBRIDGE, 0x80, 0x10);
  pci_mm_write16 (SOUTHBRIDGE, 0x82, 0x3f07);
  if (s3resume)
    {
      read_mchbar32 (0x1e8);
      write_mchbar32 (0x1e8, 0x6);
      read_mchbar32 (0x1e8);
      write_mchbar32 (0x1e8, 0x4);
    }

  pci_mm_write16 (SOUTHBRIDGE, GPIOBASE, DEFAULT_GPIOBASE);
  pci_mm_read8 (SOUTHBRIDGE, 0x4c); // = 0x10
  pci_mm_write8 (SOUTHBRIDGE, 0x4c, 0x10);
  gav (inw (DEFAULT_GPIOBASE | 0x38)); // = 0x10

  outl (0x1963a5ff, DEFAULT_GPIOBASE);
  outl (0xffffffff, DEFAULT_GPIOBASE | 0xc);
  outl (0x87bf6aff, DEFAULT_GPIOBASE | 0x4);
  outl (0x0, DEFAULT_GPIOBASE | 0x18);
  outl (0x120c6, DEFAULT_GPIOBASE | 0x2c);
  outl (0x27706fe, DEFAULT_GPIOBASE | 0x30);
  outl (0x29fffff, DEFAULT_GPIOBASE | 0x38);
  outl (0x1b01f9f4, DEFAULT_GPIOBASE | 0x34);
  outl (0x0, DEFAULT_GPIOBASE | 0x40);
  outl (0x0, DEFAULT_GPIOBASE | 0x48);
  outl (0xf00, DEFAULT_GPIOBASE | 0x44);
  pci_mm_write16 (SOUTHBRIDGE, PMBASE, DEFAULT_PMBASE);
  gav (pci_mm_read8 (SOUTHBRIDGE, ACPI_CNTL)); // = 0x80
  pci_mm_write8 (SOUTHBRIDGE, ACPI_CNTL, 0x80);
  gav (pci_mm_read32 (SOUTHBRIDGE, 0xac)); // = 0x0
  pci_mm_write32 (SOUTHBRIDGE, 0xac, 0x0);
  pci_mm_write32 (SOUTHBRIDGE, 0x84, 0xc0681);

  gav (read32 (DEFAULT_RCBA | 0x3400));
  write32 (DEFAULT_RCBA | 0x3400, 0x1c);
  gav (read32 (DEFAULT_RCBA | 0x3410));
  write32 (DEFAULT_RCBA | 0x3410, 0xc61);

  gav (read_tco16 (0x8));
  write_tco16 (0x8, 0x800);
  write_tco8 (0x6, gav (read_tco8 (0x6)) | 0x2);
  gav (inb (0x61)); // = 0x2
  outb (0x3c, 0x61);
  enable_hpet ();
  pci_mm_read8 (SOUTHBRIDGE, 0xa4); // = 0x5
#if !REAL
  if (!s3resume)
    {
      nvram_write (0xa, nvram_read (0xa) | 0x70);
      nvram_write (0xb, nvram_read (0xb) | 0x80);
      nvram_write (0xa, nvram_read (0xa) & ~0x50);
      nvram_write (0xb, nvram_read (0xb) & ~0x80);
    }
#endif
  gav (read32 (DEFAULT_RCBA | 0x3410));   
  write32 (DEFAULT_RCBA | 0x3410, 0xc61);
  gav (pci_read8 (SOUTHBRIDGE, 0x8)); // = 0x6
  gav (inl (DEFAULT_GPIOBASE)); // = 0x6
  outl (0x7963a5ff, DEFAULT_GPIOBASE);
  gav (inl (DEFAULT_GPIOBASE | 0x4)); // = 0x7963a5ff
  outl (0x87bf6aff, DEFAULT_GPIOBASE | 0x4);
  outl (gav (inl (DEFAULT_GPIOBASE | 0xc)) | 0x40000000, DEFAULT_GPIOBASE | 0xc);
  gav (inl (DEFAULT_GPIOBASE | 0x60)); // = 0xfffbfffb
  outl (0x41000000, DEFAULT_GPIOBASE | 0x60);
  pci_write32 (SOUTHBRIDGE, LPC_GEN3_DEC, 0x1c1681);
  pci_write32 (SOUTHBRIDGE, LPC_GEN2_DEC, 0xc15e1);
  pci_write32 (SOUTHBRIDGE, LPC_GEN1_DEC, 0x7c1601);
  gav (inl (DEFAULT_GPIOBASE | 0xc)); // = 0x7c1601
  outb (0x15, 0x62);
  outb (0x16, 0x62);
  gav (pci_read32 (NORTHBRIDGE, D0F0_MCHBAR_LO)); // = DEFAULT_MCHBAR | 1
  u8 x2ca8;

  gav (x2ca8 = read_mchbar8 (0x2ca8));
  if ((x2ca8 & 1) || (x2ca8 == 8 && !s3resume))
    {
      printk(BIOS_DEBUG, "soft reset detected, rebooting properly\n");
      write_mchbar8 (0x2ca8, 0);
      outb (0xe, 0xcf9);
#if REAL
      while (1)
	{
	  asm volatile ("hlt");
	}
#else
      printf ("CP5\n");
      exit (0);
#endif
    }

#if !REAL
  if (!s3resume)
  {
    u8 t;
    gav (t = nvram_read (0x33));
    if (x2ca8 == 0)
      {
	nvram_write (0x33, t & ~0x40);
	gav (read32 (DEFAULT_RCBA | 0x3598));
	write32 (DEFAULT_RCBA | 0x3598, 0x1);
	pci_write16 (0, 0x1d, 0x0, 0x20, 0x2000);
	gav (pci_read8 (0, 0x1d, 0x0, 0x4)); // = 0xff
	pci_write8 (0, 0x1d, 0x0, 0x4, 0xff);
	pci_write16 (0, 0x1d, 0x1, 0x20, 0x2020);
	gav (pci_read8 (0, 0x1d, 0x1, 0x4)); // = 0xff
	pci_write8 (0, 0x1d, 0x1, 0x4, 0xff);
	pci_write16 (0, 0x1d, 0x2, 0x20, 0x2040);
	gav (pci_read8 (0, 0x1d, 0x2, 0x4)); // = 0xff
	pci_write8 (0, 0x1d, 0x2, 0x4, 0xff);
	pci_write16 (0, 0x1d, 0x3, 0x20, 0x2060);
	gav (pci_read8 (0, 0x1d, 0x3, 0x4)); // = 0xff
	pci_write8 (0, 0x1d, 0x3, 0x4, 0xff);
	pci_write16 (0, 0x1a, 0x0, 0x20, 0x2080);
	gav (pci_read8 (0, 0x1a, 0x0, 0x4)); // = 0xff
	pci_write8 (0, 0x1a, 0x0, 0x4, 0xff);
	pci_write16 (0, 0x1a, 0x1, 0x20, 0x20a0);
	gav (pci_read8 (0, 0x1a, 0x1, 0x4)); // = 0xff
	pci_write8 (0, 0x1a, 0x1, 0x4, 0xff);
	pci_write16 (0, 0x1a, 0x2, 0x20, 0x20e0);
	gav (pci_read8 (0, 0x1a, 0x2, 0x4)); // = 0xff
	pci_write8 (0, 0x1a, 0x2, 0x4, 0xff);
	for (i = 0; i < 15; i++)
	  {
	    gav (inw (0x2010)); // = 0xff
	    gav (inw (0x2012)); // = 0xff
	    gav (inw (0x2030)); // = 0xff
	    gav (inw (0x2032)); // = 0xff
	    gav (inw (0x2050)); // = 0xff
	    gav (inw (0x2052)); // = 0xff
	    gav (inw (0x2070)); // = 0xff
	    gav (inw (0x2072)); // = 0xff
	    gav (inw (0x2090)); // = 0xff
	    gav (inw (0x2092)); // = 0xff
	    gav (inw (0x20b0)); // = 0xff
	    gav (inw (0x20b2)); // = 0xff
	    gav (inw (0x20f0)); // = 0xff
	    gav (inw (0x20f2)); // = 0xff
	    if (i != 14)
	      pm_wait (0x400); /* <10*/ 
	  }
	pci_write16 (0, 0x1d, 0x0, 0x20, 0x0);
	gav (pci_read8 (0, 0x1d, 0x0, 0x4)); // = 0xff
	pci_write8 (0, 0x1d, 0x0, 0x4, 0xfe);
	pci_write16 (0, 0x1d, 0x1, 0x20, 0x0);
	gav (pci_read8 (0, 0x1d, 0x1, 0x4)); // = 0xff
	pci_write8 (0, 0x1d, 0x1, 0x4, 0xfe);
	pci_write16 (0, 0x1d, 0x2, 0x20, 0x0);
	gav (pci_read8 (0, 0x1d, 0x2, 0x4)); // = 0xff
	pci_write8 (0, 0x1d, 0x2, 0x4, 0xfe);
	pci_write16 (0, 0x1d, 0x3, 0x20, 0x0);
	gav (pci_read8 (0, 0x1d, 0x3, 0x4)); // = 0xff
	pci_write8 (0, 0x1d, 0x3, 0x4, 0xfe);
	pci_write16 (0, 0x1a, 0x0, 0x20, 0x0);
	gav (pci_read8 (0, 0x1a, 0x0, 0x4)); // = 0xff
	pci_write8 (0, 0x1a, 0x0, 0x4, 0xfe);
	pci_write16 (0, 0x1a, 0x1, 0x20, 0x0);
	gav (pci_read8 (0, 0x1a, 0x1, 0x4)); // = 0xff
	pci_write8 (0, 0x1a, 0x1, 0x4, 0xfe);
	pci_write16 (0, 0x1a, 0x2, 0x20, 0x0);
	gav (pci_read8 (0, 0x1a, 0x2, 0x4)); // = 0xff
	pci_write8 (0, 0x1a, 0x2, 0x4, 0xfe);
	write32 (DEFAULT_RCBA | 0x3598, 0x0);
      }
  }
#endif

  outb (0x55, 0x62);
  
#if REAL
  outb (0xb6, 0x43);
  outb (0x00, 0x42);
  outb (0x05, 0x42);
#endif

  outb (0x32, 0x62);
  /*Unrecognised: addr ff7ff7da val ff7ff856*/
  
  gav (pci_read32 (0, 0x1f, 0x3, 0x0)); // = 0x3b308086
#if REAL
  gav (pci_read32 (0, 0x1f, 0x3, 0x20)); // = 0x1101
#endif
  pci_write32 (0, 0x1f, 0x3, 0x20, 0x1100);
  gav (pci_read8 (0, 0x1f, 0x3, 0x4)); // = 0x1
  pci_write8 (0, 0x1f, 0x3, 0x4, 0x1);
  gav (pci_read8 (0, 0x1f, 0x3, 0x40)); // = 0x1
  pci_write8 (0, 0x1f, 0x3, 0x40, 0x9);
  gav (pci_read8 (0, 0x1f, 0x3, 0x40)); // = 0x1
  pci_write8 (0, 0x1f, 0x3, 0x40, 0x1);
#if REAL
  pci_read32 (0, 0x1f, 0x3, 0x20); // = 0x1101
#endif
  outb (0x4f, 0x62);
  outb (0x50, 0x62);
  /*Unrecognised: addr ff7ff7da val ff7ff856*/

  gav (pci_read8 (NORTHBRIDGE, D0F0_CAPID0 + 8)); // = 0x88
  rdmsr (0x17);// !!!
  /*Unrecognised: [0000:fffaf715] 1a183.1a184   Microcode Update: ERROR: Cannot fake write in a post-hook.*/

  rdmsr (0x17);// !!!
  /*Unrecognised: [0000:fffaf715] 1a25d.1a25e   Microcode Update: ERROR: Cannot fake write in a post-hook.*/

  outb (0x48, 0x62);
  if (x2ca8 != 0)
    {
      outb (0x42, 0x15ec);
      gav (inb (0x15ee)); // = 0x42
    }

#if REAL
  /* Enable SMBUS. */
  enable_smbus();
#endif

  gav (pci_mm_read16 (SOUTHBRIDGE, GPIOBASE)); // = DEFAULT_GPIOBASE | 1
  u16 t3;

  if (x2ca8 == 0)
    {
      gav (t3 = inw (DEFAULT_GPIOBASE | 0x38));
      outw (t3 & ~0x400, DEFAULT_GPIOBASE | 0x38);
      printk (BIOS_ERR, "5c:6 start\n");
      gav (smbus_read_byte (0x5c, 0x06));
      smbus_write_byte (0x5c, 0x06, 0x8f);
      printk (BIOS_ERR, "5c:6 end\n");
       
      for (i = 0; i < 5; i++)
	pm_wait (0x3e8);

      printk (BIOS_ERR, "5c:7 start\n");
      gav (smbus_read_byte (0x5c, 0x07));
      smbus_write_byte (0x5c, 0x07, 0x8f);
      printk (BIOS_ERR, "5c:7 end\n");
       
      for (i = 0; i < 5; i++)
	pm_wait (0x3e8);
      gav (pci_mm_read16 (SOUTHBRIDGE, GPIOBASE)); // = DEFAULT_GPIOBASE | 1
      outw (t3 | 0x400, DEFAULT_GPIOBASE | 0x38);
      outb (0x42, 0x15ec);
      gav (inb (0x15ee)); // = 0x42
      gav (pci_mm_read16 (SOUTHBRIDGE, GPIOBASE)); // = DEFAULT_GPIOBASE | 1
    }

  gav (t3 = inw (DEFAULT_GPIOBASE | 0x38));
  outw (t3 & ~0x400, DEFAULT_GPIOBASE | 0x38);

  printk (BIOS_ERR, "57:55 start\n");
  gav (smbus_read_byte (0x57, 0x55));
  printk (BIOS_ERR, "57:55 end\n");
  gav (pci_mm_read16 (SOUTHBRIDGE, GPIOBASE)); // = DEFAULT_GPIOBASE | 1
  outw (t3 | 0x400, DEFAULT_GPIOBASE | 0x38);

  outb (0x42, 0x62);
  gav (read_tco16 (0x6));
#if !REAL
  pci_mm_write32 (NORTHBRIDGE, PCI_SUBSYSTEM_VENDOR_ID, 0x219317aa);
  pci_mm_write32 (0, 0x1, 0x0, 0x8c, 0x219417aa);
  pci_mm_write32 (0xff, 0, 0, PCI_SUBSYSTEM_VENDOR_ID, 0x219617aa);
  pci_mm_write32 (0xff, 0, 1, PCI_SUBSYSTEM_VENDOR_ID, 0x219617aa);
  pci_mm_write32 (0xff, 2, 0, PCI_SUBSYSTEM_VENDOR_ID, 0x219617aa);
  pci_mm_write32 (0xff, 2, 1, PCI_SUBSYSTEM_VENDOR_ID, 0x219617aa);
#endif
  pci_mm_write32 (NORTHBRIDGE, D0F0_MCHBAR_LO, DEFAULT_MCHBAR | 1);
  pci_mm_write32 (NORTHBRIDGE, D0F0_MCHBAR_HI, 0x0);
  pci_mm_write32 (NORTHBRIDGE, D0F0_DMIBAR_LO, DEFAULT_DMIBAR | 1);
  pci_mm_write32 (NORTHBRIDGE, D0F0_DMIBAR_HI, 0x0);
  pci_mm_write32 (NORTHBRIDGE, D0F0_EPBAR_LO, DEFAULT_EPBAR | 1);
  pci_mm_write32 (NORTHBRIDGE, D0F0_EPBAR_HI, 0x0);
  pci_mm_read32 (NORTHBRIDGE, D0F0_MCHBAR_LO); // = DEFAULT_MCHBAR | 1
  pci_mm_read32 (NORTHBRIDGE, D0F0_MCHBAR_HI); // = 0x0
  gav (read8 (DEFAULT_DMIBAR | 0x254));
  write8 (DEFAULT_DMIBAR | 0x254, 0x1);
  gav (pci_mm_read32 (SOUTHBRIDGE, 0xec)); // = 0x2b83806
  write16 (DEFAULT_DMIBAR | 0x1b8, 0x18f2);
  pci_mm_write16 (NORTHBRIDGE, D0F0_DEVEN, pci_mm_read16 (NORTHBRIDGE, D0F0_DEVEN) & 0xfeff);
  read_mchbar16 (0x48);
  write_mchbar16 (0x48, 0x2);
  rdmsr (0x17);

  pci_mm_read32 (NORTHBRIDGE, D0F0_DMIBAR_LO); // = DEFAULT_DMIBAR | 1
  if (pci_mm_read16 (0, 0x1, 0x0, 0x0) != 0xffff)
    {
      gav (pci_mm_read16 (0, 0x1, 0x0, 0xac));
      pci_mm_write32 (0, 0x1, 0x0, 0x200, pci_mm_read32 (0, 0x1, 0x0, 0x200) & ~0x100);
      pci_mm_write8 (0, 0x1, 0x0, 0x1f8, (pci_mm_read8 (0, 0x1, 0x0, 0x1f8) & ~1) | 4);
      u32 t4431 = read32 (DEFAULT_DMIBAR | 0xd68);
      gav (t4431);
      write32 (DEFAULT_DMIBAR | 0xd68, t4431 | 0x08000000);
      pci_mm_write32 (0, 0x1, 0x0, 0x200, pci_mm_read32 (0, 0x1, 0x0, 0x200) & ~0x00200000);
      gav (pci_mm_read8 (0, 0x1, 0x0, 0xd60)); // = 0x0
      gav (pci_mm_read8 (0, 0x1, 0x0, 0xd60)); // = 0x0
      pci_mm_write8 (0, 0x1, 0x0, 0xd60, 0x3);
      gav (pci_mm_read16 (0, 0x1, 0x0, 0xda8)); // = 0xbf9
      gav (pci_mm_read16 (0, 0x1, 0x0, 0xda8)); // = 0xbf9
      pci_mm_write16 (0, 0x1, 0x0, 0xda8, 0xf9);
      pci_mm_read16 (0, 0x1, 0x0, 0xda8); // = 0xf9
      pci_mm_read16 (0, 0x1, 0x0, 0xda8); // = 0xf9
      pci_mm_write16 (0, 0x1, 0x0, 0xda8, 0x79);
      pci_mm_read8 (0, 0x1, 0x0, 0xd0); // = 0x2
      pci_mm_read8 (0, 0x1, 0x0, 0xd0); // = 0x2
      pci_mm_write8 (0, 0x1, 0x0, 0xd0, 0x1);
      pci_mm_read16 (0, 0x1, 0x0, 0x224); // = 0xd
      pci_mm_read32 (NORTHBRIDGE, D0F0_CAPID0); // = 0x10c0009
      pci_mm_read32 (NORTHBRIDGE, D0F0_CAPID0 + 4); // = 0x316126
      pci_mm_read16 (0, 0x1, 0x0, 0x224); // = 0xd
      pci_mm_write16 (0, 0x1, 0x0, 0x224, 0x1d);
      pci_mm_read16 (0, 0x6, 0x0, 0x0); // = 0xffff
      pci_mm_read16 (0, 0x1, 0x0, 0x224); // = 0x1d
      pci_mm_read16 (0, 0x6, 0x0, 0x0); // = 0xffff
      pci_mm_write16 (0, 0x1, 0x0, 0xac, 0x4d01);
      pci_mm_read16 (0, 0x1, 0x0, 0x224); // = 0x1d
      pci_mm_read8 (0, 0x1, 0x0, 0xba); // = 0x0
      pci_mm_read16 (0, 0x1, 0x0, 0x0); // = 0x8086
      pci_mm_read32 (0, 0x1, 0x0, 0xc00); // = 0xffffffff
      pci_mm_write32 (0, 0x1, 0x0, 0xc00, 0xffffc0fc);
      pci_mm_read32 (0, 0x1, 0x0, 0xc04); // = 0x9600000f
      pci_mm_write32 (0, 0x1, 0x0, 0xc04, 0x96000000);
      pci_mm_read32 (0, 0x1, 0x0, 0xc04); // = 0x96000000
      pci_mm_write32 (0, 0x1, 0x0, 0xc04, 0x16000000);
      pci_mm_write32 (0, 0x1, 0x0, 0xc08, 0x0);
    }
  else
    pci_mm_read16 (0, 0x1, 0x0, 0x0); // = 0xffff

  pci_mm_read32 (NORTHBRIDGE, D0F0_DMIBAR_LO); // = DEFAULT_DMIBAR | 1
  pci_mm_read16 (0, 0x6, 0x0, 0x0); // = 0xffff
  pci_mm_read16 (0, 0x6, 0x0, 0x0); // = 0xffff
  pci_mm_write32 (HECIDEV, HECIBAR, DEFAULT_HECIBAR);
  pci_mm_write32 (HECIDEV, PCI_COMMAND, PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);
    
  outl ((gav (inl (DEFAULT_GPIOBASE | 0x38)) & ~0x140000) | 0x400000, DEFAULT_GPIOBASE | 0x38);
  gav (inb (DEFAULT_GPIOBASE | 0xe)); // = 0xfdcaff6e

#if !REAL
  pci_mm_write32 (0, 0x2, 0x0, PCI_SUBSYSTEM_VENDOR_ID, 0x215a17aa);
#endif
  pci_mm_write8 (NORTHBRIDGE, D0F0_DEVEN, pci_mm_read8 (NORTHBRIDGE, D0F0_DEVEN) | 2);
  pci_mm_write16 (0, 0x1, 0x0, 0x224, pci_mm_read16 (0, 0x1, 0x0, 0x224) | 1);
  pci_mm_write16 (NORTHBRIDGE, D0F0_GGC, pci_mm_read16 (NORTHBRIDGE, D0F0_GGC) | 2);
  pci_mm_read32 (NORTHBRIDGE, D0F0_MCHBAR_LO); // = DEFAULT_MCHBAR | 1
  pci_mm_read32 (NORTHBRIDGE, D0F0_MCHBAR_HI); // = 0x0
  pci_mm_read32 (NORTHBRIDGE, D0F0_DMIBAR_LO); // = DEFAULT_DMIBAR | 1
  pci_mm_read32 (NORTHBRIDGE, D0F0_DMIBAR_HI); // = 0x0

  const struct { int dev, func; } bridges[] = { {0x1e, 0}, {0x1c, 0}, {0x1c, 1}, {0x1c, 2}, {0x1c, 3}, {0x1c, 4}, {0x1c, 5}, {0x1c, 6}, {0x1c, 7} };
  for (i = 0; i < sizeof (bridges) / sizeof (bridges[0]); i++)
    {
      u16 dev;
      pci_mm_write32 (0, bridges[i].dev, bridges[i].func, 0x18, 0x20200);
      for (dev = 0; dev < 0x20; dev++)
	{
	  u16 vendor = pci_mm_read16 (2, dev, 0x0, 0x0);
	  if (vendor == 0xffff)
	    continue;
	  pci_mm_read16 (2, dev, 0x0, 0xa);
	}
      pci_mm_write32 (0, bridges[i].dev, bridges[i].func, 0x18, 0x0);
    }

  pci_mm_read16 (0, 0x1, 0x0, 0x0); // = 0x8086
  pci_mm_read8 (NORTHBRIDGE, D0F0_CAPID0 + 6); // = 0x31
  pci_mm_read8 (0, 0x1, 0x0, 0xba); // = 0x0
  pci_mm_read16 (0, 0x6, 0x0, 0x0); // = 0xffff
  pci_mm_read8 (NORTHBRIDGE, D0F0_CAPID0 + 6); // = 0x31
  pci_mm_read8 (0, 0x6, 0x0, 0xba); // = 0xff
  pci_mm_read16 (0, 0x1, 0x0, 0x11a); // = 0x2
  pci_mm_read16 (0, 0x6, 0x0, 0x11a); // = 0xffff
  pci_mm_read16 (0, 0x1, 0x0, 0x0); // = 0x8086
  pci_mm_read32 (0, 0x1, 0x0, 0x18); // = 0x0
  pci_mm_write32 (0, 0x1, 0x0, 0x18, 0x0);
  pci_mm_read16 (0, 0x6, 0x0, 0x0); // = 0xffff
  write_mchbar16 (0x1170, 0xa880);
  write_mchbar8 (0x11c1, 0x1);
  write_mchbar16 (0x1170, 0xb880);
  read_mchbar8 (0x1210);
  write_mchbar8 (0x1210, 0x84);
  pci_mm_read8 (NORTHBRIDGE, D0F0_GGC); // = 0x52
  pci_mm_write8 (NORTHBRIDGE, D0F0_GGC, 0x2);
  pci_mm_read8 (NORTHBRIDGE, D0F0_GGC); // = 0x2
  pci_mm_write8 (NORTHBRIDGE, D0F0_GGC, 0x52);
  pci_mm_read16 (NORTHBRIDGE, D0F0_GGC); // = 0xb52

  pci_mm_write16 (NORTHBRIDGE, D0F0_GGC, 0xb52);
  u16 deven;
  deven = pci_mm_read16 (NORTHBRIDGE, D0F0_DEVEN); // = 0x3

  if (deven & 8)
    {
      write_mchbar8 (0x2c30, 0x20);
      pci_mm_read8 (0, 0x0, 0x0, 0x8); // = 0x18
      write_mchbar16 (0x2c30, read_mchbar16 (0x2c30) | 0x200);
      write_mchbar16 (0x2c32, 0x434);
      read_mchbar32 (0x2c44);
      write_mchbar32 (0x2c44, 0x1053687);
      pci_mm_read8 (0, 0x2, 0x0, 0x62); // = 0x2
      pci_mm_write8 (0, 0x2, 0x0, 0x62, 0x2);
      read8 (DEFAULT_RCBA | 0x2318);
      write8 (DEFAULT_RCBA | 0x2318, 0x47);
      read8 (DEFAULT_RCBA | 0x2320);
      write8 (DEFAULT_RCBA | 0x2320, 0xfc);
    }

  read_mchbar32 (0x30);
  write_mchbar32 (0x30, 0x40);
  pci_mm_read8 (SOUTHBRIDGE, 0x8); // = 0x6
  pci_mm_read16 (NORTHBRIDGE, D0F0_GGC); // = 0xb52
  pci_mm_write16 (NORTHBRIDGE, D0F0_GGC, 0xb50);
  gav (read32 (DEFAULT_RCBA | 0x3428));
  write32 (DEFAULT_RCBA | 0x3428, 0x1d);
#if !REAL
  for (i = 0x10; i < 0x28; )
    {
      u32 s;
      pci_mm_read32 (0, 0x2, 0x0, i); // = 0xffffffff
      pci_mm_read32 (0, 0x2, 0x0, i); // = 0xffffffff
      pci_mm_write32 (0, 0x2, 0x0, i, 0x0);
      pci_mm_read32 (0, 0x2, 0x0, i); // = 0xffffffff
      pci_mm_write32 (0, 0x2, 0x0, i, 0xffffffff);
      s = pci_mm_read32 (0, 0x2, 0x0, i);
      if (s != 0xffffffff && s != 0)
	{
	  if (s & 1)
	    {
	      pci_mm_write32 (0, 0x2, 0x0, i, s & 0x7);
	      i += 4;
	    }
	  else
	    {
	      pci_mm_read32 (0, 0x2, 0x0, i); // = 0xffffffff
	      pci_mm_write32 (0, 0x2, 0x0, i, s & 0xf);
	      i += 8;
	    }
	}
      else
	i += 4;
    }
#endif

  outb ((gav (inb (DEFAULT_GPIOBASE | 0x3a)) & ~0x2) | 0x20,
	DEFAULT_GPIOBASE | 0x3a);
  outb (0x50, 0x15ec);
  outb (inb (0x15ee) & 0x70, 0x15ee);
#if !REAL
  pci_mm_read8 (0, 0x1d, 0x0, 0x80); // = 0x0
  pci_mm_write8 (0, 0x1d, 0x0, 0x80, 0x1);
  pci_mm_read8 (0, 0x1a, 0x0, 0x80); // = 0x0
  pci_mm_write8 (0, 0x1a, 0x0, 0x80, 0x1);
  pci_mm_write32 (HECIDEV, PCI_SUBSYSTEM_VENDOR_ID, 0x215f17aa);
  pci_mm_write32 (0, 0x16, 0x2, PCI_SUBSYSTEM_VENDOR_ID, 0x216117aa);
  pci_mm_write32 (0, 0x16, 0x3, PCI_SUBSYSTEM_VENDOR_ID, 0x216217aa);
  pci_mm_write32 (0, 0x1a, 0x0, PCI_SUBSYSTEM_VENDOR_ID, 0x216317aa);
  pci_mm_write32 (0, 0x1b, 0x0, PCI_SUBSYSTEM_VENDOR_ID, 0x215e17aa);
  pci_mm_write32 (0, 0x1c, 0x0, 0x94, 0x216417aa);
  pci_mm_write32 (0, 0x1c, 0x1, 0x94, 0x216417aa);
  pci_mm_write32 (0, 0x1c, 0x2, 0x94, 0x216417aa);
  pci_mm_write32 (0, 0x1c, 0x3, 0x94, 0x216417aa);
  pci_mm_write32 (0, 0x1c, 0x4, 0x94, 0x216417aa);
  pci_mm_write32 (0, 0x1d, 0x0, PCI_SUBSYSTEM_VENDOR_ID, 0x216317aa);
  pci_mm_write32 (0, 0x1e, 0x0, 0x54, 0x216517aa);
  pci_mm_write32 (SOUTHBRIDGE, PCI_SUBSYSTEM_VENDOR_ID, 0x216617aa);
  pci_mm_write32 (0, 0x1f, 0x3, PCI_SUBSYSTEM_VENDOR_ID, 0x216717aa);
  pci_mm_write32 (0, 0x1f, 0x5, PCI_SUBSYSTEM_VENDOR_ID, 0x216a17aa);
  pci_mm_write32 (0, 0x1f, 0x6, PCI_SUBSYSTEM_VENDOR_ID, 0x219017aa);
  pci_mm_read8 (0, 0x1d, 0x0, 0x80); // = 0x1
  pci_mm_write8 (0, 0x1d, 0x0, 0x80, 0x0);
  pci_mm_read8 (0, 0x1a, 0x0, 0x80); // = 0x1
  pci_mm_write8 (0, 0x1a, 0x0, 0x80, 0x0);
  pci_mm_write32 (13, 0x0, 0x0, PCI_SUBSYSTEM_VENDOR_ID, 0x213317aa);
  pci_mm_write32 (13, 0x0, 0x1, PCI_SUBSYSTEM_VENDOR_ID, 0x213417aa);
  pci_mm_write32 (13, 0x0, 0x3, PCI_SUBSYSTEM_VENDOR_ID, 0x213617aa);
#endif

  pci_mm_write32 (HECIDEV, HECIBAR, DEFAULT_HECIBAR);
  pci_mm_write32 (HECIDEV, PCI_COMMAND, PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);
  write_acpi16 (0x2, 0x0);
  write_acpi32 (0x28, 0x0);
  write_acpi32 (0x2c, 0x0);
  if (!s3resume)
    {
      gav (read_acpi32 (0x4));
      gav (read_acpi32 (0x20));
      gav (read_acpi32 (0x34));
      write_acpi16 (0x0, 0x900);
      write_acpi32 (0x20, 0xffff7ffe);
      write_acpi32 (0x34, 0x56974);
      pci_mm_read8 (SOUTHBRIDGE, 0xa4); // = 0x5
      pci_mm_write8 (SOUTHBRIDGE, 0xa4, 0x7);
    }
  gav (read32 (DEFAULT_RCBA | 0x3410));

#if !REAL
  if (gav (read32 (DEFAULT_RCBA | 0x3804)) & 0x4000)
    {
      u32 v;

      write8 (DEFAULT_RCBA | 0x3894, 0x6);
      write8 (DEFAULT_RCBA | 0x3895, 0x50);
      write8 (DEFAULT_RCBA | 0x3896, 0x3b);
      write8 (DEFAULT_RCBA | 0x3897, 0x14);
      write8 (DEFAULT_RCBA | 0x3898, 0x2);
      write8 (DEFAULT_RCBA | 0x3899, 0x3);
      write8 (DEFAULT_RCBA | 0x389a, 0x20);
      write8 (DEFAULT_RCBA | 0x389b, 0x5);
      write8 (DEFAULT_RCBA | 0x389c, 0x9f);
      write8 (DEFAULT_RCBA | 0x389d, 0x20);
      write8 (DEFAULT_RCBA | 0x389e, 0x1);
      write8 (DEFAULT_RCBA | 0x389f, 0x6);
      write8 (DEFAULT_RCBA | 0x3890, 0xc);
      gav (read8 (DEFAULT_RCBA | 0x3890));
      write32 (DEFAULT_RCBA | 0x3808, 0x0);
      gav (read32 (DEFAULT_RCBA | 0x3808));
      write16 (DEFAULT_RCBA | 0x3891, 0x4242);
      gav (read16 (DEFAULT_RCBA | 0x3891));
      gav (read8 (DEFAULT_RCBA | 0x3890));
      write8 (DEFAULT_RCBA | 0x3890, 0xc);
      gav (read8 (DEFAULT_RCBA | 0x3890));
      if ((gav (read32 (DEFAULT_RCBA | 0x3810)) & 0x20) || WTF1)
	v = 0x2005;
      else
	v = 0x2015;
      write32 (DEFAULT_RCBA | 0x38c8, v);
      write32 (DEFAULT_RCBA | 0x38c4, 0x800000 | v);
      gav (read32 (DEFAULT_RCBA | 0x38b0));
      write32 (DEFAULT_RCBA | 0x38b0, 0x1000);
      gav (read32 (DEFAULT_RCBA | 0x38b4));
      gav (read32 (DEFAULT_RCBA | 0x38b0));
      write32 (DEFAULT_RCBA | 0x38b0, 0x4);
      gav (read32 (DEFAULT_RCBA | 0x38b4));
      write32 (DEFAULT_RCBA | 0x3874, 0x1fff07d0);
    }
#endif
  gav (inb (DEFAULT_GPIOBASE | 0xe)); // = 0x1fff07d0

#if REAL
  /* Enable SMBUS. */
  enable_smbus();
#endif

  {
    u8 block[5];
    u16 fsbfreq = 62879;
#if REAL
    printk (BIOS_ERR, "69:0 start\n");
#endif
    smbus_block_read (0x69, 0, 5, block);
    block[0] = fsbfreq;
    block[1] = fsbfreq >> 8;
#if REAL
    printk (BIOS_ERR, "Clock generator:  ");
    for (i = 0; i < 5; i++)
      printk (BIOS_ERR, "%02x ", block[i]);
    printk (BIOS_ERR, "\n");
#endif

    smbus_block_write (0x69, 0, 5, block);
#if REAL
    printk (BIOS_ERR, "69:0 end\n");
#endif
  }

  outb (0x44, 0x62);
  outb (0x3c, 0x62);
#if !REAL
  nvram_read (0x71);
#endif
  rdmsr (0xfe);
  rdmsr (0x201);
  rdmsr (0x203);
  rdmsr (0x205);
  rdmsr (0x207);
  gav (read32 (DEFAULT_RCBA | 0x3410));
  write32 (DEFAULT_RCBA | 0x3410, 0xc61);
  gav (read8 (0xfed40000));
  pci_mm_read32 (0xff, 0, 0, 0x88);
  read_mchbar32 (0x28);
  gav (read8 (0xfed30008));

  memset (&info, 0x5a, sizeof (info));

  info.last_500_command[0] = 0;
  info.last_500_command[1] = 0;

  info.some_base_frequency = 0x10e0;
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

  info.heci_bar = 0;
  info.memory_reserved_for_heci_mb = 0;
  if (!s3resume || REAL)
    {
      pci_mm_read8 (SOUTHBRIDGE, 0xa2); // = 0x80

      collect_system_info (&info);

#if REAL
      /* Enable SMBUS. */
      enable_smbus();
#endif

      memset (&info.populated_ranks, 0, sizeof (info.populated_ranks));

      gav (0x55);

      info.use_ecc = 1;
      for (channel = 0; channel < NUM_CHANNELS; channel++)
	for (slot = 0; slot < NUM_CHANNELS; slot++)
	  {
	    int v;
	    int try;
	    int addr;
	    const u8 useful_addresses[] = 
	      {
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
		0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
		THERMAL_AND_REFRESH,
		0x20,
		REFERENCE_RAW_CARD_USED,
		RANK1_ADDRESS_MAPPING,
		0x75, 0x76, 0x77, 0x78,
		0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88,
		0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95
	      };
	    if (slot)
	      continue;
	    for (try = 0; try < 5; try++)
	      {
		v = smbus_read_byte (0x50 + channel, DEVICE_TYPE);
		if (v >= 0)
		  break;
	      }
	    if (v < 0)
	      continue;
	    for (addr = 0; addr < sizeof (useful_addresses) / sizeof (useful_addresses[0]); addr++)
	      gav (info.spd[channel][0][useful_addresses[addr]] = smbus_read_byte (0x50 + channel, useful_addresses[addr]));
	    if (info.spd[channel][0][DEVICE_TYPE] != 11 )
	      die ("Only DDR3 is supported");

	    v = info.spd[channel][0][RANKS_AND_DQ];
	    info.populated_ranks[channel][0][0] = 1;
	    info.populated_ranks[channel][0][1] = ((v >> 3) & 7);
	    if (((v >> 3) & 7) > 1)
	      die ("At most 2 ranks are supported");
	    if ((v & 7) == 0 || (v & 7) > 2) 
	      die ("Only x8 and x16 modules are supported");
	    if ((info.spd[channel][slot][MODULE_TYPE] & 0xF) != 2
		&& (info.spd[channel][slot][MODULE_TYPE] & 0xF) != 3)
	      die ("Registered memory is not supported");
	    info.is_x16_module[channel][0] = (v & 7) - 1;
	    info.density[channel][slot] = info.spd[channel][slot][DENSITY] & 0xF;
	    if (!(info.spd[channel][slot][MEMORY_BUS_WIDTH] & 0x18))
	      info.use_ecc = 0;
	  }

      gav (0x55);

      for (channel = 0; channel < NUM_CHANNELS; channel++)
	{
	  int v = 0;
	  for (slot = 0; slot < NUM_SLOTS; slot++)
	    for (rank = 0; rank < NUM_RANKS; rank++)
	      v |= info.populated_ranks[channel][slot][rank] << (2 * slot + rank);
	  info.populated_ranks_mask[channel] = v;
	}

      gav (0x55);

      gav (pci_mm_read32 (NORTHBRIDGE, D0F0_CAPID0 + 4));
    }
  else
    {
      memset (info.populated_ranks, 0, sizeof (info.populated_ranks));
#if 0
      info.populated_ranks[0][0][0] = 1;
      info.populated_ranks[0][0][1] = 1;
      info.populated_ranks_mask[0] = 3;
      info.populated_ranks_mask[1] = 0;
      info.use_ecc = 0;
      info.max_slots_used_in_channel = 1;

      info.spd[0][0][0x02] = 0x0b;
      info.spd[0][0][0x03] = 0x03;
      info.spd[0][0][0x04] = 0x03;
      info.spd[0][0][0x07] = 0x09;
      info.spd[0][0][0x08] = 0x03;
      info.spd[0][0][0x0a] = 0x01;
      info.spd[0][0][0x0b] = 0x08;
      info.spd[0][0][0x0c] = 0x0c;
      info.spd[0][0][0x0e] = 0x3e;
      info.spd[0][0][0x0f] = 0x00;
      info.spd[0][0][0x10] = 0x69;
      info.spd[0][0][0x11] = 0x78;
      info.spd[0][0][0x12] = 0x69;
      info.spd[0][0][0x13] = 0x30;
      info.spd[0][0][0x14] = 0x69;
      info.spd[0][0][0x15] = 0x11;
      info.spd[0][0][0x16] = 0x20;
      info.spd[0][0][0x17] = 0x89;
      info.spd[0][0][0x18] = 0x00;
      info.spd[0][0][0x19] = 0x05;
      info.spd[0][0][0x1a] = 0x3c;
      info.spd[0][0][0x1b] = 0x3c;
      info.spd[0][0][0x1c] = 0x00;
      info.spd[0][0][0x1d] = 0xf0;
      info.spd[0][0][0x1f] = 0x05;
      info.spd[0][0][0x20] = 0x00;
      info.spd[0][0][0x3e] = 0x05;
      info.spd[0][0][0x3f] = 0x00;
      info.spd[0][0][0x75] = 0x80;
      info.spd[0][0][0x76] = 0xad;
      info.spd[0][0][0x77] = 0x01;
      info.spd[0][0][0x78] = 0x10;
      info.spd[0][0][0x79] = 0x52;
      info.spd[0][0][0x7a] = 0x26;
      info.spd[0][0][0x7b] = 0x50;
      info.spd[0][0][0x7c] = 0xf4;
      info.spd[0][0][0x7d] = 0x7d;
      info.spd[0][0][0x7e] = 0xb0;
      info.spd[0][0][0x7f] = 0xcf;
      info.spd[0][0][0x80] = 0x48;
      info.spd[0][0][0x81] = 0x4d;
      info.spd[0][0][0x82] = 0x54;
      info.spd[0][0][0x83] = 0x33;
      info.spd[0][0][0x84] = 0x35;
      info.spd[0][0][0x85] = 0x31;
      info.spd[0][0][0x86] = 0x53;
      info.spd[0][0][0x87] = 0x36;
      info.spd[0][0][0x88] = 0x42;
      info.spd[0][0][0x89] = 0x46;
      info.spd[0][0][0x8a] = 0x52;
      info.spd[0][0][0x8b] = 0x38;
      info.spd[0][0][0x8c] = 0x43;
      info.spd[0][0][0x8d] = 0x2d;
      info.spd[0][0][0x8e] = 0x48;
      info.spd[0][0][0x8f] = 0x39;
      info.spd[0][0][0x90] = 0x20;
      info.spd[0][0][0x91] = 0x20;
      info.spd[0][0][0x92] = 0x4e;
      info.spd[0][0][0x93] = 0x30;
      info.spd[0][0][0x94] = 0x80;
      info.spd[0][0][0x95] = 0xad;
#else
      info.populated_ranks[0][0][0] = 1;
      info.populated_ranks[0][0][1] = 1;
      info.populated_ranks[1][0][0] = 1;
      info.populated_ranks[1][0][1] = 1;
      info.populated_ranks_mask[0] = 3;
      info.populated_ranks_mask[1] = 3;
      info.use_ecc = 0;
      info.max_slots_used_in_channel = 1;

      info.spd[0][0][0x02] = 0x0b;
      info.spd[0][0][0x03] = 0x03;
      info.spd[0][0][0x04] = 0x03;
      info.spd[0][0][0x07] = 0x09;
      info.spd[0][0][0x08] = 0x03;
      info.spd[0][0][0x0a] = 0x01;
      info.spd[0][0][0x0b] = 0x08;
      info.spd[0][0][0x0c] = 0x0c;
      info.spd[0][0][0x0e] = 0x3e;
      info.spd[0][0][0x0f] = 0x00;
      info.spd[0][0][0x10] = 0x69;
      info.spd[0][0][0x11] = 0x78;
      info.spd[0][0][0x12] = 0x69;
      info.spd[0][0][0x13] = 0x30;
      info.spd[0][0][0x14] = 0x69;
      info.spd[0][0][0x15] = 0x11;
      info.spd[0][0][0x16] = 0x20;
      info.spd[0][0][0x17] = 0x89;
      info.spd[0][0][0x18] = 0x00;
      info.spd[0][0][0x19] = 0x05;
      info.spd[0][0][0x1a] = 0x3c;
      info.spd[0][0][0x1b] = 0x3c;
      info.spd[0][0][0x1c] = 0x00;
      info.spd[0][0][0x1d] = 0xf0;
      info.spd[0][0][0x1f] = 0x05;
      info.spd[0][0][0x20] = 0x00;
      info.spd[0][0][0x3e] = 0x05;
      info.spd[0][0][0x3f] = 0x00;
      info.spd[0][0][0x75] = 0x80;
      info.spd[0][0][0x76] = 0xad;
      info.spd[0][0][0x77] = 0x01;
      info.spd[0][0][0x78] = 0x10;
      info.spd[0][0][0x79] = 0x52;
      info.spd[0][0][0x7a] = 0x26;
      info.spd[0][0][0x7b] = 0x50;
      info.spd[0][0][0x7c] = 0xf4;
      info.spd[0][0][0x7d] = 0x7d;
      info.spd[0][0][0x7e] = 0xb0;
      info.spd[0][0][0x7f] = 0xcf;
      info.spd[0][0][0x80] = 0x48;
      info.spd[0][0][0x81] = 0x4d;
      info.spd[0][0][0x82] = 0x54;
      info.spd[0][0][0x83] = 0x33;
      info.spd[0][0][0x84] = 0x35;
      info.spd[0][0][0x85] = 0x31;
      info.spd[0][0][0x86] = 0x53;
      info.spd[0][0][0x87] = 0x36;
      info.spd[0][0][0x88] = 0x42;
      info.spd[0][0][0x89] = 0x46;
      info.spd[0][0][0x8a] = 0x52;
      info.spd[0][0][0x8b] = 0x38;
      info.spd[0][0][0x8c] = 0x43;
      info.spd[0][0][0x8d] = 0x2d;
      info.spd[0][0][0x8e] = 0x48;
      info.spd[0][0][0x8f] = 0x39;
      info.spd[0][0][0x90] = 0x20;
      info.spd[0][0][0x91] = 0x20;
      info.spd[0][0][0x92] = 0x4e;
      info.spd[0][0][0x93] = 0x30;
      info.spd[0][0][0x94] = 0x80;
      info.spd[0][0][0x95] = 0xad;

      info.spd[1][0][0x02] = 0x0b;
      info.spd[1][0][0x03] = 0x03;
      info.spd[1][0][0x04] = 0x03;
      info.spd[1][0][0x07] = 0x09;
      info.spd[1][0][0x08] = 0x03;
      info.spd[1][0][0x0a] = 0x01;
      info.spd[1][0][0x0b] = 0x08;
      info.spd[1][0][0x0c] = 0x0c;
      info.spd[1][0][0x0e] = 0x3e;
      info.spd[1][0][0x0f] = 0x00;
      info.spd[1][0][0x10] = 0x69;
      info.spd[1][0][0x11] = 0x78;
      info.spd[1][0][0x12] = 0x69;
      info.spd[1][0][0x13] = 0x30;
      info.spd[1][0][0x14] = 0x69;
      info.spd[1][0][0x15] = 0x11;
      info.spd[1][0][0x16] = 0x20;
      info.spd[1][0][0x17] = 0x89;
      info.spd[1][0][0x18] = 0x00;
      info.spd[1][0][0x19] = 0x05;
      info.spd[1][0][0x1a] = 0x3c;
      info.spd[1][0][0x1b] = 0x3c;
      info.spd[1][0][0x1c] = 0x00;
      info.spd[1][0][0x1d] = 0xf0;
      info.spd[1][0][0x1f] = 0x05;
      info.spd[1][0][0x20] = 0x00;
      info.spd[1][0][0x3e] = 0x05;
      info.spd[1][0][0x3f] = 0x00;
      info.spd[1][0][0x75] = 0x80;
      info.spd[1][0][0x76] = 0xad;
      info.spd[1][0][0x77] = 0x01;
      info.spd[1][0][0x78] = 0x10;
      info.spd[1][0][0x79] = 0x52;
      info.spd[1][0][0x7a] = 0x26;
      info.spd[1][0][0x7b] = 0x50;
      info.spd[1][0][0x7c] = 0xf4;
      info.spd[1][0][0x7d] = 0x7d;
      info.spd[1][0][0x7e] = 0xb0;
      info.spd[1][0][0x7f] = 0xcf;
      info.spd[1][0][0x80] = 0x48;
      info.spd[1][0][0x81] = 0x4d;
      info.spd[1][0][0x82] = 0x54;
      info.spd[1][0][0x83] = 0x33;
      info.spd[1][0][0x84] = 0x35;
      info.spd[1][0][0x85] = 0x31;
      info.spd[1][0][0x86] = 0x53;
      info.spd[1][0][0x87] = 0x36;
      info.spd[1][0][0x88] = 0x42;
      info.spd[1][0][0x89] = 0x46;
      info.spd[1][0][0x8a] = 0x52;
      info.spd[1][0][0x8b] = 0x38;
      info.spd[1][0][0x8c] = 0x43;
      info.spd[1][0][0x8d] = 0x2d;
      info.spd[1][0][0x8e] = 0x48;
      info.spd[1][0][0x8f] = 0x39;
      info.spd[1][0][0x90] = 0x20;
      info.spd[1][0][0x91] = 0x20;
      info.spd[1][0][0x92] = 0x4e;
      info.spd[1][0][0x93] = 0x30;
      info.spd[1][0][0x94] = 0x80;
      info.spd[1][0][0x95] = 0xad;

#endif

      info.is_x16_module[0][0] = (info.spd[0][0][RANKS_AND_DQ] & 7) - 1;
      info.density[0][0] = info.spd[0][0][DENSITY] & 0xF;

      info.is_x16_module[1][0] = (info.spd[1][0][RANKS_AND_DQ] & 7) - 1;
      info.density[1][0] = info.spd[1][0][DENSITY] & 0xF;
    }

  write_mchbar8 (0x2ca8, read_mchbar8 (0x2ca8) & 0xfc);
#if REAL
  write_mchbar8 (0x2ca8, 8);
#endif
  rdmsr (0x207); // !!!

  collect_system_info (&info);
  calculate_timings (&info);

#if !REAL
  pci_mm_write8 (NORTHBRIDGE, 0xdf, 0x82);
#endif

  if (!s3resume)
    gav (pci_mm_read8 (SOUTHBRIDGE, 0xa2));

  gav (read_mchbar8 (0x2ca8)); ///!!!!

  if (!s3resume && x2ca8 == 0)
    pci_mm_write8 (SOUTHBRIDGE, 0xa2, pci_mm_read8 (SOUTHBRIDGE, 0xa2) | 0x80);

  compute_derived_timings (&info);

  if (x2ca8 == 0)
    {
      gav (read_mchbar8 (0x164));
      write_mchbar8 (0x164, 0x26);
      write_mchbar16 (0x2c20, 0x10);
    }

  write_mchbar32 (0x18b4, read_mchbar32 (0x18b4) | 0x210000); /* OK */
  write_mchbar32 (0x1890, read_mchbar32 (0x1890) | 0x2000000); /* OK */
  write_mchbar32 (0x18b4, read_mchbar32 (0x18b4) | 0x8000);
	
  gav (pci_mm_read32 (0xff, 2, 1, 0x50)); // !!!!
  pci_mm_write8 (0xff, 2, 1, 0x54, 0x12);

  gav (read_mchbar16 (0x2c10)); // !!!!
  write_mchbar16 (0x2c10, 0x412);
  gav (read_mchbar16 (0x2c10)); // !!!!
  write_mchbar16 (0x2c12, read_mchbar16 (0x2c12) | 0x100); /* OK */

  gav (read_mchbar8 (0x2ca8)); // !!!!
  write_mchbar32 (0x1804, (read_mchbar32 (0x1804) & 0xfffffffc) | 0x8400080);

  pci_mm_read32 (0xff, 2, 1, 0x6c); // !!!!
  pci_mm_write32 (0xff, 2, 1, 0x6c, 0x40a0a0);
  gav (read_mchbar32 (0x1c04)); // !!!!
  gav (read_mchbar32 (0x1804)); // !!!!

  if (x2ca8 == 0)
    {
      write_mchbar8 (0x2ca8, read_mchbar8 (0x2ca8) | 1);
    }

  write_mchbar32 (0x18d8, 0x120000);
  write_mchbar32 (0x18dc, 0x30a484a);
  pci_mm_write32 (0xff, 2, 1, 0xe0, 0x0);
  pci_mm_write32 (0xff, 2, 1, 0xf4, 0x9444a);
  write_mchbar32 (0x18d8, 0x40000);
  write_mchbar32 (0x18dc, 0xb000000);
  pci_mm_write32 (0xff, 2, 1, 0xe0, 0x60000);
  pci_mm_write32 (0xff, 2, 1, 0xf4, 0x0);
  write_mchbar32 (0x18d8, 0x180000);
  write_mchbar32 (0x18dc, 0xc0000142);
  pci_mm_write32 (0xff, 2, 1, 0xe0, 0x20000);
  pci_mm_write32 (0xff, 2, 1, 0xf4, 0x142);
  write_mchbar32 (0x18d8, 0x1e0000);
 
  gav (read_mchbar32 (0x18dc)); // !!!!
  write_mchbar32 (0x18dc, 0x3);
  gav (read_mchbar32 (0x18dc)); // !!!!

  if (x2ca8 == 0)
    {
      write_mchbar8 (0x2ca8, read_mchbar8 (0x2ca8) | 1); // guess
    }

  write_mchbar32 (0x188c, 0x20bc09);
  pci_mm_write32 (0xff, 2, 1, 0xd0, 0x40b0c09);
  write_mchbar32 (0x1a10, 0x4200010e);
  write_mchbar32 (0x18b8, read_mchbar32 (0x18b8) | 0x200);
  gav (read_mchbar32 (0x1918)); // !!!!
  write_mchbar32 (0x1918, 0x332);

  gav (read_mchbar32 (0x18b8)); // !!!!
  write_mchbar32 (0x18b8, 0xe00);
  gav (read_mchbar32 (0x182c)); // !!!!
  write_mchbar32 (0x182c, 0x10202);
  gav (pci_mm_read32 (0xff, 2, 1, 0x94)); // !!!!
  pci_mm_write32 (0xff, 2, 1, 0x94, 0x10202);
  write_mchbar32 (0x1a1c, read_mchbar32 (0x1a1c) & 0x8fffffff);
  write_mchbar32 (0x1a70, read_mchbar32 (0x1a70) | 0x100000);

  write_mchbar32 (0x18b4, read_mchbar32 (0x18b4) & 0xffff7fff);
  gav (read_mchbar32 (0x1a68)); // !!!!
  write_mchbar32 (0x1a68, 0x343800);
  gav (read_mchbar32 (0x1e68)); // !!!!
  gav (read_mchbar32 (0x1a68)); // !!!!

  if (x2ca8 == 0)
    {
      write_mchbar8 (0x2ca8, read_mchbar8 (0x2ca8) | 1); // guess
    }

  pci_mm_read32 (0xff, 2, 0, 0x048); // !!!!
  pci_mm_write32 (0xff, 2, 0, 0x048, 0x140000);
  pci_mm_read32 (0xff, 2, 0, 0x058); // !!!!
  pci_mm_write32 (0xff, 2, 0, 0x058, 0x64555);
  pci_mm_read32 (0xff, 2, 0, 0x058); // !!!!
  pci_mm_read32 (0xff, 0, 0, 0xd0); // !!!!
  pci_mm_write32 (0xff, 0, 0, 0xd0, 0x180);
  gav (read_mchbar32 (0x1af0)); // !!!!
  gav (read_mchbar32 (0x1af0)); // !!!!
  write_mchbar32 (0x1af0, 0x1f020003);
  gav (read_mchbar32 (0x1af0)); // !!!!

  if (((x2ca8 == 0)))
    {
      write_mchbar8 (0x2ca8, read_mchbar8 (0x2ca8) | 1); // guess
    }

  gav (read_mchbar32 (0x1890)); // !!!!
  write_mchbar32 (0x1890, 0x80102);
  gav (read_mchbar32 (0x18b4)); // !!!!
  write_mchbar32 (0x18b4, 0x216000);
  write_mchbar32 (0x18a4, 0x22222222);
  write_mchbar32 (0x18a8, 0x22222222);
  write_mchbar32 (0x18ac, 0x22222);

  if (x2ca8 == 0)
    {
      int b144[2];
      if (!s3resume)
	for (channel = 0; channel < 2; channel++)
	  {
	    gav (read_mchbar8 (0x2ca8));
	    write_mchbar32 (0x140, (read_mchbar32 (0x140) & 0xf8ffffff) | 0x2000000);
	    write_mchbar32 (0x138, (read_mchbar32 (0x138) & 0xf8ffffff) | 0x2000000);
	    gav (b144[channel] = (read_mchbar8 (0x144) >> 4) & 1);
	    write_mchbar32 (0x274 + (channel << 10), 0x50005);
	    write_mchbar16 (0x265 + (channel << 10), 0xe00);
	  }
      else
	{
	  write_mchbar32 (0x274, 0x30005);
	  write_mchbar16 (0x265, 0xd00);
	  write_mchbar32 (0x674, info.populated_ranks[1][0][0] ? 0x40005 : 0x20004);
	  write_mchbar16 (0x665, info.populated_ranks[1][0][0] ? 0xd00 : 0xc00);
	  b144[0] = 0;
	  b144[1] = 1;
	}

      write_mchbar8 (0x2ca9, read_mchbar8 (0x2ca9) & ~1);
      write_mchbar32 (0x2d04, 0x1);
      write_mchbar32 (0x2d00, 0x1010000);
      write_mchbar32 (0x2d0c, 0x0);
      write_mchbar32 (0x2d08, 0x1110000);
      write_mchbar32 (0x2d14, 0x5);
      write_mchbar32 (0x2d10, 0x401fdff);
      write_mchbar32 (0x2d1c, 0x101);
      write_mchbar32 (0x2d18, 0x811f9fc);
      write_mchbar32 (0x2d24, 0x105);
      write_mchbar32 (0x2d20, 0x410fcff);
      write_mchbar32 (0x2d2c, 0x7);
      write_mchbar32 (0x2d28, 0x911f1fb);
      write_mchbar32 (0x2d34, 0x101);
      write_mchbar32 (0x2d30, 0x811fdfa);
      write_mchbar32 (0x2d3c, 0x105);
      write_mchbar32 (0x2d38, 0x4200000);
      write_mchbar32 (0x2d44, 0x107);
      write_mchbar32 (0x2d40, 0x2200000);
      write_mchbar32 (0x2d4c, 0x107);
      write_mchbar32 (0x2d48, 0x2200000);
      write_mchbar32 (0x2d54, 0x105);
      write_mchbar32 (0x2d50, 0x4200000);

      write_mchbar32 (0x6d4, 0x100f8);
      write_mchbar32 (0x6d8, 0x1100f8);
      if (s3resume)
	write_mchbar32 (0x6dc, info.populated_ranks[1][0][0] ? 0x33fefa : 0x33fffa);
      else
	write_mchbar32 (0x6dc, b144[0] ? 0x23faff : 0x22fdf9);
#if !REAL
      printf ("CP13\n");
#endif

      write_mchbar32 (0x6e0, 0x100fa);
      write_mchbar32 (0x6e4, 0x0);
      if (s3resume && info.populated_ranks[1][0][0])
	write_mchbar32 (0x6e8, 0x22fcf8);
      else
	write_mchbar32 (0x6e8, b144[1] ? 0x23faff : 0x22fdf9);

      write_mchbar32 (0x2d5c, 0x1010001);
      write_mchbar32 (0x2d58, 0x1110000);
      write_mchbar32 (0x2d64, 0x2020104);
      write_mchbar32 (0x2d60, 0x211fffe);
      write_mchbar32 (0x2d6c, 0x117);
      write_mchbar32 (0x2d68, 0x430e7ee);
      write_mchbar32 (0x2d74, 0x10100126);
      write_mchbar32 (0x2d70, 0x1010f0ff);
      write_mchbar32 (0x2d7c, 0x113);
      write_mchbar32 (0x2d78, 0x810ebf1);
      write_mchbar32 (0x2d84, 0xa0a010d);
      write_mchbar32 (0x2d80, 0x510fbff);
      write_mchbar32 (0x2d8c, 0xa0a010d);
      write_mchbar32 (0x2d88, 0x510fbff);
      write_mchbar32 (0x2da4, 0x8080016);
      write_mchbar32 (0x2da0, 0x800fff2);
      write_mchbar32 (0x2dac, 0x1b1b0025);
      write_mchbar32 (0x2da8, 0x1b11e7ec);
      write_mchbar32 (0x2db4, 0x808012e);
      write_mchbar32 (0x2db0, 0x830e8fd);
      write_mchbar32 (0x2dbc, 0xe000ef);
      write_mchbar32 (0x2db8, 0x10d0077);
      if (!(deven & 8))
	{
	  read_mchbar32 (0x2cb0);
	  write_mchbar32 (0x2cb0, 0x40);
	}
	
      if (deven & 8)
	{
	  write_mchbar32 (0xff8, 0x1800 | read_mchbar32 (0xff8));
	  read_mchbar32 (0x2cb0);
	  write_mchbar32 (0x2cb0, 0x00);
	  pci_mm_read8 (0, 0x2, 0x0, 0x4c);
	  pci_mm_read8 (0, 0x2, 0x0, 0x4c);
	  pci_mm_read8 (0, 0x2, 0x0, 0x4e);

	  read_mchbar8 (0x1150);
	  read_mchbar8 (0x1151);
	  read_mchbar8 (0x1022);
	  read_mchbar8 (0x16d0);
	  write_mchbar32 (0x1300, 0x60606060);
	  write_mchbar32 (0x1304, 0x60606060);
	  write_mchbar32 (0x1308, 0x78797a7b);
	  write_mchbar32 (0x130c, 0x7c7d7e7f);
	  write_mchbar32 (0x1310, 0x60606060);
	  write_mchbar32 (0x1314, 0x60606060);
	  write_mchbar32 (0x1318, 0x60606060);
	  write_mchbar32 (0x131c, 0x60606060);
	  write_mchbar32 (0x1320, 0x50515253);
	  write_mchbar32 (0x1324, 0x54555657);
	  write_mchbar32 (0x1328, 0x58595a5b);
	  write_mchbar32 (0x132c, 0x5c5d5e5f);
	  write_mchbar32 (0x1330, 0x40414243);
	  write_mchbar32 (0x1334, 0x44454647);
	  write_mchbar32 (0x1338, 0x48494a4b);
	  write_mchbar32 (0x133c, 0x4c4d4e4f);
	  write_mchbar32 (0x1340, 0x30313233);
	  write_mchbar32 (0x1344, 0x34353637);
	  write_mchbar32 (0x1348, 0x38393a3b);
	  write_mchbar32 (0x134c, 0x3c3d3e3f);
	  write_mchbar32 (0x1350, 0x20212223);
	  write_mchbar32 (0x1354, 0x24252627);
	  write_mchbar32 (0x1358, 0x28292a2b);
	  write_mchbar32 (0x135c, 0x2c2d2e2f);
	  write_mchbar32 (0x1360, 0x10111213);
	  write_mchbar32 (0x1364, 0x14151617);
	  write_mchbar32 (0x1368, 0x18191a1b);
	  write_mchbar32 (0x136c, 0x1c1d1e1f);
	  write_mchbar32 (0x1370, 0x10203);
	  write_mchbar32 (0x1374, 0x4050607);
	  write_mchbar32 (0x1378, 0x8090a0b);
	  write_mchbar32 (0x137c, 0xc0d0e0f);
	  write_mchbar8 (0x11cc, 0x4e);
	  write_mchbar32 (0x1110, 0x73970404);
	  write_mchbar32 (0x1114, 0x72960404);
	  write_mchbar32 (0x1118, 0x6f950404);
	  write_mchbar32 (0x111c, 0x6d940404);
	  write_mchbar32 (0x1120, 0x6a930404);
	  write_mchbar32 (0x1124, 0x68a41404);
	  write_mchbar32 (0x1128, 0x66a21404);
	  write_mchbar32 (0x112c, 0x63a01404);
	  write_mchbar32 (0x1130, 0x609e1404);
	  write_mchbar32 (0x1134, 0x5f9c1404);
	  write_mchbar32 (0x1138, 0x5c961404);
	  write_mchbar32 (0x113c, 0x58a02404);
	  write_mchbar32 (0x1140, 0x54942404);
	  write_mchbar32 (0x1190, 0x900080a);
	  write_mchbar16 (0x11c0, 0xc40b);
	  write_mchbar16 (0x11c2, 0x303);
	  write_mchbar16 (0x11c4, 0x301);
	  read_mchbar32 (0x1190);
	  write_mchbar32 (0x1190, 0x8900080a);
	  write_mchbar32 (0x11b8, 0x70c3000);
	  write_mchbar8 (0x11ec, 0xa);
	  write_mchbar16 (0x1100, 0x800);
	  read_mchbar32 (0x11bc);
	  write_mchbar32 (0x11bc, 0x1e84800);
	  write_mchbar16 (0x11ca, 0xfa);
	  write_mchbar32 (0x11e4, 0x4e20);
	  write_mchbar8 (0x11bc, 0xf);
	  write_mchbar16 (0x11da, 0x19);
	  write_mchbar16 (0x11ba, 0x470c);
	  write_mchbar32 (0x1680, 0xe6ffe4ff);
	  write_mchbar32 (0x1684, 0xdeffdaff);
	  write_mchbar32 (0x1688, 0xd4ffd0ff);
	  write_mchbar32 (0x168c, 0xccffc6ff);
	  write_mchbar32 (0x1690, 0xc0ffbeff);
	  write_mchbar32 (0x1694, 0xb8ffb0ff);
	  write_mchbar32 (0x1698, 0xa8ff0000);
	  write_mchbar32 (0x169c, 0xc00);
	  write_mchbar32 (0x1290, 0x5000000);
	}

      write_mchbar32 (0x124c, 0x15040d00);
      write_mchbar32 (0x1250, 0x7f0000);
      write_mchbar32 (0x1254, 0x1e220004);
      write_mchbar32 (0x1258, 0x4000004);
      write_mchbar32 (0x1278, 0x0);
      write_mchbar32 (0x125c, 0x0);
      write_mchbar32 (0x1260, 0x0);
      write_mchbar32 (0x1264, 0x0);
      write_mchbar32 (0x1268, 0x0);
      write_mchbar32 (0x126c, 0x0);
      write_mchbar32 (0x1270, 0x0);
      write_mchbar32 (0x1274, 0x0);
    }

  if ((deven & 8) && x2ca8 == 0)
    {
      write_mchbar16 (0x1214, 0x320);
      write_mchbar32 (0x1600, 0x40000000);
      read_mchbar32 (0x11f4);
      write_mchbar32 (0x11f4, 0x10000000);
      read_mchbar16 (0x1230);
      write_mchbar16 (0x1230, 0x8000);
      write_mchbar32 (0x1400, 0x13040020);
      write_mchbar32 (0x1404, 0xe090120);
      write_mchbar32 (0x1408, 0x5120220);
      write_mchbar32 (0x140c, 0x5120330);
      write_mchbar32 (0x1410, 0xe090220);
      write_mchbar32 (0x1414, 0x1010001);
      write_mchbar32 (0x1418, 0x1110000);
      write_mchbar32 (0x141c, 0x9020020);
      write_mchbar32 (0x1420, 0xd090220);
      write_mchbar32 (0x1424, 0x2090220);
      write_mchbar32 (0x1428, 0x2090330);
      write_mchbar32 (0x142c, 0xd090220);
      write_mchbar32 (0x1430, 0x1010001);
      write_mchbar32 (0x1434, 0x1110000);
      write_mchbar32 (0x1438, 0x11040020);
      write_mchbar32 (0x143c, 0x4030220);
      write_mchbar32 (0x1440, 0x1060220);
      write_mchbar32 (0x1444, 0x1060330);
      write_mchbar32 (0x1448, 0x4030220);
      write_mchbar32 (0x144c, 0x1010001);
      write_mchbar32 (0x1450, 0x1110000);
      write_mchbar32 (0x1454, 0x4010020);
      write_mchbar32 (0x1458, 0xb090220);
      write_mchbar32 (0x145c, 0x1090220);
      write_mchbar32 (0x1460, 0x1090330);
      write_mchbar32 (0x1464, 0xb090220);
      write_mchbar32 (0x1468, 0x1010001);
      write_mchbar32 (0x146c, 0x1110000);
      write_mchbar32 (0x1470, 0xf040020);
      write_mchbar32 (0x1474, 0xa090220);
      write_mchbar32 (0x1478, 0x1120220);
      write_mchbar32 (0x147c, 0x1120330);
      write_mchbar32 (0x1480, 0xa090220);
      write_mchbar32 (0x1484, 0x1010001);
      write_mchbar32 (0x1488, 0x1110000);
      write_mchbar32 (0x148c, 0x7020020);
      write_mchbar32 (0x1490, 0x1010220);
      write_mchbar32 (0x1494, 0x10210);
      write_mchbar32 (0x1498, 0x10320);
      write_mchbar32 (0x149c, 0x1010220);
      write_mchbar32 (0x14a0, 0x1010001);
      write_mchbar32 (0x14a4, 0x1110000);
      write_mchbar32 (0x14a8, 0xd040020);
      write_mchbar32 (0x14ac, 0x8090220);
      write_mchbar32 (0x14b0, 0x1111310);
      write_mchbar32 (0x14b4, 0x1111420);
      write_mchbar32 (0x14b8, 0x8090220);
      write_mchbar32 (0x14bc, 0x1010001);
      write_mchbar32 (0x14c0, 0x1110000);
      write_mchbar32 (0x14c4, 0x3010020);
      write_mchbar32 (0x14c8, 0x7090220);
      write_mchbar32 (0x14cc, 0x1081310);
      write_mchbar32 (0x14d0, 0x1081420);
      write_mchbar32 (0x14d4, 0x7090220);
      write_mchbar32 (0x14d8, 0x1010001);
      write_mchbar32 (0x14dc, 0x1110000);
      write_mchbar32 (0x14e0, 0xb040020);
      write_mchbar32 (0x14e4, 0x2030220);
      write_mchbar32 (0x14e8, 0x1051310);
      write_mchbar32 (0x14ec, 0x1051420);
      write_mchbar32 (0x14f0, 0x2030220);
      write_mchbar32 (0x14f4, 0x1010001);
      write_mchbar32 (0x14f8, 0x1110000);
      write_mchbar32 (0x14fc, 0x5020020);
      write_mchbar32 (0x1500, 0x5090220);
      write_mchbar32 (0x1504, 0x2071310);
      write_mchbar32 (0x1508, 0x2071420);
      write_mchbar32 (0x150c, 0x5090220);
      write_mchbar32 (0x1510, 0x1010001);
      write_mchbar32 (0x1514, 0x1110000);
      write_mchbar32 (0x1518, 0x7040120);
      write_mchbar32 (0x151c, 0x2090220);
      write_mchbar32 (0x1520, 0x70b1210);
      write_mchbar32 (0x1524, 0x70b1310);
      write_mchbar32 (0x1528, 0x2090220);
      write_mchbar32 (0x152c, 0x1010001);
      write_mchbar32 (0x1530, 0x1110000);
      write_mchbar32 (0x1534, 0x1010110);
      write_mchbar32 (0x1538, 0x1081310);
      write_mchbar32 (0x153c, 0x5041200);
      write_mchbar32 (0x1540, 0x5041310);
      write_mchbar32 (0x1544, 0x1081310);
      write_mchbar32 (0x1548, 0x1010001);
      write_mchbar32 (0x154c, 0x1110000);
      write_mchbar32 (0x1550, 0x1040120);
      write_mchbar32 (0x1554, 0x4051210);
      write_mchbar32 (0x1558, 0xd051200);
      write_mchbar32 (0x155c, 0xd051200);
      write_mchbar32 (0x1560, 0x4051210);
      write_mchbar32 (0x1564, 0x1010001);
      write_mchbar32 (0x1568, 0x1110000);
      write_mchbar16 (0x1222, 0x220a);
      write_mchbar16 (0x123c, 0x1fc0);
      write_mchbar16 (0x1220, 0x1388);
    }

  read_mchbar32 (0x2c80); // !!!!
  write_mchbar32 (0x2c80, 0x1053688);
  read_mchbar32 (0x1c04); // !!!!
  write_mchbar32 (0x1804, 0x406080);

  read_mchbar8 (0x2ca8);

  if (x2ca8 == 0)
    {
      write_mchbar8 (0x2ca8, read_mchbar8 (0x2ca8) & ~3);
      write_mchbar8 (0x2ca8, read_mchbar8 (0x2ca8) + 4);
      write_mchbar32 (0x1af0, read_mchbar32 (0x1af0) | 0x10);
#if REAL
      while (1)
	{
	  asm volatile ("hlt");
	}
#else
      printf ("CP5\n");
      exit (0);
#endif
    }

  write_mchbar8 (0x2ca8, read_mchbar8 (0x2ca8));
  read_mchbar32 (0x2c80); // !!!!
  write_mchbar32 (0x2c80, 0x53688);
  pci_mm_write32 (0xff, 0, 0, 0x60, 0x20220);
  read_mchbar16 (0x2c20); // !!!!
  read_mchbar16 (0x2c10); // !!!!
  read_mchbar16 (0x2c00); // !!!!
  write_mchbar16 (0x2c00, 0x8c0);
  write_1d0 (0, 0x33d, 0, 0);
  write_500 (&info, 0, 0, 0xb61, 0, 0);
  write_500 (&info, 1, 0, 0xb61, 0, 0);
  write_mchbar32 (0x1a30, 0x0);
  write_mchbar32 (0x1a34, 0x0);
  write_mchbar16 (0x614, 0xb5b | (info.populated_ranks[1][0][0] * 0x404) | (info.populated_ranks[0][0][0] * 0xa0));
  write_mchbar16 (0x616, 0x26a);
  write_mchbar32 (0x134, 0x856000);
  write_mchbar32 (0x160, 0x5ffffff);
  read_mchbar32 (0x114); // !!!!
  write_mchbar32 (0x114, 0xc2024440);
  read_mchbar32 (0x118); // !!!!
  write_mchbar32 (0x118, 0x4);
  for (channel = 0; channel < NUM_CHANNELS; channel++)
    write_mchbar32 (0x260 + (channel << 10), 0x30809ff | ((info.populated_ranks_mask[channel] & 3) << 20));
  for (channel = 0; channel < NUM_CHANNELS; channel++)
    {
      write_mchbar16 (0x31c + (channel << 10), 0x101);
      write_mchbar16 (0x360 + (channel << 10), 0x909);
      write_mchbar16 (0x3a4 + (channel << 10), 0x101);
      write_mchbar16 (0x3e8 + (channel << 10), 0x101);
      write_mchbar32 (0x320 + (channel << 10), 0x29002900);
      write_mchbar32 (0x324 + (channel << 10), 0x0);
      write_mchbar32 (0x368 + (channel << 10), 0x32003200);
      write_mchbar16 (0x352 + (channel << 10), 0x505);
      write_mchbar16 (0x354 + (channel << 10), 0x3c3c);
      write_mchbar16 (0x356 + (channel << 10), 0x1040);
      write_mchbar16 (0x39a + (channel << 10), 0x73e4);
      write_mchbar16 (0x3de + (channel << 10), 0x77ed);
      write_mchbar16 (0x422 + (channel << 10), 0x1040);
    }

  write_1d0 (0x4, 0x151, 4, 1);
  write_1d0 (0, 0x142, 3, 1);
  rdmsr (0x1ac); // !!!!
  write_500 (&info, 1, 1, 0x6b3, 4, 1);
  write_500 (&info, 1, 1, 0x6cf, 4, 1);

  rmw_1d0 (0x21c, 0x38, 0, 6, 1);

  write_1d0 (((!info.populated_ranks[1][0][0]) << 1) | ((!info.populated_ranks[0][0][0]) << 0), 0x1d1, 3, 1);
  for (channel = 0; channel < NUM_CHANNELS; channel++)
    {
      write_mchbar16 (0x38e + (channel << 10), 0x5f5f);
      write_mchbar16 (0x3d2 + (channel << 10), 0x5f5f);
    }

  set_334 (0);

  program_base_timings (&info);

  write_mchbar8 (0x5ff, read_mchbar8 (0x5ff) | 0x80); /* OK */

  write_1d0 (0x2, 0x1d5, 2, 1);
  write_1d0 (0x20, 0x166, 7, 1);
  write_1d0 (0x0, 0xeb, 3, 1);
  write_1d0 (0x0, 0xf3, 6, 1);

  for (channel = 0; channel < NUM_CHANNELS; channel++)
    for (lane = 0; lane < 9; lane++)
      {
	u16 addr = 0x125 + get_lane_offset (0, 0, lane);
	u8 a;
	a = read_500 (&info, channel, addr, 6); // = 0x20040080 //!!!!
	write_500 (&info, channel, a, addr, 6, 1);
      }

  if (s3resume)
    {
      const struct ram_training *train;

      train = get_cached_training();
      if (train == NULL)
	{
	  u32 reg32;
	  printk (BIOS_ERR, "Couldn't find training data. Rebooting\n");
	  reg32 = inl(DEFAULT_PMBASE + 0x04);
	  outl(reg32 & ~(7 << 10), DEFAULT_PMBASE + 0x04);
	  outb (0xe, 0xcf9);

#if REAL
	  while (1)
	    {
	      asm volatile ("hlt");
	    }
#else
	  printf ("CP5\n");
	  exit (0);
#endif
	}
      int tm;
      info.training =  *train;
      for (tm = 0; tm < 4; tm++)
	for (channel = 0; channel < NUM_CHANNELS; channel++)
	  for (slot = 0; slot < NUM_SLOTS; slot++)
	    for (rank = 0; rank < NUM_RANKS; rank++)
	      for (lane = 0; lane < 9; lane++)
		write_500 (&info, channel, info.training.lane_timings[tm][channel][slot][rank][lane], get_timing_register_addr (lane, tm, slot, rank), 9, 0);
      write_1d0 (train->reg_178, 0x178, 7, 1);
      write_1d0 (train->reg_10b, 0x10b, 6, 1);
    }

  read_mchbar32 (0x1f4); // !!!!
  write_mchbar32 (0x1f4, 0x20000);
  write_mchbar32 (0x1f0, 0x1d000200);
  read_mchbar8 (0x1f0); // !!!!
  write_mchbar8 (0x1f0, 0x1);
  read_mchbar8 (0x1f0); // !!!!

  program_board_delay(&info);

  write_mchbar8 (0x5ff, 0x0); /* OK */
  write_mchbar8 (0x5ff, 0x80); /* OK */
  write_mchbar8 (0x5f4, 0x1); /* OK */

  write_mchbar32 (0x130, read_mchbar32 (0x130) & 0xfffffffd); // | 2 when ?
  while (read_mchbar32 (0x130) & 1);
  gav (read_1d0 (0x14b, 7)); // = 0x81023100
  write_1d0 (0x30, 0x14b, 7, 1);
  read_1d0 (0xd6, 6); // = 0xfa008080 // !!!!
  write_1d0 (7, 0xd6, 6, 1);
  read_1d0 (0x328, 6); // = 0xfa018080 // !!!!
  write_1d0 (7, 0x328, 6, 1);

  for (channel = 0; channel < NUM_CHANNELS; channel++)
    set_4cf (&info, channel, info.populated_ranks[channel][0][0] ? 8 : 0);

  read_1d0 (0x116, 4); // = 0x4040432 // !!!!
  write_1d0 (2, 0x116, 4, 1);
  read_1d0 (0xae, 6); // = 0xe8088080 // !!!!
  write_1d0 (0, 0xae, 6, 1);
  read_1d0 (0x300, 4); // = 0x48088080 // !!!!
  write_1d0 (0, 0x300, 6, 1);
  read_mchbar16 (0x356); // !!!!
  write_mchbar16 (0x356, 0x1040);
  read_mchbar16 (0x756); // !!!!
  write_mchbar16 (0x756, 0x1040);
  write_mchbar32 (0x140, read_mchbar32 (0x140) & ~0x07000000);
  write_mchbar32 (0x138, read_mchbar32 (0x138) & ~0x07000000);
  write_mchbar32 (0x130, 0x31111301);
  while (read_mchbar32 (0x130) & 1);

  {
    u32 t;
    u8 val_a1;
    val_a1 = read_1d0 (0xa1, 6); // = 0x1cf4040 // !!!!
    t = read_1d0 (0x2f3, 6); // = 0x10a4040 // !!!!
    rmw_1d0 (0x320, 0x07, (t & 4) | ((t & 8) >> 2) | ((t & 0x10) >> 4), 6, 1);
    rmw_1d0 (0x14b, 0x78, ((((val_a1 >> 2) & 4) | (val_a1 & 8)) >> 2) | (val_a1 & 4), 7, 1);
    rmw_1d0 (0xce, 0x38, ((((val_a1 >> 2) & 4) | (val_a1 & 8)) >> 2) | (val_a1 & 4), 6, 1);
  }

  for (channel = 0; channel < NUM_CHANNELS; channel++)
    set_4cf (&info, channel, info.populated_ranks[channel][0][0] ? 9 : 1);

  rmw_1d0 (0x116, 0xe, 1, 4, 1); // = 0x4040432 // !!!!
  read_mchbar32 (0x144); // !!!!
  write_1d0 (2, 0xae, 6, 1);
  write_1d0 (2, 0x300, 6, 1);
  write_1d0 (2, 0x121, 3, 1);
  read_1d0 (0xd6, 6); // = 0xfa00c0c7 // !!!!
  write_1d0 (4, 0xd6, 6, 1);
  read_1d0 (0x328, 6); // = 0xfa00c0c7 // !!!!
  write_1d0 (4, 0x328, 6, 1);

  for (channel = 0; channel < NUM_CHANNELS; channel++)
    set_4cf (&info, channel, info.populated_ranks[channel][0][0] ? 9 : 0);

  write_mchbar32 (0x130, 0x11111301 | (info.populated_ranks[1][0][0] << 30) | (info.populated_ranks[0][0][0] << 29));
  while (read_mchbar8 (0x130) & 1); // !!!!
  read_1d0 (0xa1, 6); // = 0x1cf4054 // !!!!
  read_1d0 (0x2f3, 6); // = 0x10a4054 // !!!!
  read_1d0 (0x21c, 6); // = 0xafa00c0 // !!!!
  write_1d0 (0, 0x21c, 6, 1);
  read_1d0 (0x14b, 7); // = 0x810231b0 // !!!!
  write_1d0 (0x35, 0x14b, 7, 1);

  for (channel = 0; channel < NUM_CHANNELS; channel++)
    set_4cf (&info, channel, info.populated_ranks[channel][0][0] ? 0xb : 0x2);

  set_334 (1);

  write_mchbar8 (0x1e8, 0x4); /* OK */

  for (channel = 0; channel < NUM_CHANNELS; channel++)
    {
      write_500 (&info, channel, 0x3 & ~(info.populated_ranks_mask[channel]), 0x6b7, 2, 1);
      write_500 (&info, channel, 0x3, 0x69b, 2, 1);
    }
  write_mchbar32 (0x2d0, (read_mchbar32 (0x2d0) & 0xff2c01ff) | 0x200000); /* OK */
  write_mchbar16 (0x6c0, 0x14a0); /* OK */
  write_mchbar32 (0x6d0, (read_mchbar32 (0x6d0) & 0xff0080ff) | 0x8000); /* OK */
  write_mchbar16 (0x232, 0x8);
  write_mchbar32 (0x234, (read_mchbar32 (0x234) & 0xfffbfffb) | 0x40004); /* 0x40004 or 0 depending on ? */
  write_mchbar32 (0x34, (read_mchbar32 (0x34) & 0xfffffffd) | 5); /* OK */
  write_mchbar32 (0x128, 0x2150d05);
  write_mchbar8 (0x12c, 0x1f); /* OK */
  write_mchbar8 (0x12d, 0x56); /* OK */
  write_mchbar8 (0x12e, 0x31);
  write_mchbar8 (0x12f, 0x0); /* OK */
  write_mchbar8 (0x271, 0x2); /* OK */
  write_mchbar8 (0x671, 0x2); /* OK */
  write_mchbar8 (0x1e8, 0x4); /* OK */
  for (channel = 0; channel < NUM_CHANNELS; channel++)
    write_mchbar32 (0x294 + (channel << 10), (info.populated_ranks_mask[channel] & 3) << 16);
  write_mchbar32 (0x134, (read_mchbar32 (0x134) & 0xfc01ffff) | 0x10000); /* OK */
  write_mchbar32 (0x134, (read_mchbar32 (0x134) & 0xfc85ffff) | 0x850000); /* OK */
  for (channel = 0; channel < NUM_CHANNELS; channel++)
    write_mchbar32 (0x260 + (channel << 10), (read_mchbar32 (0x260 + (channel << 10)) & ~0xf00000) | 0x8000000 |((info.populated_ranks_mask[channel] & 3) << 20));

  if (!s3resume)
    jedec_init (&info);

  int totalrank = 0;
  for (channel = 0; channel < NUM_CHANNELS; channel++)
    for (slot = 0; slot < NUM_SLOTS; slot++)
      for (rank = 0; rank < NUM_RANKS; rank++)
	if (info.populated_ranks[channel][slot][rank])
	  {
	    jedec_read (&info, channel, slot, rank, totalrank, 0xa, 0x400);
	    totalrank++;
	  }

  write_mchbar8 (0x12c, 0x9f);

  read_mchbar8 (0x271); // 2 // !!!!
  write_mchbar8 (0x271, 0xe);
  read_mchbar8 (0x671); // !!!!
  write_mchbar8 (0x671, 0xe);

  if (s3resume)
    {
      pci_mm_write16 (NORTHBRIDGE, 0xc8, 3);
      write_26c (0, 0x820);
      write_26c (1, 0x820);
      write_mchbar32 (0x130, read_mchbar32 (0x130) | 2);
    }

  for (channel = 0; channel < NUM_CHANNELS; channel++)
    {
      write_mchbar32 (0x294 + (channel << 10), (info.populated_ranks_mask[channel] & 3) << 16);
      write_mchbar16 (0x298 + (channel << 10), (info.populated_ranks[channel][0][0]) | (info.populated_ranks[channel][0][1] << 5));
      write_mchbar32 (0x29c + (channel << 10), 0x77a);
    }
  read_mchbar32 (0x2c0); /// !!!
  write_mchbar32 (0x2c0, 0x6009cc00);

  if (!s3resume)
    {
      {
	u8 a, b;
	a = read_mchbar8 (0x243); // !!!!
	b = read_mchbar8 (0x643); // !!!!
	write_mchbar8 (0x243, a | 2);
	write_mchbar8 (0x643, b | 2);
      }

      write_1d0 (7, 0x19b, 3, 1);
      write_1d0 (7, 0x1c0, 3, 1);
      write_1d0 (4, 0x1c6, 4, 1);
      write_1d0 (4, 0x1cc, 4, 1);
      read_1d0 (0x151, 4); // = 0x408c6d74 // !!!!
      write_1d0 (4, 0x151, 4 ,1);
      write_mchbar32 (0x584, 0xfffff);
      write_mchbar32 (0x984, 0xfffff);

      for (channel = 0; channel < NUM_CHANNELS; channel++)
	for (slot = 0; slot < NUM_SLOTS; slot++)
	  for (rank = 0; rank < NUM_RANKS; rank++)
	    if (info.populated_ranks[channel][slot][rank])
	      config_rank (&info, s3resume, channel, slot, rank);

      seq8 ();

      pci_mm_write16 (NORTHBRIDGE, 0xc8, 3);
      write_26c (0, 0x820);
      write_26c (1, 0x820);
      write_mchbar32 (0x130, read_mchbar32 (0x130) | 2);
    }

  write_mchbar32 (0xfa4, read_mchbar32 (0xfa4) & ~0x01000002);
  write_mchbar32 (0xfb0, 0x2000e019);

#if !REAL
  printf ("CP16\n");
#endif

  /* FIXME: here there should be training.  */

  dump_timings (&info);

  program_modules_memory_map (&info, 0);
  program_total_memory_map (&info);

  printk (BIOS_ERR, __FILE__ ":%d\n", __LINE__);

#if 1 //def TROUBLE
  /* Trouble start */
  if (info.non_interleaved_part_mb != 0 && info.interleaved_part_mb != 0)
    write_mchbar8 (0x111, 0x20 | (0 << 2) | (1 << 6) | (0 << 7));
  else if (have_match_ranks (&info, 0, 4) && have_match_ranks (&info, 1, 4))
    write_mchbar8 (0x111, 0x20 | (3 << 2) | (0 << 6) | (1 << 7));
  else if (have_match_ranks (&info, 0, 2) && have_match_ranks (&info, 1, 2))
    write_mchbar8 (0x111, 0x20 | (3 << 2) | (0 << 6) | (0 << 7));
  else
    write_mchbar8 (0x111, 0x20 | (3 << 2) | (1 << 6) | (0 << 7));

  write_mchbar32 (0xfac, read_mchbar32 (0xfac) & ~0x80000000); // OK
  write_mchbar32 (0xfb4, 0x4800); // OK
  write_mchbar32 (0xfb8, (info.revision < 8) ? 0x20 : 0x0); // OK
  write_mchbar32 (0xe94, 0x7ffff); // OK
  write_mchbar32 (0xfc0, 0x80002040); // OK
  write_mchbar32 (0xfc4, 0x701246); // OK
  write_mchbar8 (0xfc8, read_mchbar8 (0xfc8) & ~0x70); // OK
  write_mchbar32 (0xe5c, 0x1000000 | read_mchbar32 (0xe5c)); // OK
  write_mchbar32 (0x1a70, (read_mchbar32 (0x1a70) | 0x00200000) & ~0x00100000); // OK
  write_mchbar32 (0x50, 0x700b0); // OK
  write_mchbar32 (0x3c, 0x10); // OK
  write_mchbar8 (0x1aa8, (read_mchbar8 (0x1aa8) & ~0x35) | 0xa); // OK
  write_mchbar8 (0xff4, read_mchbar8 (0xff4) | 0x2); // OK
  write_mchbar32 (0xff8, (read_mchbar32 (0xff8) & ~0xe008) | 0x1020); // OK

#if REAL
  write_mchbar32 (0xd00, IOMMU_BASE2 | 1);
  write_mchbar32 (0xd40, IOMMU_BASE1 | 1);
  write_mchbar32 (0xdc0, IOMMU_BASE4 | 1);

  write32 (IOMMU_BASE1 | 0xffc, 0x80000000);
  write32 (IOMMU_BASE2 | 0xffc, 0xc0000000);
  write32 (IOMMU_BASE4 | 0xffc, 0x80000000);

#else
  {
    u32 eax;      
    eax = read32 (0xffc + (read_mchbar32 (0xd00) & ~1)) | 0x08000000; // = 0xe911714b// OK
    write32 (0xffc + (read_mchbar32 (0xd00) & ~1), eax);// OK
    eax = read32 (0xffc + (read_mchbar32 (0xdc0) & ~1)) | 0x40000000; // = 0xe911714b// OK
    write32 (0xffc + (read_mchbar32 (0xdc0) & ~1), eax);// OK
  }
#endif

  {
    u32 eax;      

    eax = (info.some_base_frequency >> 4) / 9;
    write_mchbar32 (0xfcc, (read_mchbar32 (0xfcc) & 0xfffc0000) | (eax * 0x280) | (eax * 0x5000) | eax | 0x40000);// OK
    write_mchbar32 (0x20, 0x33001); //OK
  }

  printk (BIOS_ERR, __FILE__ ":%d\n", __LINE__);

  for (channel = 0; channel < NUM_CHANNELS; channel++)
    {
      write_mchbar32 (0x220 + (channel << 10),
		      read_mchbar32 (0x220 + (channel << 10)) & ~0x7770); //OK
      if (info.max_slots_used_in_channel == 1)
	write_mchbar16 (0x237 + (channel << 10), (read_mchbar16 (0x237 + (channel << 10)) | 0x0201)); //OK
      else
	write_mchbar16 (0x237 + (channel << 10), (read_mchbar16 (0x237 + (channel << 10)) & ~0x0201)); //OK

      write_mchbar8 (0x241 + (channel << 10), read_mchbar8 (0x241 + (channel << 10)) | 1); // OK

      if (info.clock_speed_index <= 1 && (info.silicon_revision == 2 || info.silicon_revision == 3))
	write_mchbar32 (0x248 + (channel << 10),
			(read_mchbar32 (0x248 + (channel << 10)) | 0x00102000)); // OK
      else
	write_mchbar32 (0x248 + (channel << 10),
			(read_mchbar32 (0x248 + (channel << 10)) & ~0x00102000)); // OK
    }

  write_mchbar32 (0x115, read_mchbar32 (0x115) | 0x1000000); // OK

  {
    u8 al;
    al = 0xd;
    if (!(info.silicon_revision == 0 || info.silicon_revision == 1))
      al += 2;
    al |= ((1 << (info.max_slots_used_in_channel - 1)) - 1) << 4;
    write_mchbar32 (0x210, (al << 16) | 0x20); // OK
  }

  for (channel = 0; channel < NUM_CHANNELS; channel++)
    {
      write_mchbar32 (0x288 + (channel << 10), 0x70605040); // OK
      write_mchbar32 (0x28c + (channel << 10), 0xfffec080); // OK
      write_mchbar32 (0x290 + (channel << 10), 0x282091c | ((info.max_slots_used_in_channel - 1) << 0x16)); // OK
    }
  u32 reg1c;
  pci_mm_read32 (NORTHBRIDGE, 0x40); // = DEFAULT_EPBAR | 0x001 // OK
  reg1c = read32 (DEFAULT_EPBAR | 0x01c); // = 0x8001 // OK
  pci_mm_read32 (NORTHBRIDGE, 0x40); // = DEFAULT_EPBAR | 0x001 // OK
  write32 (DEFAULT_EPBAR | 0x01c, reg1c); // OK
  read_mchbar8 (0xe08); // = 0x0 
  pci_mm_read32 (NORTHBRIDGE, 0xe4); // = 0x316126
  write_mchbar8 (0x1210, read_mchbar8 (0x1210) | 2); // OK
  write_mchbar32 (0x1200, 0x8800440); // OK
  write_mchbar32 (0x1204, 0x53ff0453); // OK
  write_mchbar32 (0x1208, 0x19002043); // OK
  write_mchbar16 (0x1214, 0x320); // OK

  /* trouble start. */
  if (info.revision == 0x10 || info.revision == 0x11)
    {
      write_mchbar16 (0x1214, 0x220); // OK
      write_mchbar8 (0x1210, read_mchbar8 (0x1210) | 0x40); // OK
    }

  write_mchbar8 (0x1214, read_mchbar8 (0x1214) | 0x4); // OK
  write_mchbar8 (0x120c, 0x1); // OK
  write_mchbar8 (0x1218, 0x3); // OK
  write_mchbar8 (0x121a, 0x3); // OK
  write_mchbar8 (0x121c, 0x3); // OK
  write_mchbar16 (0xc14, 0x0); // OK
  write_mchbar16 (0xc20, 0x0); // OK
  write_mchbar32 (0x1c, 0x0); // OK

  /* revision dependent here.  */

  write_mchbar16 (0x1230, read_mchbar16 (0x1230) | 0x1f07); // OK

  if (info.uma_enabled)
    write_mchbar32 (0x11f4, read_mchbar32 (0x11f4) | 0x10000000); // OK

  write_mchbar16 (0x1230, read_mchbar16 (0x1230) | 0x8000); // OK
  write_mchbar8 (0x1214, read_mchbar8 (0x1214) | 1); // OK

  u8 bl, ebpb;
  u16 reg_1020;

  reg_1020 = read_mchbar32 (0x1020); // = 0x6c733c  // OK
  write_mchbar8 (0x1070, 0x1); // OK

  write_mchbar32 (0x1000, 0x100); // OK
  write_mchbar8 (0x1007, 0x0); // OK

  if (reg_1020 != 0)
    {
      write_mchbar16 (0x1018, 0x0); // OK
      bl = reg_1020 >> 8;
      ebpb = reg_1020 & 0xff;
    }
  else
    {
      ebpb = 0;
      bl = 8;
    }

  /* trouble start */
  rdmsr (0x1a2);

  write_mchbar32 (0x1014, 0xffffffff); // OK

  write_mchbar32 (0x1010, ((((ebpb + 0x7d) << 7) / bl) & 0xff) * (!!reg_1020)); // OK

  write_mchbar8 (0x101c, 0xb8); // OK

  write_mchbar8 (0x123e, (read_mchbar8 (0x123e) & 0xf) | 0x60); // OK
  if (reg_1020 != 0)
    {
      write_mchbar32 (0x123c, (read_mchbar32 (0x123c) & ~0x00900000) | 0x600000); // OK
      write_mchbar8 (0x101c, 0xb8); // OK
    }
#endif
  printk (BIOS_ERR, __FILE__ ":%d\n", __LINE__);

  /* trouble start */

  setup_heci_uma (&info);

  printk (BIOS_ERR, __FILE__ ":%d\n", __LINE__);

  /* trouble end */
#if 1 //def TROUBLE
  if (info.uma_enabled)
    {
      u16 ax;
      write_mchbar32 (0x11b0, read_mchbar32 (0x11b0) | 0x4000); // OK
      write_mchbar32 (0x11b4, read_mchbar32 (0x11b4) | 0x4000); // OK
      write_mchbar16 (0x1190, read_mchbar16 (0x1190) | 0x4000); // OK

      ax = read_mchbar16 (0x1190) & 0xf00; // = 0x480a  // OK
      write_mchbar16 (0x1170, ax | (read_mchbar16 (0x1170) & 0x107f) | 0x4080); // OK
      write_mchbar16 (0x1170, read_mchbar16 (0x1170) | 0x1000); // OK
#if REAL
      udelay (1000);
#endif
      u16 ecx;
      for (ecx = 0xffff; ecx && (read_mchbar16 (0x1170) & 0x1000); ecx--);// OK
      write_mchbar16 (0x1190, read_mchbar16 (0x1190) & ~0x4000);// OK
    }
  printk (BIOS_ERR, __FILE__ ":%d\n", __LINE__);

  pci_mm_write8 (SOUTHBRIDGE, 0xa2, pci_mm_read8 (SOUTHBRIDGE, 0xa2) & ~0x80);
  write_mchbar16 (0x2ca8, 0x0);
  /* Trouble end */
#endif

  printk (BIOS_ERR, __FILE__ ":%d\n", __LINE__);

#if REAL
  udelay (1000);
  write_mchbar16 (0x2ca8, 0x8);
  dump_timings (&info);
  if (!s3resume)
    save_timings (&info);
#endif
}

#if REAL
unsigned long get_top_of_ram(void)
{
	/* Base of TSEG is top of usable DRAM */
	u32 tom = pci_read_config32(PCI_DEV(0,0,0), TSEG);
	return (unsigned long) tom;
}

struct cbmem_entry *get_cbmem_toc(void)
{
	return (struct cbmem_entry *)(get_top_of_ram() - HIGH_MEMORY_SIZE);
}
#endif

#if !REAL
int
main (void)
{
  raminit (0);
  return 0;
}
#endif
