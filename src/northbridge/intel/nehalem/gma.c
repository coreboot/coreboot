/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Chromium OS Authors
 * Copyright (C) 2013 Vladimir Serbinenko
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <string.h>
#include <device/pci_ops.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <drivers/intel/gma/edid.h>
#include <drivers/intel/gma/i915.h>
#include <pc80/vga.h>
#include <pc80/vga_io.h>


#include "chip.h"
#include "nehalem.h"

struct gt_powermeter {
	u16 reg;
	u32 value;
};

static const struct gt_powermeter snb_pm_gt1[] = {
	{0xa200, 0xcc000000},
	{0xa204, 0x07000040},
	{0xa208, 0x0000fe00},
	{0xa20c, 0x00000000},
	{0xa210, 0x17000000},
	{0xa214, 0x00000021},
	{0xa218, 0x0817fe19},
	{0xa21c, 0x00000000},
	{0xa220, 0x00000000},
	{0xa224, 0xcc000000},
	{0xa228, 0x07000040},
	{0xa22c, 0x0000fe00},
	{0xa230, 0x00000000},
	{0xa234, 0x17000000},
	{0xa238, 0x00000021},
	{0xa23c, 0x0817fe19},
	{0xa240, 0x00000000},
	{0xa244, 0x00000000},
	{0xa248, 0x8000421e},
	{0}
};

static const struct gt_powermeter snb_pm_gt2[] = {
	{0xa200, 0x330000a6},
	{0xa204, 0x402d0031},
	{0xa208, 0x00165f83},
	{0xa20c, 0xf1000000},
	{0xa210, 0x00000000},
	{0xa214, 0x00160016},
	{0xa218, 0x002a002b},
	{0xa21c, 0x00000000},
	{0xa220, 0x00000000},
	{0xa224, 0x330000a6},
	{0xa228, 0x402d0031},
	{0xa22c, 0x00165f83},
	{0xa230, 0xf1000000},
	{0xa234, 0x00000000},
	{0xa238, 0x00160016},
	{0xa23c, 0x002a002b},
	{0xa240, 0x00000000},
	{0xa244, 0x00000000},
	{0xa248, 0x8000421e},
	{0}
};

static const struct gt_powermeter ivb_pm_gt1[] = {
	{0xa800, 0x00000000},
	{0xa804, 0x00021c00},
	{0xa808, 0x00000403},
	{0xa80c, 0x02001700},
	{0xa810, 0x05000200},
	{0xa814, 0x00000000},
	{0xa818, 0x00690500},
	{0xa81c, 0x0000007f},
	{0xa820, 0x01002501},
	{0xa824, 0x00000300},
	{0xa828, 0x01000331},
	{0xa82c, 0x0000000c},
	{0xa830, 0x00010016},
	{0xa834, 0x01100101},
	{0xa838, 0x00010103},
	{0xa83c, 0x00041300},
	{0xa840, 0x00000b30},
	{0xa844, 0x00000000},
	{0xa848, 0x7f000000},
	{0xa84c, 0x05000008},
	{0xa850, 0x00000001},
	{0xa854, 0x00000004},
	{0xa858, 0x00000007},
	{0xa85c, 0x00000000},
	{0xa860, 0x00010000},
	{0xa248, 0x0000221e},
	{0xa900, 0x00000000},
	{0xa904, 0x00001c00},
	{0xa908, 0x00000000},
	{0xa90c, 0x06000000},
	{0xa910, 0x09000200},
	{0xa914, 0x00000000},
	{0xa918, 0x00590000},
	{0xa91c, 0x00000000},
	{0xa920, 0x04002501},
	{0xa924, 0x00000100},
	{0xa928, 0x03000410},
	{0xa92c, 0x00000000},
	{0xa930, 0x00020000},
	{0xa934, 0x02070106},
	{0xa938, 0x00010100},
	{0xa93c, 0x00401c00},
	{0xa940, 0x00000000},
	{0xa944, 0x00000000},
	{0xa948, 0x10000e00},
	{0xa94c, 0x02000004},
	{0xa950, 0x00000001},
	{0xa954, 0x00000004},
	{0xa960, 0x00060000},
	{0xaa3c, 0x00001c00},
	{0xaa54, 0x00000004},
	{0xaa60, 0x00060000},
	{0}
};

static const struct gt_powermeter ivb_pm_gt2[] = {
	{0xa800, 0x10000000},
	{0xa804, 0x00033800},
	{0xa808, 0x00000902},
	{0xa80c, 0x0c002f00},
	{0xa810, 0x12000400},
	{0xa814, 0x00000000},
	{0xa818, 0x00d20800},
	{0xa81c, 0x00000002},
	{0xa820, 0x03004b02},
	{0xa824, 0x00000600},
	{0xa828, 0x07000773},
	{0xa82c, 0x00000000},
	{0xa830, 0x00010032},
	{0xa834, 0x1520040d},
	{0xa838, 0x00020105},
	{0xa83c, 0x00083700},
	{0xa840, 0x0000151d},
	{0xa844, 0x00000000},
	{0xa848, 0x20001b00},
	{0xa84c, 0x0a000010},
	{0xa850, 0x00000000},
	{0xa854, 0x00000008},
	{0xa858, 0x00000008},
	{0xa85c, 0x00000000},
	{0xa860, 0x00020000},
	{0xa248, 0x0000221e},
	{0xa900, 0x00000000},
	{0xa904, 0x00003500},
	{0xa908, 0x00000000},
	{0xa90c, 0x0c000000},
	{0xa910, 0x12000500},
	{0xa914, 0x00000000},
	{0xa918, 0x00b20000},
	{0xa91c, 0x00000000},
	{0xa920, 0x08004b02},
	{0xa924, 0x00000200},
	{0xa928, 0x07000820},
	{0xa92c, 0x00000000},
	{0xa930, 0x00030000},
	{0xa934, 0x050f020d},
	{0xa938, 0x00020300},
	{0xa93c, 0x00903900},
	{0xa940, 0x00000000},
	{0xa944, 0x00000000},
	{0xa948, 0x20001b00},
	{0xa94c, 0x0a000010},
	{0xa950, 0x00000000},
	{0xa954, 0x00000008},
	{0xa960, 0x00110000},
	{0xaa3c, 0x00003900},
	{0xaa54, 0x00000008},
	{0xaa60, 0x00110000},
	{0}
};

