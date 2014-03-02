/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
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

#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <pc80/mc146818rtc.h>
#include "i945.h"
#include "chip.h"
#include <edid.h>
#include <drivers/intel/gma/edid.h>
#include <drivers/intel/gma/i915.h>
#include <string.h>

#define GDRST 0xc0

#define  LVDS_CLOCK_A_POWERUP_ALL	(3 << 8)
#define  LVDS_CLOCK_B_POWERUP_ALL	(3 << 4)
#define  LVDS_CLOCK_BOTH_POWERUP_ALL	(3 << 2)
#define   DISPPLANE_BGRX888			(0x6<<26)
#define   DPLLB_LVDS_P2_CLOCK_DIV_7	(1 << 24) /* i915 */

#define PGETBL_CTL	0x2020
#define PGETBL_ENABLED	0x00000001

#if CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT

static int gtt_setup(unsigned int mmiobase)
{
	unsigned long PGETBL_save;

	PGETBL_save = read32(mmiobase + PGETBL_CTL) & ~PGETBL_ENABLED;
	PGETBL_save |= PGETBL_ENABLED;

	PGETBL_save |= pci_read_config32(dev_find_slot(0, PCI_DEVFN(2,0)), 0x5c) & 0xfffff000;
	PGETBL_save |= 2; /* set GTT to 256kb */

	write32(mmiobase + GFX_FLSH_CNTL, 0);

	write32(mmiobase + PGETBL_CTL, PGETBL_save);

	/* verify */
	if (read32(mmiobase + PGETBL_CTL) & PGETBL_ENABLED) {
		printk(BIOS_DEBUG, "gtt_setup is enabled.\n");
	} else {
		printk(BIOS_DEBUG, "gtt_setup failed!!!\n");
		return 1;
	}
	write32(mmiobase + GFX_FLSH_CNTL, 0);

	return 0;
}

