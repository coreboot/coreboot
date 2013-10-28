/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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

#include <baytrail/gfx.h>
#include <baytrail/iosf.h>
#include <baytrail/pci_devs.h>
#include <baytrail/ramstage.h>

struct gt_reg {
	u32 reg;
	u32 andmask;
	u32 ormask;
};

static const struct gt_reg gfx_powermeter_weights[] = {
	/* SET1 */
	{ 0xA800, 0xFFFFFFFF, 0x00000000 },
	{ 0xA804, 0xFFFFFFFF, 0x00000000 },
	{ 0xA808, 0xFFFFFFFF, 0x0000ff0A },
	{ 0xA80C, 0xFFFFFFFF, 0x1D000000 },
	{ 0xA810, 0xFFFFFFFF, 0xAC004900 },
	{ 0xA814, 0xFFFFFFFF, 0x000F0000 },
	{ 0xA818, 0xFFFFFFFF, 0x5A000000 },
	{ 0xA81C, 0xFFFFFFFF, 0x2600001F },
	{ 0xA820, 0xFFFFFFFF, 0x00090000 },
	{ 0xA824, 0xFFFFFFFF, 0x2000ff00 },
	{ 0xA828, 0xFFFFFFFF, 0xff090016 },
	{ 0xA82C, 0xFFFFFFFF, 0x00000000 },
	{ 0xA830, 0xFFFFFFFF, 0x00000100 },
	{ 0xA834, 0xFFFFFFFF, 0x00A00F51 },
	{ 0xA838, 0xFFFFFFFF, 0x000B0000 },
	{ 0xA83C, 0xFFFFFFFF, 0xcb7D3307 },
	{ 0xA840, 0xFFFFFFFF, 0x003C0000 },
	{ 0xA844, 0xFFFFFFFF, 0xFFFF0000 },
	{ 0xA848, 0xFFFFFFFF, 0x00220000 },
	{ 0xA84c, 0xFFFFFFFF, 0x43000000 },
	{ 0xA850, 0xFFFFFFFF, 0x00000800 },
	{ 0xA854, 0xFFFFFFFF, 0x00000F00 },
	{ 0xA858, 0xFFFFFFFF, 0x00000021 },
	{ 0xA85c, 0xFFFFFFFF, 0x00000000 },
	{ 0xA860, 0xFFFFFFFF, 0x00190000 },
	{ 0xAA80, 0xFFFFFFFF, 0x00FF00FF },
	{ 0xAA84, 0xFFFFFFFF, 0x00000000 },
	{ 0x1300A4, 0xFFFFFFFF, 0x00000000 },

	/* SET2 */
	{ 0xA900, 0xFFFFFFFF, 0x00000000 },
	{ 0xA904, 0xFFFFFFFF, 0x00000000 },
	{ 0xA908, 0xFFFFFFFF, 0x00000000 },
	{ 0xa90c, 0xFFFFFFFF, 0x1D000000 },
	{ 0xa910, 0xFFFFFFFF, 0xAC005000 },
	{ 0xa914, 0xFFFFFFFF, 0x000F0000 },
	{ 0xa918, 0xFFFFFFFF, 0x5A000000 },
	{ 0xa91c, 0xFFFFFFFF, 0x2600001F },
	{ 0xa920, 0xFFFFFFFF, 0x00090000 },
	{ 0xa924, 0xFFFFFFFF, 0x2000ff00 },
	{ 0xa928, 0xFFFFFFFF, 0xff090016 },
	{ 0xa92c, 0xFFFFFFFF, 0x00000000 },
	{ 0xa930, 0xFFFFFFFF, 0x00000100 },
	{ 0xa934, 0xFFFFFFFF, 0x00A00F51 },
	{ 0xa938, 0xFFFFFFFF, 0x000B0000 },
	{ 0xA93C, 0xFFFFFFFF, 0xcb7D3307 },
	{ 0xA940, 0xFFFFFFFF, 0x003C0000 },
	{ 0xA944, 0xFFFFFFFF, 0xFFFF0000 },
	{ 0xA948, 0xFFFFFFFF, 0x00220000 },
	{ 0xA94C, 0xFFFFFFFF, 0x43000000 },
	{ 0xA950, 0xFFFFFFFF, 0x00000800 },
	{ 0xA954, 0xFFFFFFFF, 0x00000000 },
	{ 0xA960, 0xFFFFFFFF, 0x00000000 },

	/* SET3 */
	{ 0xaa3c, 0xFFFFFFFF, 0x00000000 },
	{ 0xaa54, 0xFFFFFFFF, 0x00000000 },
	{ 0xaa60, 0xFFFFFFFF, 0x00000000 },

	/* Enable PowerMeter Counters */
	{ 0xA248, 0xFFFFFFFF, 0x00000058 },

	{ 0 }
};