static const struct gt_powermeter ivb_pm_gt2_17w[] = {
	{0xa800, 0x20000000},
	{0xa804, 0x000e3800},
	{0xa808, 0x00000806},
	{0xa80c, 0x0c002f00},
	{0xa810, 0x0c000800},
	{0xa814, 0x00000000},
	{0xa818, 0x00d20d00},
	{0xa81c, 0x000000ff},
	{0xa820, 0x03004b02},
	{0xa824, 0x00000600},
	{0xa828, 0x07000773},
	{0xa82c, 0x00000000},
	{0xa830, 0x00020032},
	{0xa834, 0x1520040d},
	{0xa838, 0x00020105},
	{0xa83c, 0x00083700},
	{0xa840, 0x000016ff},
	{0xa844, 0x00000000},
	{0xa848, 0xff000000},
	{0xa84c, 0x0a000010},
	{0xa850, 0x00000002},
	{0xa854, 0x00000008},
	{0xa858, 0x0000000f},
	{0xa85c, 0x00000000},
	{0xa860, 0x00020000},
	{0xa248, 0x0000221e},
	{0xa900, 0x00000000},
	{0xa904, 0x00003800},
	{0xa908, 0x00000000},
	{0xa90c, 0x0c000000},
	{0xa910, 0x12000800},
	{0xa914, 0x00000000},
	{0xa918, 0x00b20000},
	{0xa91c, 0x00000000},
	{0xa920, 0x08004b02},
	{0xa924, 0x00000300},
	{0xa928, 0x01000820},
	{0xa92c, 0x00000000},
	{0xa930, 0x00030000},
	{0xa934, 0x15150406},
	{0xa938, 0x00020300},
	{0xa93c, 0x00903900},
	{0xa940, 0x00000000},
	{0xa944, 0x00000000},
	{0xa948, 0x20001b00},
	{0xa94c, 0x0a000010},
	{0xa950, 0x00000000},
	{0xa954, 0x00000008},
	{0xa960, 0x00110000},
	{0xaa3c, 0x00003900},
	{0xaa54, 0x00000008},
	{0xaa60, 0x00110000},
	{0}
};

static const struct gt_powermeter ivb_pm_gt2_35w[] = {
	{0xa800, 0x00000000},
	{0xa804, 0x00030400},
	{0xa808, 0x00000806},
	{0xa80c, 0x0c002f00},
	{0xa810, 0x0c000300},
	{0xa814, 0x00000000},
	{0xa818, 0x00d20d00},
	{0xa81c, 0x000000ff},
	{0xa820, 0x03004b02},
	{0xa824, 0x00000600},
	{0xa828, 0x07000773},
	{0xa82c, 0x00000000},
	{0xa830, 0x00020032},
	{0xa834, 0x1520040d},
	{0xa838, 0x00020105},
	{0xa83c, 0x00083700},
	{0xa840, 0x000016ff},
	{0xa844, 0x00000000},
	{0xa848, 0xff000000},
	{0xa84c, 0x0a000010},
	{0xa850, 0x00000001},
	{0xa854, 0x00000008},
	{0xa858, 0x00000008},
	{0xa85c, 0x00000000},
	{0xa860, 0x00020000},
	{0xa248, 0x0000221e},
	{0xa900, 0x00000000},
	{0xa904, 0x00003800},
	{0xa908, 0x00000000},
	{0xa90c, 0x0c000000},
	{0xa910, 0x12000800},
	{0xa914, 0x00000000},
	{0xa918, 0x00b20000},
	{0xa91c, 0x00000000},
	{0xa920, 0x08004b02},
	{0xa924, 0x00000300},
	{0xa928, 0x01000820},
	{0xa92c, 0x00000000},
	{0xa930, 0x00030000},
	{0xa934, 0x15150406},
	{0xa938, 0x00020300},
	{0xa93c, 0x00903900},
	{0xa940, 0x00000000},
	{0xa944, 0x00000000},
	{0xa948, 0x20001b00},
	{0xa94c, 0x0a000010},
	{0xa950, 0x00000000},
	{0xa954, 0x00000008},
	{0xa960, 0x00110000},
	{0xaa3c, 0x00003900},
	{0xaa54, 0x00000008},
	{0xaa60, 0x00110000},
	{0}
};

/* some vga option roms are used for several chipsets but they only have one
 * PCI ID in their header. If we encounter such an option rom, we need to do
 * the mapping ourselfes
 */

u32 map_oprom_vendev(u32 vendev)
{
	u32 new_vendev = vendev;

	/* none curently.  */

	return new_vendev;
}

static struct resource *gtt_res = NULL;

static inline u32 gtt_read(u32 reg)
{
	return read32(gtt_res->base + reg);
}

static inline void gtt_write(u32 reg, u32 data)
{
	write32(gtt_res->base + reg, data);
}

static inline void gtt_write_powermeter(const struct gt_powermeter *pm)
{
	for (; pm && pm->reg; pm++)
		gtt_write(pm->reg, pm->value);
}

#define GTT_RETRY 1000
static int gtt_poll(u32 reg, u32 mask, u32 value)
{
	unsigned try = GTT_RETRY;
	u32 data;

	while (try--) {
		data = gtt_read(reg);
		if ((data & mask) == value)
			return 1;
		udelay(10);
	}

	printk(BIOS_ERR, "GT init timeout\n");
	return 0;
}