static int intel_gma_init(struct northbridge_intel_i945_config *conf,
			  unsigned int pphysbase, unsigned int piobase,
			  unsigned int pmmio, unsigned int pgfx)
{
	struct edid edid;
	u8 edid_data[256];
	unsigned long temp;
	int hpolarity, vpolarity;
	u32 candp1, candn;
	u32 best_delta = 0xffffffff;
	u32 target_frequency;
	u32 pixel_p1 = 1;
	u32 pixel_n = 1;
	u32 pixel_m1 = 1;
	u32 pixel_m2 = 1;
	u32 hactive, vactive, right_border, bottom_border;
	u32 vsync, hsync, vblank, hblank, hfront_porch, vfront_porch;
	u32 i, j;

	pphysbase += 0x20000;

	printk(BIOS_SPEW,
		"i915lightup: graphics %p mmio %08x addrport %04x physbase %08x\n",
		(void *)pgfx, pmmio, piobase, pphysbase);

	intel_gmbus_read_edid(pmmio + GMBUS0, 3, 0x50, edid_data);
	decode_edid(edid_data,
		    sizeof(edid_data), &edid);

	hpolarity = (edid.phsync == '-');
	vpolarity = (edid.pvsync == '-');
	hactive = edid.x_resolution;
	vactive = edid.y_resolution;
	right_border = edid.hborder;
	bottom_border = edid.vborder;
	vblank = edid.vbl;
	hblank = edid.hbl;
	vsync = edid.vspw;
	hsync = edid.hspw;
	hfront_porch = edid.hso;
	vfront_porch = edid.vso;

	for (i = 0; i < 2; i++)
		for (j = 0; j < 0x100; j++)
			write32(pmmio + PALETTE(i) + 4 * j, 0x10101 * j);

	write32(pmmio + PCH_PP_CONTROL, PANEL_UNLOCK_REGS
		| (read32(pmmio + PCH_PP_CONTROL) & ~PANEL_UNLOCK_MASK));

	write32(pmmio+MI_ARB_STATE, MI_ARB_C3_LP_WRITE_ENABLE | (1 << 27));
	/* Clean registers.  */
	for (i = 0; i < 0x20; i += 4)
		write32(pmmio + RENDER_RING_BASE + i, 0);
	for (i = 0; i < 0x20; i += 4)
		write32(pmmio + FENCE_REG_965_0 + i, 0);
	write32(pmmio+PP_ON_DELAYS, 0);
	write32(pmmio+PP_OFF_DELAYS, 0);

	/* Disable VGA.  */
	write32(pmmio+VGACNTRL, VGA_DISP_DISABLE);

	/* Disable pipes.  */
	write32(pmmio+PIPECONF(0), 0);
	write32(pmmio+PIPECONF(1), 0);

	/* Init PRB0.  */
	write32(pmmio+HWS_PGA,0x352d2000);
	write32(pmmio+PRB0_CTL, 0);
	write32(pmmio+PRB0_HEAD, 0);
	write32(pmmio+PRB0_TAIL, 0);
	write32(pmmio+PRB0_START, 0);
	write32(pmmio+PRB0_CTL, 0x0001f001);

	write32(pmmio+D_STATE, DSTATE_PLL_D3_OFF
		| DSTATE_GFX_CLOCK_GATING | DSTATE_DOT_CLOCK_GATING);
	write32(pmmio+ECOSKPD,0x00010000);
	write32(pmmio+HWSTAM,0x0000effe);
	write32(pmmio+PORT_HOTPLUG_EN, conf->gpu_hotplug);
	write32(pmmio+INSTPM,0x08000000 | INSTPM_AGPBUSY_DIS);

	target_frequency = conf->gpu_lvds_is_dual_channel ? edid.pixel_clock
		: (2 * edid.pixel_clock);

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
		return -1;
	}

	write32(pmmio + DSPCNTR(0), DISPPLANE_BGRX888
		| DISPPLANE_SEL_PIPE_B | DISPPLANE_GAMMA_ENABLE);

	mdelay(1);
	write32(pmmio + PP_CONTROL, PANEL_UNLOCK_REGS
		| (read32(pmmio + PP_CONTROL) & ~PANEL_UNLOCK_MASK));
	write32(pmmio + FP0(1),
		((pixel_n - 2) << 16)
		| ((pixel_m1 - 2) << 8) | pixel_m2);
	write32(pmmio + DPLL(1),
		DPLL_VCO_ENABLE | DPLLB_MODE_LVDS
		| (conf->gpu_lvds_is_dual_channel ? DPLLB_LVDS_P2_CLOCK_DIV_7
		   : DPLLB_LVDS_P2_CLOCK_DIV_14)
		| (0x10000 << (pixel_p1 - 1))
		| ((conf->gpu_lvds_use_spread_spectrum_clock ? 3 : 0) << 13)
		| (0x1 << (pixel_p1 - 1)));
	mdelay(1);
	write32(pmmio + DPLL(1),
		DPLL_VCO_ENABLE | DPLLB_MODE_LVDS
		| (conf->gpu_lvds_is_dual_channel ? DPLLB_LVDS_P2_CLOCK_DIV_7
		   : DPLLB_LVDS_P2_CLOCK_DIV_14)
		| (0x10000 << (pixel_p1 - 1))
		| ((conf->gpu_lvds_use_spread_spectrum_clock ? 3 : 0) << 13)
		| (0x1 << (pixel_p1 - 1)));
	mdelay(1);
	write32(pmmio + HTOTAL(1),
		((hactive + right_border + hblank - 1) << 16)
		| (hactive - 1));
	write32(pmmio + HBLANK(1),
		((hactive + right_border + hblank - 1) << 16)
		| (hactive + right_border - 1));
	write32(pmmio + HSYNC(1),
		((hactive + right_border + hfront_porch + hsync - 1) << 16)
		| (hactive + right_border + hfront_porch - 1));

	write32(pmmio + VTOTAL(1), ((vactive + bottom_border + vblank - 1) << 16)
		| (vactive - 1));
	write32(pmmio + VBLANK(1), ((vactive + bottom_border + vblank - 1) << 16)
		| (vactive + bottom_border - 1));
	write32(pmmio + VSYNC(1),
		(vactive + bottom_border + vfront_porch + vsync - 1)
		| (vactive + bottom_border + vfront_porch - 1));

	write32(pmmio + PIPESRC(1), ((hactive - 1) << 16) | (vactive - 1));

	/* Disable panel fitter (we're in native resolution).  */
	write32(pmmio + PF_CTL(0),0);
	write32(pmmio + PF_WIN_SZ(0), 0);
	write32(pmmio + PF_WIN_POS(0), 0);
	write32(pmmio+PFIT_PGM_RATIOS, 0);
	write32(pmmio+PFIT_CONTROL, 0);

	mdelay(1);

	write32(pmmio+DSPSIZE(0),(hactive - 1) | ((vactive - 1) << 16));
	write32(pmmio+DSPPOS(0),0);

	/* Backlight init. */
	write32(pmmio+FW_BLC_SELF, FW_BLC_SELF_EN_MASK);
	write32(pmmio+FW_BLC,0x011d011a);
	write32(pmmio+FW_BLC2,0x00000102);
	write32(pmmio+FW_BLC_SELF, FW_BLC_SELF_EN_MASK);
	write32(pmmio+FW_BLC_SELF,0x0001003f);
	write32(pmmio+FW_BLC,0x011d0109);
	write32(pmmio+FW_BLC2,0x00000102);
	write32(pmmio+FW_BLC_SELF, FW_BLC_SELF_EN_MASK);
	write32(pmmio+BLC_PWM_CTL,conf->gpu_backlight);

	edid.bytes_per_line = (edid.bytes_per_line + 63) & ~63;
	write32(pmmio + DSPADDR(0), 0);
	write32(pmmio + DSPSURF(0), 0);
	write32(pmmio + DSPSTRIDE(0), edid.bytes_per_line);
	write32(pmmio + DSPCNTR(0), DISPLAY_PLANE_ENABLE | DISPPLANE_BGRX888
		| DISPPLANE_SEL_PIPE_B | DISPPLANE_GAMMA_ENABLE);
	mdelay(1);

	write32(pmmio + PIPECONF(1), (1 << 31));
	write32(pmmio + LVDS,
		LVDS_PORT_ENABLE
		| (hpolarity << 20) | (vpolarity << 21)
		| (conf->gpu_lvds_is_dual_channel ? LVDS_CLOCK_B_POWERUP_ALL
		   | LVDS_CLOCK_BOTH_POWERUP_ALL : 0)
		| LVDS_CLOCK_A_POWERUP_ALL | (1 << 30));

	write32(pmmio + PP_CONTROL, PANEL_UNLOCK_REGS | PANEL_POWER_OFF);
	write32(pmmio + PP_CONTROL, PANEL_UNLOCK_REGS | PANEL_POWER_RESET);
	mdelay(1);
	write32(pmmio + PP_CONTROL, PANEL_UNLOCK_REGS
		| PANEL_POWER_ON | PANEL_POWER_RESET);

	printk (BIOS_DEBUG, "waiting for panel powerup\n");
	while (1) {
		u32 reg32;
		reg32 = read32(pmmio + PP_STATUS);
		if (((reg32 >> 28) & 3) == 0)
			break;
	}
	printk (BIOS_DEBUG, "panel powered up\n");

	write32(pmmio + PP_CONTROL, PANEL_POWER_ON | PANEL_POWER_RESET);

	/* Clear interrupts. */
	write32(pmmio + DEIIR, 0xffffffff);
	write32(pmmio + SDEIIR, 0xffffffff);
	write32(pmmio + IIR, 0xffffffff);
	write32(pmmio + IMR, 0xffffffff);
	write32(pmmio + EIR, 0xffffffff);

	/* GTT is the Global Translation Table for the graphics pipeline.
	 * It is used to translate graphics addresses to physical
	 * memory addresses. As in the CPU, GTTs map 4K pages.
	 * There are 32 bits per pixel, or 4 bytes,
	 * which means 1024 pixels per page.
	 * There are 4250 GTTs on Link:
	 * 2650 (X) * 1700 (Y) pixels / 1024 pixels per page.
	 * The setgtt function adds a further bit of flexibility:
	 * it allows you to set a range (the first two parameters) to point
	 * to a physical address (third parameter);the physical address is
	 * incremented by a count (fourth parameter) for each GTT in the
	 * range.
	 * Why do it this way? For ultrafast startup,
	 * we can point all the GTT entries to point to one page,
	 * and set that page to 0s:
	 * memset(physbase, 0, 4096);
	 * setgtt(0, 4250, physbase, 0);
	 * this takes about 2 ms, and is a win because zeroing
	 * the page takes a up to 200 ms. We will be exploiting this
	 * trick in a later rev of this code.
	 * This call sets the GTT to point to a linear range of pages
	 * starting at physbase.
	 */

	if (gtt_setup(pmmio)) {
		printk(BIOS_ERR, "ERROR: GTT Setup Failed!!!\n");
		return 0;
	}

	/* Setup GTT.  */
	for (i = 0; i < 0x2000; i++)
	{
		outl((i << 2) | 1, piobase);
		outl(pphysbase + (i << 12) + 1, piobase + 4);
	}

	temp = read32(pmmio + PGETBL_CTL);
	printk(BIOS_INFO, "GTT PGETBL_CTL register: 0x%lx\n", temp);

	if (temp & 1)
		printk(BIOS_INFO, "GTT Enabled\n");
	else
		printk(BIOS_ERR, "ERROR: GTT is still Disabled!!!\n");

	printk(BIOS_SPEW, "memset %p to 0x00 for %d bytes\n",
		(void *)pgfx, hactive * vactive * 4);
	memset((void *)pgfx, 0x00, hactive * vactive * 4);

	set_vbe_mode_info_valid(&edid, pgfx);

	return 0;
}
#endif

