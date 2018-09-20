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
 */

#include <arch/io.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <string.h>
#include <device/pci_ops.h>
#include <drivers/intel/gma/edid.h>
#include <drivers/intel/gma/i915.h>
#include <drivers/intel/gma/intel_bios.h>
#include <drivers/intel/gma/libgfxinit.h>
#include <pc80/vga.h>
#include <pc80/vga_io.h>
#include <southbridge/intel/ibexpeak/nvs.h>
#include <drivers/intel/gma/opregion.h>
#include <cbmem.h>

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
 * the mapping ourselves
 */

u32 map_oprom_vendev(u32 vendev)
{
	u32 new_vendev = vendev;

	/* none currently. */

	return new_vendev;
}

static struct resource *gtt_res = NULL;

u32 gtt_read(u32 reg)
{
	return read32(res2mmio(gtt_res, reg, 0));
}

void gtt_write(u32 reg, u32 data)
{
	write32(res2mmio(gtt_res, reg, 0), data);
}

static inline void gtt_write_powermeter(const struct gt_powermeter *pm)
{
	for (; pm && pm->reg; pm++)
		gtt_write(pm->reg, pm->value);
}

#define GTT_RETRY 1000
int gtt_poll(u32 reg, u32 mask, u32 value)
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

uintptr_t gma_get_gnvs_aslb(const void *gnvs)
{
	const global_nvs_t *gnvs_ptr = gnvs;
	return (uintptr_t)(gnvs_ptr ? gnvs_ptr->aslb : 0);
}

void gma_set_gnvs_aslb(void *gnvs, uintptr_t aslb)
{
	global_nvs_t *gnvs_ptr = gnvs;
	if (gnvs_ptr)
		gnvs_ptr->aslb = aslb;
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
	/* RPNFREQ_VAL comes from MCHBAR 0x5998 23:16 */
	/* only the lower 7 bits are used and shifted left by 25 */
	reg32 = MCHBAR32(0x5998);
	reg32 >>= 16;
	reg32 &= 0x7f;
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

/* Enable SCI to ACPI _GPE._L06 */
static void gma_enable_swsci(void)
{
	u16 reg16;

	/* clear DMISCI status */
	reg16 = inw(DEFAULT_PMBASE + TCO1_STS);
	reg16 &= DMISCI_STS;
	outw(DEFAULT_PMBASE + TCO1_STS, reg16);

	/* clear acpi tco status */
	outl(DEFAULT_PMBASE + GPE0_STS, TCOSCI_STS);

	/* enable acpi tco scis */
	reg16 = inw(DEFAULT_PMBASE + GPE0_EN);
	reg16 |= TCOSCI_EN;
	outw(DEFAULT_PMBASE + GPE0_EN, reg16);
}

static void gma_func0_init(struct device *dev)
{
	u32 reg32;

	/* IGD needs to be Bus Master */
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	reg32 |= PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO;
	pci_write_config32(dev, PCI_COMMAND, reg32);

	/* Init graphics power management */
	gma_pm_init_pre_vbios(dev);

	if (IS_ENABLED(CONFIG_MAINBOARD_USE_LIBGFXINIT)) {
		struct northbridge_intel_nehalem_config *conf = dev->chip_info;
		int lightup_ok;
		printk(BIOS_SPEW, "Initializing VGA without OPROM.");

		gma_gfxinit(&lightup_ok);
		/* Linux relies on VBT for panel info. */
		generate_fake_intel_oprom(&conf->gfx, dev,
					  "$VBT IRONLAKE-MOBILE");
	} else {
		/* PCI Init, will run VBIOS */
		pci_dev_init(dev);
	}

	/* Post VBIOS init */
	gma_pm_init_post_vbios(dev);

	gma_enable_swsci();
	intel_gma_restore_opregion();
}

static void gma_set_subsystem(struct device *dev, unsigned int vendor,
			      unsigned int device)
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
	pci_write_config32(dev, PCI_BASE_ADDRESS_2, 0xd0000001);
	pci_write_config32(dev, PCI_BASE_ADDRESS_2 + 4, 0);
	res->base = (resource_t) 0xd0000000;
	res->size = (resource_t) 0x10000000;
}

const struct i915_gpu_controller_info *
intel_gma_get_controller_info(void)
{
	struct device *dev = dev_find_slot(0, PCI_DEVFN(0x2,0));
	if (!dev) {
		return NULL;
	}
	struct northbridge_intel_nehalem_config *chip = dev->chip_info;
	return &chip->gfx;
}

static void gma_ssdt(struct device *device)
{
	const struct i915_gpu_controller_info *gfx = intel_gma_get_controller_info();
	if (!gfx) {
		return;
	}

	drivers_intel_gma_displays_ssdt_generate(gfx);
}

static unsigned long
gma_write_acpi_tables(struct device *const dev,
		      unsigned long current,
		      struct acpi_rsdp *const rsdp)
{
	igd_opregion_t *opregion = (igd_opregion_t *)current;
	global_nvs_t *gnvs;

	if (intel_gma_init_igd_opregion(opregion) != CB_SUCCESS)
		return current;

	current += sizeof(igd_opregion_t);

	/* GNVS has been already set up */
	gnvs = cbmem_find(CBMEM_ID_ACPI_GNVS);
	if (gnvs) {
		/* IGD OpRegion Base Address */
		gma_set_gnvs_aslb(gnvs, (uintptr_t)opregion);
	} else {
		printk(BIOS_ERR, "Error: GNVS table not found.\n");
	}

	current = acpi_align_current(current);
	return current;
}

static struct pci_operations gma_pci_ops = {
	.set_subsystem = gma_set_subsystem,
};

static struct device_operations gma_func0_ops = {
	.read_resources = gma_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.acpi_fill_ssdt_generator = gma_ssdt,
	.init = gma_func0_init,
	.scan_bus = 0,
	.enable = 0,
	.ops_pci = &gma_pci_ops,
	.write_acpi_tables	= gma_write_acpi_tables,
};

static const unsigned short pci_device_ids[] = {
	0x0046, 0x0102, 0x0106, 0x010a, 0x0112,
	0x0116, 0x0122, 0x0126, 0x0156,
	0x0166,
	0
};

static const struct pci_driver gma __pci_driver = {
	.ops = &gma_func0_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