static void gma_pm_init_pre_vbios(struct device *dev)
{
	u32 reg32;

	printk(BIOS_DEBUG, "GT Power Management Init\n");

	gtt_res = find_resource(dev, PCI_BASE_ADDRESS_0);
	if (!gtt_res || !gtt_res->base)
		return;

	if (bridge_silicon_revision() < IVB_STEP_C0) {
		/* 1: Enable force wake */
		gtt_write(0xa18c, 0x00000001);
		gtt_poll(0x130090, (1 << 0), (1 << 0));
	} else {
		gtt_write(0xa180, 1 << 5);
		gtt_write(0xa188, 0xffff0001);
		gtt_poll(0x130040, (1 << 0), (1 << 0));
	}

	if ((bridge_silicon_revision() & BASE_REV_MASK) == BASE_REV_SNB) {
		/* 1d: Set GTT+0x42004 [15:14]=11 (SnB C1+) */
		reg32 = gtt_read(0x42004);
		reg32 |= (1 << 14) | (1 << 15);
		gtt_write(0x42004, reg32);
	}

	if (bridge_silicon_revision() >= IVB_STEP_A0) {
		/* Display Reset Acknowledge Settings */
		reg32 = gtt_read(0x45010);
		reg32 |= (1 << 1) | (1 << 0);
		gtt_write(0x45010, reg32);
	}

	/* 2: Get GT SKU from GTT+0x911c[13] */
	reg32 = gtt_read(0x911c);
	if ((bridge_silicon_revision() & BASE_REV_MASK) == BASE_REV_SNB) {
		if (reg32 & (1 << 13)) {
			printk(BIOS_DEBUG, "SNB GT1 Power Meter Weights\n");
			gtt_write_powermeter(snb_pm_gt1);
		} else {
			printk(BIOS_DEBUG, "SNB GT2 Power Meter Weights\n");
			gtt_write_powermeter(snb_pm_gt2);
		}
	} else {
		u32 unit = MCHBAR32(0x5938) & 0xf;

		if (reg32 & (1 << 13)) {
			/* GT1 SKU */
			printk(BIOS_DEBUG, "IVB GT1 Power Meter Weights\n");
			gtt_write_powermeter(ivb_pm_gt1);
		} else {
			/* GT2 SKU */
			u32 tdp = MCHBAR32(0x5930) & 0x7fff;
			tdp /= (1 << unit);

			if (tdp <= 17) {
				/* <=17W ULV */
				printk(BIOS_DEBUG, "IVB GT2 17W "
				       "Power Meter Weights\n");
				gtt_write_powermeter(ivb_pm_gt2_17w);
			} else if ((tdp >= 25) && (tdp <= 35)) {
				/* 25W-35W */
				printk(BIOS_DEBUG, "IVB GT2 25W-35W "
				       "Power Meter Weights\n");
				gtt_write_powermeter(ivb_pm_gt2_35w);
			} else {
				/* All others */
				printk(BIOS_DEBUG, "IVB GT2 35W "
				       "Power Meter Weights\n");
				gtt_write_powermeter(ivb_pm_gt2_35w);
			}
		}
	}

	/* 3: Gear ratio map */
	gtt_write(0xa004, 0x00000010);

	/* 4: GFXPAUSE */
	gtt_write(0xa000, 0x00070020);

	/* 5: Dynamic EU trip control */
	gtt_write(0xa080, 0x00000004);

	/* 6: ECO bits */
	reg32 = gtt_read(0xa180);
	reg32 |= (1 << 26) | (1 << 31);
	/* (bit 20=1 for SNB step D1+ / IVB A0+) */
	if (bridge_silicon_revision() >= SNB_STEP_D1)
		reg32 |= (1 << 20);
	gtt_write(0xa180, reg32);

	/* 6a: for SnB step D2+ only */
	if (((bridge_silicon_revision() & BASE_REV_MASK) == BASE_REV_SNB) &&
	    (bridge_silicon_revision() >= SNB_STEP_D2)) {
		reg32 = gtt_read(0x9400);
		reg32 |= (1 << 7);
		gtt_write(0x9400, reg32);

		reg32 = gtt_read(0x941c);
		reg32 &= 0xf;
		reg32 |= (1 << 1);
		gtt_write(0x941c, reg32);
		gtt_poll(0x941c, (1 << 1), (0 << 1));
	}

	if ((bridge_silicon_revision() & BASE_REV_MASK) == BASE_REV_IVB) {
		reg32 = gtt_read(0x907c);
		reg32 |= (1 << 16);
		gtt_write(0x907c, reg32);

		/* 6b: Clocking reset controls */
		gtt_write(0x9424, 0x00000001);
	} else {
		/* 6b: Clocking reset controls */
		gtt_write(0x9424, 0x00000000);
	}

	/* 7 */
	if (gtt_poll(0x138124, (1 << 31), (0 << 31))) {
		gtt_write(0x138128, 0x00000029);	/* Mailbox Data */
		gtt_write(0x138124, 0x80000004);	/* Mailbox Cmd for RC6 VID */
		if (gtt_poll(0x138124, (1 << 31), (0 << 31)))
			gtt_write(0x138124, 0x8000000a);
		gtt_poll(0x138124, (1 << 31), (0 << 31));
	}

	/* 8 */
	gtt_write(0xa090, 0x00000000);	/* RC Control */
	gtt_write(0xa098, 0x03e80000);	/* RC1e Wake Rate Limit */
	gtt_write(0xa09c, 0x0028001e);	/* RC6/6p Wake Rate Limit */
	gtt_write(0xa0a0, 0x0000001e);	/* RC6pp Wake Rate Limit */
	gtt_write(0xa0a8, 0x0001e848);	/* RC Evaluation Interval */
	gtt_write(0xa0ac, 0x00000019);	/* RC Idle Hysteresis */

	/* 9 */
	gtt_write(0x2054, 0x0000000a);	/* Render Idle Max Count */
	gtt_write(0x12054, 0x0000000a);	/* Video Idle Max Count */
	gtt_write(0x22054, 0x0000000a);	/* Blitter Idle Max Count */

	/* 10 */
	gtt_write(0xa0b0, 0x00000000);	/* Unblock Ack to Busy */
	gtt_write(0xa0b4, 0x000003e8);	/* RC1e Threshold */
	gtt_write(0xa0b8, 0x0000c350);	/* RC6 Threshold */
	gtt_write(0xa0bc, 0x000186a0);	/* RC6p Threshold */
	gtt_write(0xa0c0, 0x0000fa00);	/* RC6pp Threshold */

	/* 11 */
	gtt_write(0xa010, 0x000f4240);	/* RP Down Timeout */
	gtt_write(0xa014, 0x12060000);	/* RP Interrupt Limits */
	gtt_write(0xa02c, 0x00015f90);	/* RP Up Threshold */
	gtt_write(0xa030, 0x000186a0);	/* RP Down Threshold */
	gtt_write(0xa068, 0x000186a0);	/* RP Up EI */
	gtt_write(0xa06c, 0x000493e0);	/* RP Down EI */
	gtt_write(0xa070, 0x0000000a);	/* RP Idle Hysteresis */

	/* 11a: Enable Render Standby (RC6) */
	if ((bridge_silicon_revision() & BASE_REV_MASK) == BASE_REV_IVB) {
		/*
		 * IvyBridge should also support DeepRenderStandby.
		 *
		 * Unfortunately it does not work reliably on all SKUs so
		 * disable it here and it can be enabled by the kernel.
		 */
		gtt_write(0xa090, 0x88040000);	/* HW RC Control */
	} else {
		gtt_write(0xa090, 0x88040000);	/* HW RC Control */
	}

	/* 12: Normal Frequency Request */
	/* RPNFREQ_VAL comes from MCHBAR 0x5998 23:16 (8 bits!? use 7) */
	reg32 = MCHBAR32(0x5998);
	reg32 >>= 16;
	reg32 &= 0xef;
	reg32 <<= 25;
	gtt_write(0xa008, reg32);

	/* 13: RP Control */
	gtt_write(0xa024, 0x00000592);

	/* 14: Enable PM Interrupts */
	gtt_write(0x4402c, 0x03000076);

	/* Clear 0x6c024 [8:6] */
	reg32 = gtt_read(0x6c024);
	reg32 &= ~0x000001c0;
	gtt_write(0x6c024, reg32);
}