static void gma_func0_init(struct device *dev)
{
	u32 reg32;

	/* Unconditionally reset graphics */
	pci_write_config8(dev, GDRST, 1);
	udelay(50);
	pci_write_config8(dev, GDRST, 0);
	/* wait for device to finish */
	while (pci_read_config8(dev, GDRST) & 1) { };

	/* IGD needs to be Bus Master */
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	pci_write_config32(dev, PCI_COMMAND, reg32 | PCI_COMMAND_MASTER
		 | PCI_COMMAND_IO | PCI_COMMAND_MEMORY);

#if !CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT
	/* PCI Init, will run VBIOS */
	pci_dev_init(dev);
#endif


#if CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT
	/* This should probably run before post VBIOS init. */
	printk(BIOS_SPEW, "Initializing VGA without OPROM.\n");
	u32 iobase, mmiobase, graphics_base;
	struct northbridge_intel_i945_config *conf = dev->chip_info;

	iobase = dev->resource_list[1].base;
	mmiobase = dev->resource_list[0].base;
	graphics_base = dev->resource_list[2].base;

	printk(BIOS_SPEW, "GMADR=0x%08x GTTADR=0x%08x\n",
		pci_read_config32(dev, 0x18),
		pci_read_config32(dev, 0x1c)
	);

	intel_gma_init(conf, pci_read_config32(dev, 0x5c) & ~0xf,
		       iobase, mmiobase, graphics_base);
#endif

}