static const struct gt_reg gfx_rc6_registers[] = {
	{ 0xA090, 0xFFFFFFFF, 0x00000000 },

	/* RC1e - RC6/6p - RC6pp Wake Rate Limits */
	{ 0xA09C, 0xFFFFFFFF, 0x00280000 },
	{ 0xA0A8, 0xFFFFFFFF, 0x0001E848 },
	{ 0xA0AC, 0xFFFFFFFF, 0x00000019 },

	/* RC Sleep / RCx Thresholds */
	{ 0xA0B0, 0xFFFFFFFF, 0x00000000 },
	{ 0xA0B8, 0xFFFFFFFF, 0x00000557 },

	{ 0 }
};

static const struct gt_reg gfx_turbo_registers[] = {
	/* Render/Video/Blitter Idle Max Count */
	{ 0x2054,  0xFFFFFFFF, 0x0000000A },
	{ 0x12054, 0xFFFFFFFF, 0x0000000A },
	{ 0x22054, 0xFFFFFFFF, 0x0000000A },

	/* RP Down Timeout */
	{ 0xA010,  0xFFFFFFFF, 0x000F4240 },

	{ 0 }
};

static const struct gt_reg gfx_turbo_ctl_registers[] = {
	/* RP Up/Down Threshold */
	{ 0xA02C, 0xFFFFFFFF, 0x0000E8E8 },
	{ 0xA030, 0xFFFFFFFF, 0x0003BD08 },

	/* RP Up/Down EI */
	{ 0xA068, 0xFFFFFFFF, 0x000101D0 },
	{ 0xA06C, 0xFFFFFFFF, 0x00055730 },

	{ 0 }
};

static struct resource *gtt_res = NULL;

static inline unsigned long gtt_read(unsigned long reg)
{
	u32 val;
	val = read32(gtt_res->base + reg);
	return val;

}

static inline void gtt_write(unsigned long reg, unsigned long data)
{
	write32(gtt_res->base + reg, data);
}

static inline void gtt_rmw(u32 reg, u32 andmask, u32 ormask)
{
	u32 val = gtt_read(reg);
	val &= andmask;
	val |= ormask;
	gtt_write(reg, val);
}

static void gtt_write_regs(const struct gt_reg *gt)
{
	for (; gt && gt->reg; gt++)
		gtt_rmw(gt->reg, gt->andmask, gt->ormask);
}

#define GFX_POLL_RETRY 10000

static int gtt_poll(u32 reg, u32 mask, u32 value)
{
	unsigned try = GFX_POLL_RETRY;
	u32 data;

	while (try--) {
		data = gtt_read(reg);
		if ((data & mask) == value)
			return 1;
		udelay(100);
	}

	printk(BIOS_ERR, "GTT timeout\n");
	return 0;
}

static int gfx_poll_power_gate(u32 mask, u32 value)
{
	unsigned try = GFX_POLL_RETRY;
	u32 data;

	while (try--) {
		data = iosf_punit_read(PUNIT_PWRGT_STATUS);
		if ((data & mask) == value)
			return 1;
		udelay(100);
	}

	printk(BIOS_ERR, "Power Gate timeout\n");
	return 0;
}

/*
 * Lock Power Context Base Register to point to a 24KB block
 * of memory in GSM.  Power context save data is stored here.
 */
static void gfx_lock_pcbase(device_t dev)
{
	const u16 gms_size_map[17] = { 0,32,64,96,128,160,192,224,256,
				       288,320,352,384,416,448,480,512 };
	u32 pcsize = 24 << 10;  /* 24KB */
	u32 wopcmsz = 0x100000; /* PAVP offset */
	u32 gms, gmsize, pcbase;

	gms = pci_read_config32(dev, GGC) & GGC_GSM_SIZE_MASK;
	gms >>= 3;
	if (gms > sizeof(gms_size_map))
		return;
	gmsize = gms_size_map[gms];

	/* PcBase = BDSM + GMS Size - WOPCMSZ - PowerContextSize */
	pcbase = pci_read_config32(dev, GSM_BASE) & 0xfff00000;
	pcbase += (gmsize-1) * wopcmsz - pcsize;
	pcbase |= 1; /* Lock */

	gtt_write(0x182120, pcbase);
}