static void gma_pm_init_post_vbios(struct device *dev)
{
	struct northbridge_intel_nehalem_config *conf = dev->chip_info;
	u32 reg32;

	printk(BIOS_DEBUG, "GT Power Management Init (post VBIOS)\n");

	/* 15: Deassert Force Wake */
	if (bridge_silicon_revision() < IVB_STEP_C0) {
		gtt_write(0xa18c, gtt_read(0xa18c) & ~1);
		gtt_poll(0x130090, (1 << 0), (0 << 0));
	} else {
		gtt_write(0xa188, 0x1fffe);
		if (gtt_poll(0x130040, (1 << 0), (0 << 0)))
			gtt_write(0xa188, gtt_read(0xa188) | 1);
	}

	/* 16: SW RC Control */
	gtt_write(0xa094, 0x00060000);

	/* Setup Digital Port Hotplug */
	reg32 = gtt_read(0xc4030);
	if (!reg32) {
		reg32 = (conf->gpu_dp_b_hotplug & 0x7) << 2;
		reg32 |= (conf->gpu_dp_c_hotplug & 0x7) << 10;
		reg32 |= (conf->gpu_dp_d_hotplug & 0x7) << 18;
		gtt_write(0xc4030, reg32);
	}

	/* Setup Panel Power On Delays */
	reg32 = gtt_read(0xc7208);
	if (!reg32) {
		reg32 = (conf->gpu_panel_port_select & 0x3) << 30;
		reg32 |= (conf->gpu_panel_power_up_delay & 0x1fff) << 16;
		reg32 |= (conf->gpu_panel_power_backlight_on_delay & 0x1fff);
		gtt_write(0xc7208, reg32);
	}

	/* Setup Panel Power Off Delays */
	reg32 = gtt_read(0xc720c);
	if (!reg32) {
		reg32 = (conf->gpu_panel_power_down_delay & 0x1fff) << 16;
		reg32 |= (conf->gpu_panel_power_backlight_off_delay & 0x1fff);
		gtt_write(0xc720c, reg32);
	}

	/* Setup Panel Power Cycle Delay */
	if (conf->gpu_panel_power_cycle_delay) {
		reg32 = gtt_read(0xc7210);
		reg32 &= ~0xff;
		reg32 |= conf->gpu_panel_power_cycle_delay & 0xff;
		gtt_write(0xc7210, reg32);
	}

	/* Enable Backlight if needed */
	if (conf->gpu_cpu_backlight) {
		gtt_write(0x48250, (1 << 31));
		gtt_write(0x48254, conf->gpu_cpu_backlight);
	}
	if (conf->gpu_pch_backlight) {
		gtt_write(0xc8250, (1 << 31));
		gtt_write(0xc8254, conf->gpu_pch_backlight);
	}
}

#if IS_ENABLED(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT)

static void train_link(u32 mmio)
{
	/* Clear interrupts. */
	write32(mmio + DEIIR, 0xffffffff);

	write32(mmio + 0x000f0018, 0x000000ff);
	write32(mmio + 0x000f1018, 0x000000ff);
	write32(mmio + 0x000f000c, 0x001a2050);
	write32(mmio + 0x00060100, 0x001c4000);
	write32(mmio + 0x00060100, 0x801c4000);
	write32(mmio + 0x000f000c, 0x801a2050);
	write32(mmio + 0x00060100, 0x801c4000);
	write32(mmio + 0x000f000c, 0x801a2050);
	mdelay(1);

	read32(mmio + 0x000f0014); // = 0x00000100
	write32(mmio + 0x000f0014, 0x00000100);
	write32(mmio + 0x00060100, 0x901c4000);
	write32(mmio + 0x000f000c, 0x901a2050);
	mdelay(1);
	read32(mmio + 0x000f0014); // = 0x00000600
}

