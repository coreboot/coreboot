/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Chromium OS Authors
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

#include <arch/io.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

#include "chip.h"
#include "sandybridge.h"

/* some vga option roms are used for several chipsets but they only have one
 * PCI ID in their header. If we encounter such an option rom, we need to do
 * the mapping ourselfes
 */

u32 map_oprom_vendev(u32 vendev)
{
	u32 new_vendev=vendev;

	switch (vendev) {
	case 0x80860102:		/* GT1 Desktop */
	case 0x8086010a:		/* GT1 Server */
	case 0x80860112:		/* GT2 Desktop */
	case 0x80860116:		/* GT2 Mobile */
	case 0x80860122:		/* GT2 Desktop >=1.3GHz */
	case 0x80860126:		/* GT2 Mobile >=1.3GHz */
	case 0x80860166:                /* IVB */
		new_vendev=0x80860106;	/* GT1 Mobile */
		break;
	}

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
		if (!gtt_poll(0x130090, (1 << 0), (1 << 0)))
			return;
	} else {
		gtt_write(0xa180, 1 << 5);
		gtt_write(0xa188, 0xffff0001);
		if (!gtt_poll(0x130090, (1 << 0), (1 << 0)))
			return;
	}

	if ((bridge_silicon_revision() & BASE_REV_MASK) == BASE_REV_SNB) {
		/* 1d: Set GTT+0x42004 [15:14]=11 (SnB C1+) */
		reg32 = gtt_read(0x42004);
		reg32 |= (1 << 14) | (1 << 15);
		gtt_write(0x42004, reg32);
	}

	if (bridge_silicon_revision() >= IVB_STEP_A0) {
		/* Display Reset Acknowledge Settings */
		gtt_write(0xa18c, 0x00000001);
		reg32 = gtt_read(0x45010);
		reg32 |= (1 << 1) | (1 << 0);
		gtt_write(0x45010, reg32);
	}

	/* 2: Get GT SKU from GTT+0x911c[13] */
	if ((bridge_silicon_revision() & BASE_REV_MASK) == BASE_REV_SNB) {
		reg32 = gtt_read(0x911c);
		if (reg32 & (1 << 13)) {
			printk(BIOS_DEBUG, "GT1 Power Meter Weights\n");
			gtt_write(0xa200, 0xcc000000);
			gtt_write(0xa204, 0x07000040);
			gtt_write(0xa208, 0x0000fe00);
			gtt_write(0xa20c, 0x00000000);
			gtt_write(0xa210, 0x17000000);
			gtt_write(0xa214, 0x00000021);
			gtt_write(0xa218, 0x0817fe19);
			gtt_write(0xa21c, 0x00000000);
			gtt_write(0xa220, 0x00000000);
			gtt_write(0xa224, 0xcc000000);
			gtt_write(0xa228, 0x07000040);
			gtt_write(0xa22c, 0x0000fe00);
			gtt_write(0xa230, 0x00000000);
			gtt_write(0xa234, 0x17000000);
			gtt_write(0xa238, 0x00000021);
			gtt_write(0xa23c, 0x0817fe19);
			gtt_write(0xa240, 0x00000000);
			gtt_write(0xa244, 0x00000000);
			gtt_write(0xa248, 0x8000421e);
		} else {
			printk(BIOS_DEBUG, "GT2 Power Meter Weights\n");
			gtt_write(0xa200, 0x330000a6);
			gtt_write(0xa204, 0x402d0031);
			gtt_write(0xa208, 0x00165f83);
			gtt_write(0xa20c, 0xf1000000);
			gtt_write(0xa210, 0x00000000);
			gtt_write(0xa214, 0x00160016);
			gtt_write(0xa218, 0x002a002b);
			gtt_write(0xa21c, 0x00000000);
			gtt_write(0xa220, 0x00000000);
			gtt_write(0xa224, 0x330000a6);
			gtt_write(0xa228, 0x402d0031);
			gtt_write(0xa22c, 0x00165f83);
			gtt_write(0xa230, 0xf1000000);
			gtt_write(0xa234, 0x00000000);
			gtt_write(0xa238, 0x00160016);
			gtt_write(0xa23c, 0x002a002b);
			gtt_write(0xa240, 0x00000000);
			gtt_write(0xa244, 0x00000000);
			gtt_write(0xa248, 0x8000421e);
		}
	} else {
		printk(BIOS_DEBUG, "IVB GT Power Meter Weights\n");
		gtt_write(0xa800, 0x00000000);
		gtt_write(0xa804, 0x00023800);
		gtt_write(0xa808, 0x00000902);
		gtt_write(0xa80c, 0x0c002f00);
		gtt_write(0xa810, 0x12000500);
		gtt_write(0xa814, 0x00000000);
		gtt_write(0xa818, 0x00b20000);
		gtt_write(0xa81c, 0x00000002);
		gtt_write(0xa820, 0x03004b02);
		gtt_write(0xa824, 0x00000600);
		gtt_write(0xa828, 0x07000773);
		gtt_write(0xa82c, 0x00000000);
		gtt_write(0xa830, 0x00010000);
		gtt_write(0xa834, 0x0510020d);
		gtt_write(0xa838, 0x00020100);
		gtt_write(0xa83c, 0x00103700);
		gtt_write(0xa840, 0x0000001d);
		gtt_write(0xa844, 0x00000000);
		gtt_write(0xa848, 0x20001b00);
		gtt_write(0xa84c, 0x0a000010);
		gtt_write(0xa850, 0x00000000);
		gtt_write(0xa854, 0x00000008);
		gtt_write(0xa858, 0x00000000);
		gtt_write(0xa85c, 0x00000000);
		gtt_write(0xa860, 0x00040000);
		gtt_write(0xa248, 0x0000221e);
		gtt_write(0xa900, 0x00000000);
		gtt_write(0xa904, 0x00003500);
		gtt_write(0xa908, 0x00000000);
		gtt_write(0xa90c, 0x0c000000);
		gtt_write(0xa910, 0x12000500);
		gtt_write(0xa914, 0x00000000);
		gtt_write(0xa918, 0x00b20000);
		gtt_write(0xa91c, 0x00000000);
		gtt_write(0xa920, 0x08004b02);
		gtt_write(0xa924, 0x00000400);
		gtt_write(0xa928, 0x07000820);
		gtt_write(0xa92c, 0x00000000);
		gtt_write(0xa930, 0x00030000);
		gtt_write(0xa934, 0x050f020d);
		gtt_write(0xa938, 0x00020300);
		gtt_write(0xa93c, 0x00903900);
		gtt_write(0xa940, 0x00000000);
		gtt_write(0xa944, 0x00000000);
		gtt_write(0xa948, 0x20001b00);
		gtt_write(0xa94c, 0x0a000010);
		gtt_write(0xa950, 0x00000000);
		gtt_write(0xa954, 0x00000008);
		gtt_write(0xa960, 0x00110000);
		gtt_write(0xaa3c, 0x00003900);
		gtt_write(0xaa54, 0x00000008);
		gtt_write(0xaa60, 0x00110000);
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
		if (!gtt_poll(0x941c, (1 << 1), (0 << 1)))
			return;
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
	if (!gtt_poll(0x138124, (1 << 31), (0 << 31)))
		return;
	gtt_write(0x138128, 0x00000029); /* Mailbox Data */
	gtt_write(0x138124, 0x80000004); /* Mailbox Cmd for RC6 VID */
	if (!gtt_poll(0x138124, (1 << 31), (0 << 31)))
		return;
	gtt_write(0x138124, 0x8000000a); /* Mailbox Cmd to clear RC6 count */
	if (!gtt_poll(0x138124, (1 << 31), (0 << 31)))
		return;

	/* 8 */
	gtt_write(0xa090, 0x00000000); /* RC Control */
	gtt_write(0xa098, 0x03e80000); /* RC1e Wake Rate Limit */
	gtt_write(0xa09c, 0x0028001e); /* RC6/6p Wake Rate Limit */
	gtt_write(0xa0a0, 0x0000001e); /* RC6pp Wake Rate Limit */
	gtt_write(0xa0a8, 0x0001e848); /* RC Evaluation Interval */
	gtt_write(0xa0ac, 0x00000019); /* RC Idle Hysteresis */

	/* 9 */
	gtt_write(0x2054, 0x0000000a); /* Render Idle Max Count */
	gtt_write(0x12054,0x0000000a); /* Video Idle Max Count */
	gtt_write(0x22054,0x0000000a); /* Blitter Idle Max Count */

	/* 10 */
	gtt_write(0xa0b0, 0x00000000); /* Unblock Ack to Busy */
	gtt_write(0xa0b4, 0x000003e8); /* RC1e Threshold */
	gtt_write(0xa0b8, 0x0000c350); /* RC6 Threshold */
	gtt_write(0xa0bc, 0x000186a0); /* RC6p Threshold */
	gtt_write(0xa0c0, 0x0000fa00); /* RC6pp Threshold */

	/* 11 */
	gtt_write(0xa010, 0x000f4240); /* RP Down Timeout */
	gtt_write(0xa014, 0x12060000); /* RP Interrupt Limits */
	gtt_write(0xa02c, 0x00015f90); /* RP Up Threshold */
	gtt_write(0xa030, 0x000186a0); /* RP Down Threshold */
	gtt_write(0xa068, 0x000186a0); /* RP Up EI */
	gtt_write(0xa06c, 0x000493e0); /* RP Down EI */
	gtt_write(0xa070, 0x0000000a); /* RP Idle Hysteresis */

	/* 11a: Enable Render Standby (RC6) */
	if ((bridge_silicon_revision() & BASE_REV_MASK) == BASE_REV_IVB) {
		/* on IVB: also enable DeepRenderStandby */
		gtt_write(0xa090, 0x88060000); /* HW RC Control */
	} else {
		gtt_write(0xa090, 0x88040000); /* HW RC Control */
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
	struct northbridge_intel_sandybridge_config *conf = dev->chip_info;
	u32 reg32;

	printk(BIOS_DEBUG, "GT Power Management Init (post VBIOS)\n");

	/* 15: Deassert Force Wake */
	gtt_write(0xa18c, gtt_read(0xa18c) & ~1);
	if (!gtt_poll(0x130090, (1 << 0), (0 << 0)))
		return;

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

	/* PCI Init, will run VBIOS */
	pci_dev_init(dev);

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
				((device & 0xffff) << 16) | (vendor & 0xffff));
	}
}

static struct pci_operations gma_pci_ops = {
	.set_subsystem    = gma_set_subsystem,
};

static struct device_operations gma_func0_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= gma_func0_init,
	.scan_bus		= 0,
	.enable			= 0,
	.ops_pci		= &gma_pci_ops,
};

static const struct pci_driver gma_gt1_desktop __pci_driver = {
	.ops	= &gma_func0_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x0102,
};

static const struct pci_driver gma_gt1_mobile __pci_driver = {
	.ops	= &gma_func0_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x0106,
};

static const struct pci_driver gma_gt1_server __pci_driver = {
	.ops	= &gma_func0_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x010a,
};

static const struct pci_driver gma_gt2_desktop __pci_driver = {
	.ops	= &gma_func0_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x0112,
};

static const struct pci_driver gma_gt2_mobile __pci_driver = {
	.ops	= &gma_func0_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x0116,
};

static const struct pci_driver gma_gt2_desktop_fast __pci_driver = {
	.ops	= &gma_func0_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x0122,
};

static const struct pci_driver gma_gt2_mobile_fast __pci_driver = {
	.ops	= &gma_func0_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x0126,
};

static const struct pci_driver gma_func0_driver_3 __pci_driver = {
	.ops    = &gma_func0_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = 0x0166,
};