/* This doesn't reclaim stolen UMA memory, but IGD could still
   be reenabled later. */
static void gma_func0_disable(struct device *dev)
{
	struct device *dev_host = dev_find_slot(0, PCI_DEVFN(0x0, 0));

	pci_write_config16(dev, GCFC, 0xa00);
	pci_write_config16(dev_host, GGC, (1 << 1));

	unsigned int reg32 = pci_read_config32(dev_host, DEVEN);
	reg32 &= ~(DEVEN_D2F0 | DEVEN_D2F1);
	pci_write_config32(dev_host, DEVEN, reg32);

	dev->enabled = 0;
}

static void gma_func1_init(struct device *dev)
{
	u32 reg32;
	u8 val;

	/* IGD needs to be Bus Master, also enable IO accesss */
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	pci_write_config32(dev, PCI_COMMAND, reg32 |
			PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO);

	if (get_option(&val, "tft_brightness") == CB_SUCCESS)
		pci_write_config8(dev, 0xf4, val);
	else
		pci_write_config8(dev, 0xf4, 0xff);
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
	.disable		= gma_func0_disable,
	.ops_pci		= &gma_pci_ops,
};


static struct device_operations gma_func1_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= gma_func1_init,
	.scan_bus		= 0,
	.enable			= 0,
	.ops_pci		= &gma_pci_ops,
};

static const struct pci_driver i945_gma_func0_driver __pci_driver = {
	.ops	= &gma_func0_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x27a2,
};

static const struct pci_driver i945_gma_func1_driver __pci_driver = {
	.ops	= &gma_func1_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x27a6,
};