static void power_port(u32 mmio)
{
	read32(mmio + 0x000e1100); // = 0x00000000
	write32(mmio + 0x000e1100, 0x00000000);
	write32(mmio + 0x000e1100, 0x00010000);
	read32(mmio + 0x000e1100); // = 0x00010000
	read32(mmio + 0x000e1100); // = 0x00010000
	read32(mmio + 0x000e1100); // = 0x00000000
	write32(mmio + 0x000e1100, 0x00000000);
	read32(mmio + 0x000e1100); // = 0x00000000
	read32(mmio + 0x000e4200); // = 0x0000001c
	write32(mmio + 0x000e4210, 0x8004003e);
	write32(mmio + 0x000e4214, 0x80060002);
	write32(mmio + 0x000e4218, 0x01000000);
	read32(mmio + 0x000e4210); // = 0x5144003e
	write32(mmio + 0x000e4210, 0x5344003e);
	read32(mmio + 0x000e4210); // = 0x0144003e
	write32(mmio + 0x000e4210, 0x8074003e);
	read32(mmio + 0x000e4210); // = 0x5144003e
	read32(mmio + 0x000e4210); // = 0x5144003e
	write32(mmio + 0x000e4210, 0x5344003e);
	read32(mmio + 0x000e4210); // = 0x0144003e
	write32(mmio + 0x000e4210, 0x8074003e);
	read32(mmio + 0x000e4210); // = 0x5144003e
	read32(mmio + 0x000e4210); // = 0x5144003e
	write32(mmio + 0x000e4210, 0x5344003e);
	read32(mmio + 0x000e4210); // = 0x0144003e
	write32(mmio + 0x000e4210, 0x8074003e);
	read32(mmio + 0x000e4210); // = 0x5144003e
	read32(mmio + 0x000e4210); // = 0x5144003e
	write32(mmio + 0x000e4210, 0x5344003e);
	write32(mmio + 0x000e4f00, 0x0100030c);
	write32(mmio + 0x000e4f04, 0x00b8230c);
	write32(mmio + 0x000e4f08, 0x06f8930c);
	write32(mmio + 0x000e4f0c, 0x09f8e38e);
	write32(mmio + 0x000e4f10, 0x00b8030c);
	write32(mmio + 0x000e4f14, 0x0b78830c);
	write32(mmio + 0x000e4f18, 0x0ff8d3cf);
	write32(mmio + 0x000e4f1c, 0x01e8030c);
	write32(mmio + 0x000e4f20, 0x0ff863cf);
	write32(mmio + 0x000e4f24, 0x0ff803cf);
	write32(mmio + 0x000c4030, 0x00001000);
	read32(mmio + 0x000c4000); // = 0x00000000
	write32(mmio + 0x000c4030, 0x00001000);
	read32(mmio + 0x000e1150); // = 0x0000001c
	write32(mmio + 0x000e1150, 0x0000089c);
	write32(mmio + 0x000fcc00, 0x01986f00);
	write32(mmio + 0x000fcc0c, 0x01986f00);
	write32(mmio + 0x000fcc18, 0x01986f00);
	write32(mmio + 0x000fcc24, 0x01986f00);
	read32(mmio + 0x000c4000); // = 0x00000000
	read32(mmio + 0x000e1180); // = 0x40000002
}