static void gfx_pm_init(device_t dev)
{
	printk(BIOS_INFO, "GFX: Power Management Init\n");

	/*
	 * Allow-Wake render/media wells.
	 * Used by PUNIT as part of S0IX save/restore.
	 */
	gtt_rmw(0x130090, ~1, 1);
	if (!gtt_poll(0x130094, 1, 1)) {
		printk(BIOS_ERR, "gfx_pm_init: Unable to allow-wake\n");
		return;
	}

	/* Render Force-Wake */
	gtt_write(0x1300B0, 0x80008000);
	if (!gtt_poll(0x1300B4, 0x8000, 0x8000)) {
		printk(BIOS_ERR, "gfx_pm_init: Unable to force-wake render\n");
		return;
	}

	/* Media Force-Wake */
	gtt_write(0x1300B8, 0x80008000);
	if (!gtt_poll(0x1300BC, 0x8000, 0x8000)) {
		printk(BIOS_ERR, "gfx_pm_init: Unable to force-wake media\n");
		return;
	};

	/* Workaround - X0:261954/A0:261955 */
	gtt_rmw(0x182060, ~0xf, 1);

	/* Program GT PowerMeter weights */
	gtt_write_regs(gfx_powermeter_weights);

	/* Program PUNIT_GPU_EC_VIRUS based on DPTF SDP */
	/* SDP Profile 4 == 0x11940, others 0xcf08 */
	iosf_punit_write(PUNIT_GPU_EC_VIRUS, 0xcf08);

	/* GfxPause */
	gtt_write(0xa000, 0x00071388);

	/* Dynamic EU Control Settings */
	gtt_write(0xa080, 0x00000004);

	/* Lock ECO Bit Settings */
	gtt_write(0xa180, 0x80000000);

	/* DOP Clock Gating */
	gtt_write(0x9424, 0x00000001);

	/* MBCunit will send the VCR (Fuse) writes as NP-W */
	gtt_rmw(0x907c, 0xfffeffff, 0x00010000);

	/* RC6 Settings */
	gtt_write_regs(gfx_rc6_registers);

	/* Turbo Settings */
	gtt_write_regs(gfx_turbo_registers);

	/* Turbo Control Settings */
	gtt_write_regs(gfx_turbo_ctl_registers);

	/* RP Idle Hysteresis */
	gtt_write(0xa070, 0x0000000a);

	/* HW RC6 Control Settings */
	gtt_write(0xa090, 0x11000000);

	/* RP Control */
	gtt_write(0xa024, 0x00000592);

	/* Enable PM Interrupts */
	gtt_write(0x44024, 0x03000000);
	gtt_write(0x4402c, 0x03000076);
	gtt_write(0xa168, 0x0000007e);

	/* Lock power context base */
	gfx_lock_pcbase(dev);

	/* Aggressive Clock Gating */
	gtt_write(0x9400, 0);
	gtt_write(0x9404, 0);
	gtt_write(0x9408, 0);
	gtt_write(0x940c, 0);
}

static void gfx_pre_vbios_init(device_t dev)
{
	/* Display */
	iosf_punit_write(PUNIT_PWRGT_CONTROL, 0xc0);
	gfx_poll_power_gate(0xc0, 0xc0);

	/* Tx/Rx Lanes */
	iosf_punit_write(PUNIT_PWRGT_CONTROL, 0xfff0c0);
	gfx_poll_power_gate(0xfff0c0, 0xfff0c0);

	/* Common Lane */
	iosf_punit_write(PUNIT_PWRGT_CONTROL, 0xfffcc0);
	gfx_poll_power_gate(0xfffcc0, 0xfffcc0);

	/* Ungating Tx only */
	iosf_punit_write(PUNIT_PWRGT_CONTROL, 0xf00cc0);
	gfx_poll_power_gate(0xfffcc0, 0xf00cc0);

	/* Ungating Common Lane only */
	iosf_punit_write(PUNIT_PWRGT_CONTROL, 0xf000c0);
	gfx_poll_power_gate(0xffffc0, 0xf000c0);

	/* Ungating Display */
	iosf_punit_write(PUNIT_PWRGT_CONTROL, 0xf00000);
	gfx_poll_power_gate(0xfffff0, 0xf00000);
}

static void gfx_post_vbios_init(device_t dev)
{
	/* Deassert Render Force-Wake */
	gtt_write(0x1300B0, 0x80000000);
	gtt_poll(0x1300B4, 0x8000, 0);

	/* Deassert Media Force-Wake */
	gtt_write(0x1300B8, 0x80000000);
	gtt_poll(0x1300BC, 0x8000, 0);

	/* Set Lock bits */
	pci_write_config32(dev, GGC, pci_read_config32(dev, GGC) | 1);
	pci_write_config32(dev, GSM_BASE, pci_read_config32(dev, GSM_BASE) | 1);
	pci_write_config32(dev, GTT_BASE, pci_read_config32(dev, GTT_BASE) | 1);
}

static void gfx_init(device_t dev)
{
	u32 reg32;

	/* Find GTT resource */
	gtt_res = find_resource(dev, PCI_BASE_ADDRESS_0);
	if (!gtt_res || !gtt_res->base)
		return;

	/* GFX needs to be Bus Master */
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	reg32 |= PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO;
	pci_write_config32(dev, PCI_COMMAND, reg32);

	/* Pre VBIOS Init */
	gfx_pre_vbios_init(dev);
	gfx_pm_init(dev);

	/* Run VBIOS */
	pci_dev_init(dev);

	/* Post VBIOS Init */
	gfx_post_vbios_init(dev);
}

static void gfx_read_resources(device_t dev)
{
	pci_dev_read_resources(dev);

#if CONFIG_MARK_GRAPHICS_MEM_WRCOMB
	struct resource *res;

	/* Set the graphics memory to write combining. */
	res = find_resource(dev, PCI_BASE_ADDRESS_2);
	if (res == NULL) {
		printk(BIOS_DEBUG, "GFX: memory resource not found.\n");
		return;
	}
	res->flags |= IORESOURCE_WRCOMB;
#endif
}

static struct device_operations gfx_device_ops = {
	.read_resources		= gfx_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= gfx_init,
	.ops_pci		= &soc_pci_ops,
};

static const struct pci_driver gfx_driver __pci_driver = {
	.ops	= &gfx_device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= GFX_DEVID,
};