static void intel_gma_init(const struct northbridge_intel_nehalem_config *info,
			   u32 mmio, u32 physbase, u16 piobase, u32 lfb)
{
	int i;
	u8 edid_data[128];
	struct edid edid;
	u32 hactive, vactive, right_border, bottom_border;
	int hpolarity, vpolarity;
	u32 vsync, hsync, vblank, hblank, hfront_porch, vfront_porch;
	u32 candp1, candn;
	u32 best_delta = 0xffffffff;
	u32 target_frequency;
	u32 pixel_p1 = 1;
	u32 pixel_n = 1;
	u32 pixel_m1 = 1;
	u32 pixel_m2 = 1;
	u32 link_frequency = info->gpu_link_frequency_270_mhz ? 270000 : 162000;
	u32 data_m1;
	u32 data_n1 = 0x00800000;
	u32 link_m1;
	u32 link_n1 = 0x00080000;

	write32(mmio + 0x00070080, 0x00000000);
	write32(mmio + DSPCNTR(0), 0x00000000);
	write32(mmio + 0x00071180, 0x00000000);
	write32(mmio + CPU_VGACNTRL, 0x0000298e | VGA_DISP_DISABLE);
	write32(mmio + 0x0007019c, 0x00000000);
	write32(mmio + 0x0007119c, 0x00000000);
	write32(mmio + 0x000fc008, 0x2c010000);
	write32(mmio + 0x000fc020, 0x2c010000);
	write32(mmio + 0x000fc038, 0x2c010000);
	write32(mmio + 0x000fc050, 0x2c010000);
	write32(mmio + 0x000fc408, 0x2c010000);
	write32(mmio + 0x000fc420, 0x2c010000);
	write32(mmio + 0x000fc438, 0x2c010000);
	write32(mmio + 0x000fc450, 0x2c010000);
	vga_gr_write(0x18, 0);
	write32(mmio + 0x00042004, 0x02000000);
	write32(mmio + 0x000fd034, 0x8421ffe0);

	/* Setup GTT.  */
	for (i = 0; i < 0x2000; i++)
	{
		outl((i << 2) | 1, piobase);
		outl(physbase + (i << 12) + 1, piobase + 4);
	}

	vga_misc_write(0x67);

	const u8 cr[] = { 0x5f, 0x4f, 0x50, 0x82, 0x55, 0x81, 0xbf, 0x1f,
		    0x00, 0x4f, 0x0d, 0x0e, 0x00, 0x00, 0x00, 0x00,
		    0x9c, 0x8e, 0x8f, 0x28, 0x1f, 0x96, 0xb9, 0xa3,
		    0xff
	};
	vga_cr_write(0x11, 0);

	for (i = 0; i <= 0x18; i++)
		vga_cr_write(i, cr[i]);

	power_port(mmio);

	intel_gmbus_read_edid(mmio + PCH_GMBUS0, 3, 0x50, edid_data, 128);
	decode_edid(edid_data,
		    sizeof(edid_data), &edid);

	/* Disable screen memory to prevent garbage from appearing.  */
	vga_sr_write(1, vga_sr_read(1) | 0x20);

	hactive = edid.x_resolution;
	vactive = edid.y_resolution;
	right_border = edid.hborder;
	bottom_border = edid.vborder;
	hpolarity = (edid.phsync == '-');
	vpolarity = (edid.pvsync == '-');
	vsync = edid.vspw;
	hsync = edid.hspw;
	vblank = edid.vbl;
	hblank = edid.hbl;
	hfront_porch = edid.hso;
	vfront_porch = edid.vso;

	target_frequency = info->gpu_lvds_dual_channel ? edid.pixel_clock
		: (2 * edid.pixel_clock);
#if !IS_ENABLED(CONFIG_FRAMEBUFFER_KEEP_VESA_MODE)
	vga_textmode_init();
#else
	vga_sr_write(1, 1);
	vga_sr_write(0x2, 0xf);
	vga_sr_write(0x3, 0x0);
	vga_sr_write(0x4, 0xe);
	vga_gr_write(0, 0x0);
	vga_gr_write(1, 0x0);
	vga_gr_write(2, 0x0);
	vga_gr_write(3, 0x0);
	vga_gr_write(4, 0x0);
	vga_gr_write(5, 0x0);
	vga_gr_write(6, 0x5);
	vga_gr_write(7, 0xf);
	vga_gr_write(0x10, 0x1);
	vga_gr_write(0x11, 0);


	edid.bytes_per_line = (edid.bytes_per_line + 63) & ~63;

	write32(mmio + DSPCNTR(0), DISPPLANE_BGRX888);
	write32(mmio + DSPADDR(0), 0);
	write32(mmio + DSPSTRIDE(0), edid.bytes_per_line);
	write32(mmio + DSPSURF(0), 0);
	for (i = 0; i < 0x100; i++)
		write32(mmio + LGC_PALETTE(0) + 4 * i, i * 0x010101);
#endif

	/* Find suitable divisors.  */
	for (candp1 = 1; candp1 <= 8; candp1++) {
		for (candn = 5; candn <= 10; candn++) {
			u32 cur_frequency;
			u32 m; /* 77 - 131.  */
			u32 denom; /* 35 - 560.  */
			u32 current_delta;

			denom = candn * candp1 * 7;
			/* Doesnt overflow for up to
			   5000000 kHz = 5 GHz.  */
			m = (target_frequency * denom + 60000) / 120000;

			if (m < 77 || m > 131)
				continue;

			cur_frequency = (120000 * m) / denom;
			if (target_frequency > cur_frequency)
				current_delta = target_frequency - cur_frequency;
			else
				current_delta = cur_frequency - target_frequency;


			if (best_delta > current_delta) {
				best_delta = current_delta;
				pixel_n = candn;
				pixel_p1 = candp1;
				pixel_m2 = ((m + 3) % 5) + 7;
				pixel_m1 = (m - pixel_m2) / 5;
			}
		}
	}

	if (best_delta == 0xffffffff) {
		printk (BIOS_ERR, "Couldn't find GFX clock divisors\n");
		return;
	}

	link_m1 = ((uint64_t)link_n1 * edid.pixel_clock) / link_frequency;
	data_m1 = ((uint64_t)data_n1 * 18 * edid.pixel_clock)
		/ (link_frequency * 8 * (info->gpu_lvds_num_lanes ? : 4));

	printk(BIOS_INFO, "bringing up panel at resolution %d x %d\n",
	       hactive, vactive);
	printk(BIOS_DEBUG, "Borders %d x %d\n",
	       right_border, bottom_border);
	printk(BIOS_DEBUG, "Blank %d x %d\n",
	       hblank, vblank);
	printk(BIOS_DEBUG, "Sync %d x %d\n",
	       hsync, vsync);
	printk(BIOS_DEBUG, "Front porch %d x %d\n",
	       hfront_porch, vfront_porch);
	printk(BIOS_DEBUG, (info->gpu_use_spread_spectrum_clock
			    ? "Spread spectrum clock\n" : "DREF clock\n"));
	printk(BIOS_DEBUG,
	       info->gpu_lvds_dual_channel ? "Dual channel\n" : "Single channel\n");
	printk(BIOS_DEBUG, "Polarities %d, %d\n",
	       hpolarity, vpolarity);
	printk(BIOS_DEBUG, "Data M1=%d, N1=%d\n",
	       data_m1, data_n1);
	printk(BIOS_DEBUG, "Link frequency %d kHz\n",
	       link_frequency);
	printk(BIOS_DEBUG, "Link M1=%d, N1=%d\n",
	       link_m1, link_n1);
	printk(BIOS_DEBUG, "Pixel N=%d, M1=%d, M2=%d, P1=%d\n",
	       pixel_n, pixel_m1, pixel_m2, pixel_p1);
	printk(BIOS_DEBUG, "Pixel clock %d kHz\n",
	       120000 * (5 * pixel_m1 + pixel_m2) / pixel_n
	       / (pixel_p1 * 7));

	write32(mmio + PCH_LVDS,
		(hpolarity << 20) | (vpolarity << 21)
		| (info->gpu_lvds_dual_channel ? LVDS_CLOCK_B_POWERUP_ALL
		   | LVDS_CLOCK_BOTH_POWERUP_ALL : 0)
		| LVDS_BORDER_ENABLE | LVDS_CLOCK_A_POWERUP_ALL
		| LVDS_DETECTED);
	write32(mmio + BLC_PWM_CPU_CTL2, (1 << 31));
	write32(mmio + PCH_DREF_CONTROL, (info->gpu_use_spread_spectrum_clock
					  ? 0x1002 : 0x400));
	mdelay(1);
	write32(mmio + PCH_PP_CONTROL, PANEL_UNLOCK_REGS
		| (read32(mmio + PCH_PP_CONTROL) & ~PANEL_UNLOCK_MASK));
	write32(mmio + _PCH_FP0(0),
		((pixel_n - 2) << 16)
		| ((pixel_m1 - 2) << 8) | pixel_m2);
	write32(mmio + _PCH_DPLL(0),
		DPLL_VCO_ENABLE | DPLLB_MODE_LVDS
		| (info->gpu_lvds_dual_channel ? DPLLB_LVDS_P2_CLOCK_DIV_7
		   : DPLLB_LVDS_P2_CLOCK_DIV_14)
		| (0x10000 << (pixel_p1 - 1))
		| ((info->gpu_use_spread_spectrum_clock ? 3 : 0) << 13)
		| (0x1 << (pixel_p1 - 1)));
	mdelay(1);
	write32(mmio + _PCH_DPLL(0),
		DPLL_VCO_ENABLE | DPLLB_MODE_LVDS
		| (info->gpu_lvds_dual_channel ? DPLLB_LVDS_P2_CLOCK_DIV_7
		   : DPLLB_LVDS_P2_CLOCK_DIV_14)
		| (0x10000 << (pixel_p1 - 1))
		| ((info->gpu_use_spread_spectrum_clock ? 3 : 0) << 13)
		| (0x1 << (pixel_p1 - 1)));
	/* Re-lock the registers.  */
	write32(mmio + PCH_PP_CONTROL,
		(read32(mmio + PCH_PP_CONTROL) & ~PANEL_UNLOCK_MASK));

	write32(mmio + PCH_LVDS,
		(hpolarity << 20) | (vpolarity << 21)
		| (info->gpu_lvds_dual_channel ? LVDS_CLOCK_B_POWERUP_ALL
		   | LVDS_CLOCK_BOTH_POWERUP_ALL : 0)
		| LVDS_BORDER_ENABLE | LVDS_CLOCK_A_POWERUP_ALL
		| LVDS_DETECTED);

	write32(mmio + HTOTAL(0),
		((hactive + right_border + hblank - 1) << 16)
		| (hactive - 1));
	write32(mmio + HBLANK(0),
		((hactive + right_border + hblank - 1) << 16)
		| (hactive + right_border - 1));
	write32(mmio + HSYNC(0),
		((hactive + right_border + hfront_porch + hsync - 1) << 16)
		| (hactive + right_border + hfront_porch - 1));

	write32(mmio + VTOTAL(0), ((vactive + bottom_border + vblank - 1) << 16)
		| (vactive - 1));
	write32(mmio + VBLANK(0), ((vactive + bottom_border + vblank - 1) << 16)
		| (vactive + bottom_border - 1));
	write32(mmio + VSYNC(0),
		(vactive + bottom_border + vfront_porch + vsync - 1)
		| (vactive + bottom_border + vfront_porch - 1));

	write32(mmio + PIPECONF(0), PIPECONF_DISABLE);

	write32(mmio + PF_WIN_POS(0), 0);
#if IS_ENABLED(CONFIG_FRAMEBUFFER_KEEP_VESA_MODE)
	write32(mmio + PIPESRC(0), ((hactive - 1) << 16) | (vactive - 1));
	write32(mmio + PF_CTL(0),0);
	write32(mmio + PF_WIN_SZ(0), 0);
#else
	write32(mmio + PIPESRC(0), (639 << 16) | 399);
	write32(mmio + PF_CTL(0),PF_ENABLE | PF_FILTER_MED_3x3);
	write32(mmio + PF_WIN_SZ(0), vactive | (hactive << 16));
#endif

	mdelay(1);

	write32(mmio + PIPE_DATA_M1(0), 0x7e000000 | data_m1);
	write32(mmio + PIPE_DATA_N1(0), data_n1);
	write32(mmio + PIPE_LINK_M1(0), link_m1);
	write32(mmio + PIPE_LINK_N1(0), link_n1);

	write32(mmio + 0x000f000c, 0x00002040);
	mdelay(1);
	write32(mmio + 0x000f000c, 0x00002050);
	write32(mmio + 0x00060100, 0x00044000);
	mdelay(1);
	write32(mmio + PIPECONF(0), PIPECONF_BPP_6);
	write32(mmio + 0x000f0008, 0x00000040);
	write32(mmio + 0x000f000c, 0x00022050);
	write32(mmio + PIPECONF(0), PIPECONF_BPP_6 | PIPECONF_DITHER_EN);
	write32(mmio + PIPECONF(0), PIPECONF_ENABLE | PIPECONF_BPP_6 | PIPECONF_DITHER_EN);

#if IS_ENABLED(CONFIG_FRAMEBUFFER_KEEP_VESA_MODE)
	write32(mmio + CPU_VGACNTRL, 0x20298e | VGA_DISP_DISABLE);
#else
	write32(mmio + CPU_VGACNTRL, 0x20298e);
#endif
	train_link(mmio);

#if IS_ENABLED(CONFIG_FRAMEBUFFER_KEEP_VESA_MODE)
	write32(mmio + DSPCNTR(0), DISPLAY_PLANE_ENABLE | DISPPLANE_BGRX888);
	mdelay(1);
#endif

	write32(mmio + TRANS_HTOTAL(0),
		((hactive + right_border + hblank - 1) << 16)
		| (hactive - 1));
	write32(mmio + TRANS_HBLANK(0),
		((hactive + right_border + hblank - 1) << 16)
		| (hactive + right_border - 1));
	write32(mmio + TRANS_HSYNC(0),
		((hactive + right_border + hfront_porch + hsync - 1) << 16)
		| (hactive + right_border + hfront_porch - 1));

	write32(mmio + TRANS_VTOTAL(0),
		((vactive + bottom_border + vblank - 1) << 16)
		| (vactive - 1));
	write32(mmio + TRANS_VBLANK(0),
		((vactive + bottom_border + vblank - 1) << 16)
		| (vactive + bottom_border - 1));
	write32(mmio + TRANS_VSYNC(0),
		(vactive + bottom_border + vfront_porch + vsync - 1)
		| (vactive + bottom_border + vfront_porch - 1));

	write32(mmio + 0x00060100, 0xb01c4000);
	write32(mmio + 0x000f000c, 0xb01a2050);
	mdelay(1);
	write32(mmio + TRANSCONF(0), TRANS_ENABLE | TRANS_6BPC
#if IS_ENABLED(CONFIG_FRAMEBUFFER_KEEP_VESA_MODE)
		| TRANS_STATE_MASK
#endif
		);
	write32(mmio + PCH_LVDS,
		LVDS_PORT_ENABLE
		| (hpolarity << 20) | (vpolarity << 21)
		| (info->gpu_lvds_dual_channel ? LVDS_CLOCK_B_POWERUP_ALL
		   | LVDS_CLOCK_BOTH_POWERUP_ALL : 0)
		| LVDS_BORDER_ENABLE | LVDS_CLOCK_A_POWERUP_ALL
		| LVDS_DETECTED);

	write32(mmio + PCH_PP_CONTROL, PANEL_UNLOCK_REGS | PANEL_POWER_OFF);
	write32(mmio + PCH_PP_CONTROL, PANEL_UNLOCK_REGS | PANEL_POWER_RESET);
	mdelay(1);
	write32(mmio + PCH_PP_CONTROL, PANEL_UNLOCK_REGS
		| PANEL_POWER_ON | PANEL_POWER_RESET);

	printk (BIOS_DEBUG, "waiting for panel powerup\n");
	while (1) {
		u32 reg32;
		reg32 = read32(mmio + PCH_PP_STATUS);
		if (((reg32 >> 28) & 3) == 0)
			break;
	}
	printk (BIOS_DEBUG, "panel powered up\n");

	write32(mmio + PCH_PP_CONTROL, PANEL_POWER_ON | PANEL_POWER_RESET);

	/* Enable screen memory.  */
	vga_sr_write(1, vga_sr_read(1) & ~0x20);

	/* Clear interrupts. */
	write32(mmio + DEIIR, 0xffffffff);
	write32(mmio + SDEIIR, 0xffffffff);

#if IS_ENABLED(CONFIG_FRAMEBUFFER_KEEP_VESA_MODE)
	memset ((void *) lfb, 0, edid.x_resolution * edid.y_resolution * 4);
	set_vbe_mode_info_valid(&edid, lfb);
#endif
}

#endif


static void gma_func0_init(struct device *dev)
{
	u32 reg32;

	/* IGD needs to be Bus Master */
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	reg32 |= PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO;
	pci_write_config32(dev, PCI_COMMAND, reg32);

	/* Init graphics power management */
	gma_pm_init_pre_vbios(dev);

#if !CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT
	/* PCI Init, will run VBIOS */
	pci_dev_init(dev);
#else
	u32 physbase;
	struct northbridge_intel_nehalem_config *conf = dev->chip_info;
	struct resource *lfb_res;
	struct resource *pio_res;

	lfb_res = find_resource(dev, PCI_BASE_ADDRESS_2);
	pio_res = find_resource(dev, PCI_BASE_ADDRESS_4);

	physbase = pci_read_config32(dev, 0x5c) & ~0xf;

	if (gtt_res && gtt_res->base && physbase && pio_res && pio_res->base
	    && lfb_res && lfb_res->base) {
		printk(BIOS_SPEW, "Initializing VGA without OPROM. MMIO 0x%llx\n",
		       gtt_res->base);
		intel_gma_init(conf, gtt_res->base, physbase, pio_res->base,
			       lfb_res->base);
	}
#endif

	/* Linux relies on VBT for panel info.  */
	if (read16(0xc0000) != 0xaa55) {
		optionrom_header_t *oh = (void *)0xc0000;
		optionrom_pcir_t *pcir;
		int sz;

		memset(oh->reserved, 0, 8192);

		sz = (0x80 + sizeof(fake_vbt) + 511) / 512;
		oh->signature = 0xaa55;
		oh->size = sz;
		oh->pcir_offset = 0x40;
		oh->vbt_offset = 0x80;

		pcir = (void *)0xc0040;
		pcir->signature = 0x52494350;	// PCIR
		pcir->vendor = dev->vendor;
		pcir->device = dev->device;
		pcir->length = sizeof(*pcir);
		pcir->revision = dev->class;
		pcir->classcode[0] = dev->class >> 8;
		pcir->classcode[1] = dev->class >> 16;
		pcir->classcode[2] = dev->class >> 24;
		pcir->imagelength = sz;
		pcir->indicator = 0x80;

		memcpy((void *)0xc0080, fake_vbt, sizeof(fake_vbt));
	}


	/* Post VBIOS init */
	gma_pm_init_post_vbios(dev);
}

static void gma_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	if (!vendor || !device) {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				   pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				   ((device & 0xffff) << 16) | (vendor &
								0xffff));
	}
}

static void gma_read_resources(struct device *dev)
{
	pci_dev_read_resources(dev);

	struct resource *res;

	/* Set the graphics memory to write combining. */
	res = find_resource(dev, PCI_BASE_ADDRESS_2);
	if (res == NULL) {
		printk(BIOS_DEBUG, "gma: memory resource not found.\n");
		return;
	}
	res->flags |= IORESOURCE_RESERVE | IORESOURCE_FIXED | IORESOURCE_ASSIGNED;
	pci_write_config32(dev, PCI_BASE_ADDRESS_2,
			   0xd0000001);
	pci_write_config32(dev, PCI_BASE_ADDRESS_2 + 4,
			   0);
	res->base = (resource_t) 0xd0000000;
	res->size = (resource_t) 0x10000000;
}

static struct pci_operations gma_pci_ops = {
	.set_subsystem = gma_set_subsystem,
};

static struct device_operations gma_func0_ops = {
	.read_resources = gma_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = gma_func0_init,
	.scan_bus = 0,
	.enable = 0,
	.ops_pci = &gma_pci_ops,
};

static const unsigned short pci_device_ids[] =
    { 0x0046, 0x0102, 0x0106, 0x010a, 0x0112,
	0x0116, 0x0122, 0x0126, 0x0156,
	0x0166,
	0
};

static const struct pci_driver gma __pci_driver = {
	.ops = &gma_func0_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
