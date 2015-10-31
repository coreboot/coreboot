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
 */

#include <console/console.h>
#include <bootmode.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <pc80/mc146818rtc.h>
#include <edid.h>
#include <drivers/intel/gma/edid.h>
#include <drivers/intel/gma/i915.h>
#include <string.h>
#include <pc80/vga.h>
#include <pc80/vga_io.h>

#include "i945.h"
#include "chip.h"

#define GDRST 0xc0

#define  LVDS_CLOCK_A_POWERUP_ALL	(3 << 8)
#define  LVDS_CLOCK_B_POWERUP_ALL	(3 << 4)
#define  LVDS_CLOCK_BOTH_POWERUP_ALL	(3 << 2)
#define   DISPPLANE_BGRX888			(0x6<<26)
#define   DPLLB_LVDS_P2_CLOCK_DIV_7	(1 << 24) /* i915 */

#define   DPLL_INTEGRATED_CRI_CLK_VLV	(1<<14)

#define PGETBL_CTL	0x2020
#define PGETBL_ENABLED	0x00000001

#define BASE_FREQUENCY 120000

#if CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT

static int gtt_setup(void *mmiobase)
{
	unsigned long PGETBL_save;
	unsigned long tom; // top of memory

	/*
	 * The Video BIOS places the GTT right below top of memory.
	 */
	tom = pci_read_config8(dev_find_slot(0, PCI_DEVFN(0, 0)), TOLUD) << 24;
	PGETBL_save = tom - 256 * KiB;
	PGETBL_save |= PGETBL_ENABLED;
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
			  void *pmmio, unsigned int pgfx)
{
	struct edid edid;
	struct edid_mode *mode;
	u8 edid_data[128];
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
	u32 uma_size;
	u16 reg16;

	printk(BIOS_SPEW,
	       "i915lightup: graphics %p mmio %p addrport %04x physbase %08x\n",
	       (void *)pgfx, pmmio, piobase, pphysbase);

	intel_gmbus_read_edid(pmmio + GMBUS0, 3, 0x50, edid_data, 128);
	decode_edid(edid_data, sizeof(edid_data), &edid);
	mode = &edid.mode;

	hpolarity = (mode->phsync == '-');
	vpolarity = (mode->pvsync == '-');
	hactive = edid.x_resolution;
	vactive = edid.y_resolution;
	right_border = mode->hborder;
	bottom_border = mode->vborder;
	vblank = mode->vbl;
	hblank = mode->hbl;
	vsync = mode->vspw;
	hsync = mode->hspw;
	hfront_porch = mode->hso;
	vfront_porch = mode->vso;

	for (i = 0; i < 2; i++)
		for (j = 0; j < 0x100; j++)
			/* R=j, G=j, B=j.  */
			write32(pmmio + PALETTE(i) + 4 * j, 0x10101 * j);

	write32(pmmio + PCH_PP_CONTROL, PANEL_UNLOCK_REGS
		| (read32(pmmio + PCH_PP_CONTROL) & ~PANEL_UNLOCK_MASK));

	write32(pmmio + MI_ARB_STATE, MI_ARB_C3_LP_WRITE_ENABLE | (1 << 27));
	/* Clean registers.  */
	for (i = 0; i < 0x20; i += 4)
		write32(pmmio + RENDER_RING_BASE + i, 0);
	for (i = 0; i < 0x20; i += 4)
		write32(pmmio + FENCE_REG_965_0 + i, 0);
	write32(pmmio + PP_ON_DELAYS, 0);
	write32(pmmio + PP_OFF_DELAYS, 0);

	/* Disable VGA.  */
	write32(pmmio + VGACNTRL, VGA_DISP_DISABLE);

	/* Disable pipes.  */
	write32(pmmio + PIPECONF(0), 0);
	write32(pmmio + PIPECONF(1), 0);

	/* Init PRB0.  */
	write32(pmmio + HWS_PGA, 0x352d2000);
	write32(pmmio + PRB0_CTL, 0);
	write32(pmmio + PRB0_HEAD, 0);
	write32(pmmio + PRB0_TAIL, 0);
	write32(pmmio + PRB0_START, 0);
	write32(pmmio + PRB0_CTL, 0x0001f001);

	write32(pmmio + D_STATE, DSTATE_PLL_D3_OFF
		| DSTATE_GFX_CLOCK_GATING | DSTATE_DOT_CLOCK_GATING);
	write32(pmmio + ECOSKPD, 0x00010000);
	write32(pmmio + HWSTAM, 0xeffe);
	write32(pmmio + PORT_HOTPLUG_EN, conf->gpu_hotplug);
	write32(pmmio + INSTPM, 0x08000000 | INSTPM_AGPBUSY_DIS);

	target_frequency = mode->lvds_dual_channel ? mode->pixel_clock
		: (2 * mode->pixel_clock);

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
			m = (target_frequency * denom
			     + BASE_FREQUENCY / 2) / BASE_FREQUENCY;

			if (m < 77 || m > 131)
				continue;

			cur_frequency = (BASE_FREQUENCY * m) / denom;
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

	printk(BIOS_INFO, "bringing up panel at resolution %d x %d\n",
	       hactive, vactive);
	printk(BIOS_DEBUG, "Borders %d x %d\n", right_border, bottom_border);
	printk(BIOS_DEBUG, "Blank %d x %d\n", hblank, vblank);
	printk(BIOS_DEBUG, "Sync %d x %d\n", hsync, vsync);
	printk(BIOS_DEBUG, "Front porch %d x %d\n", hfront_porch, vfront_porch);
	printk(BIOS_DEBUG, (conf->gpu_lvds_use_spread_spectrum_clock
			    ? "Spread spectrum clock\n"
			    : "DREF clock\n"));
	printk(BIOS_DEBUG, (mode->lvds_dual_channel
			    ? "Dual channel\n"
			    : "Single channel\n"));
	printk(BIOS_DEBUG, "Polarities %d, %d\n",
	       hpolarity, vpolarity);
	printk(BIOS_DEBUG, "Pixel N=%d, M1=%d, M2=%d, P1=%d\n",
	       pixel_n, pixel_m1, pixel_m2, pixel_p1);
	printk(BIOS_DEBUG, "Pixel clock %d kHz\n",
	       BASE_FREQUENCY * (5 * pixel_m1 + pixel_m2) / pixel_n
	       / (pixel_p1 * 7));

#if !IS_ENABLED(CONFIG_FRAMEBUFFER_KEEP_VESA_MODE)
	write32(pmmio + PF_WIN_SZ(0), vactive | (hactive << 16));
	write32(pmmio + PF_WIN_POS(0), 0);
	write32(pmmio + PF_CTL(0),PF_ENABLE | PF_FILTER_MED_3x3);
	write32(pmmio + PFIT_CONTROL, PFIT_ENABLE | (1 << PFIT_PIPE_SHIFT) | HORIZ_AUTO_SCALE | VERT_AUTO_SCALE);
#else
	/* Disable panel fitter (we're in native resolution).  */
	write32(pmmio + PF_CTL(0), 0);
	write32(pmmio + PF_WIN_SZ(0), 0);
	write32(pmmio + PF_WIN_POS(0), 0);
	write32(pmmio + PFIT_PGM_RATIOS, 0);
	write32(pmmio + PFIT_CONTROL, 0);
#endif

	mdelay(1);

	write32(pmmio + DSPCNTR(0), DISPPLANE_BGRX888
		| DISPPLANE_SEL_PIPE_B | DISPPLANE_GAMMA_ENABLE);

	mdelay(1);
	write32(pmmio + PP_CONTROL, PANEL_UNLOCK_REGS
		| (read32(pmmio + PP_CONTROL) & ~PANEL_UNLOCK_MASK));
	write32(pmmio + FP0(1),
		((pixel_n - 2) << 16)
		| ((pixel_m1 - 2) << 8) | pixel_m2);
	write32(pmmio + DPLL(1),
		DPLL_VGA_MODE_DIS |
		DPLL_VCO_ENABLE | DPLLB_MODE_LVDS
		| (mode->lvds_dual_channel ? DPLLB_LVDS_P2_CLOCK_DIV_7
		   : DPLLB_LVDS_P2_CLOCK_DIV_14)
		| (conf->gpu_lvds_use_spread_spectrum_clock
		   ? DPLL_INTEGRATED_CLOCK_VLV | DPLL_INTEGRATED_CRI_CLK_VLV
		   : 0)
		| (pixel_p1 << 16)
		| (pixel_p1));
	mdelay(1);
	write32(pmmio + DPLL(1),
		DPLL_VGA_MODE_DIS |
		DPLL_VCO_ENABLE | DPLLB_MODE_LVDS
		| (mode->lvds_dual_channel ? DPLLB_LVDS_P2_CLOCK_DIV_7
		   : DPLLB_LVDS_P2_CLOCK_DIV_14)
		| ((conf->gpu_lvds_use_spread_spectrum_clock ? 3 : 0) << 13)
		| (pixel_p1 << 16)
		| (pixel_p1));
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

#if !IS_ENABLED(CONFIG_FRAMEBUFFER_KEEP_VESA_MODE)
	write32(pmmio + PIPESRC(1), (639 << 16) | 399);
#else
	write32(pmmio + PIPESRC(1), ((hactive - 1) << 16) | (vactive - 1));
#endif
	mdelay(1);

	write32(pmmio + DSPSIZE(0), (hactive - 1) | ((vactive - 1) << 16));
	write32(pmmio + DSPPOS(0), 0);

	/* Backlight init. */
	write32(pmmio + FW_BLC_SELF, FW_BLC_SELF_EN_MASK);
	write32(pmmio + FW_BLC, 0x011d011a);
	write32(pmmio + FW_BLC2, 0x00000102);
	write32(pmmio + FW_BLC_SELF, FW_BLC_SELF_EN_MASK);
	write32(pmmio + FW_BLC_SELF, 0x0001003f);
	write32(pmmio + FW_BLC, 0x011d0109);
	write32(pmmio + FW_BLC2, 0x00000102);
	write32(pmmio + FW_BLC_SELF, FW_BLC_SELF_EN_MASK);
	write32(pmmio + BLC_PWM_CTL, conf->gpu_backlight);

	edid.bytes_per_line = (edid.bytes_per_line + 63) & ~63;
	write32(pmmio + DSPADDR(0), 0);
	write32(pmmio + DSPSURF(0), 0);
	write32(pmmio + DSPSTRIDE(0), edid.bytes_per_line);
	write32(pmmio + DSPCNTR(0), DISPLAY_PLANE_ENABLE | DISPPLANE_BGRX888
		| DISPPLANE_SEL_PIPE_B | DISPPLANE_GAMMA_ENABLE);
	mdelay(1);

	write32(pmmio + PIPECONF(1), PIPECONF_ENABLE);
	write32(pmmio + LVDS, LVDS_ON
		| (hpolarity << 20) | (vpolarity << 21)
		| (mode->lvds_dual_channel ? LVDS_CLOCK_B_POWERUP_ALL
		   | LVDS_CLOCK_BOTH_POWERUP_ALL : 0)
		| LVDS_CLOCK_A_POWERUP_ALL
		| LVDS_PIPE(1));

	write32(pmmio + PP_CONTROL, PANEL_UNLOCK_REGS | PANEL_POWER_OFF);
	write32(pmmio + PP_CONTROL, PANEL_UNLOCK_REGS | PANEL_POWER_RESET);
	mdelay(1);
	write32(pmmio + PP_CONTROL, PANEL_UNLOCK_REGS
		| PANEL_POWER_ON | PANEL_POWER_RESET);

	printk (BIOS_DEBUG, "waiting for panel powerup\n");
	while (1) {
		u32 reg32;
		reg32 = read32(pmmio + PP_STATUS);
		if ((reg32 & PP_SEQUENCE_MASK) == PP_SEQUENCE_NONE)
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

	if (gtt_setup(pmmio)) {
		printk(BIOS_ERR, "ERROR: GTT Setup Failed!!!\n");
		return 0;
	}

	/* Setup GTT.  */

	reg16 = pci_read_config16(dev_find_slot(0, PCI_DEVFN(0, 0)), GGC);
	uma_size = 0;
	if (!(reg16 & 2)) {
		reg16 >>= 4;
		reg16 &= 7;
		switch (reg16) {
		case 1:
			uma_size = 1024;
			break;
		case 3:
			uma_size = 8192;
			break;
		}

		printk(BIOS_DEBUG, "%dM UMA\n", uma_size >> 10);
	}

	for (i = 0; i < (uma_size - 256) / 4; i++)
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

#if !IS_ENABLED(CONFIG_FRAMEBUFFER_KEEP_VESA_MODE)
	vga_misc_write(0x67);

	write32(pmmio + DSPCNTR(0), DISPPLANE_SEL_PIPE_B);

	write32(pmmio + VGACNTRL, 0x02c4008e | VGA_PIPE_B_SELECT);

	vga_textmode_init();
#else
	printk(BIOS_SPEW, "memset %p to 0x00 for %d bytes\n",
	       (void *)pgfx, hactive * vactive * 4);
	memset((void *)pgfx, 0x00, hactive * vactive * 4);

	set_vbe_mode_info_valid(&edid, pgfx);
#endif
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
	void *mmiobase;
	u32 iobase, graphics_base;
	struct northbridge_intel_i945_config *conf = dev->chip_info;

	iobase = dev->resource_list[1].base;
	mmiobase = (void *)(uintptr_t)dev->resource_list[0].base;
	graphics_base = dev->resource_list[2].base;

	printk(BIOS_SPEW, "GMADR=0x%08x GTTADR=0x%08x\n",
		pci_read_config32(dev, GMADR),
		pci_read_config32(dev, GTTADR)
	);

	int err;
	err = intel_gma_init(conf, pci_read_config32(dev, 0x5c) & ~0xf,
			     iobase, mmiobase, graphics_base);
	if (err == 0)
		gfx_set_init_done(1);
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

	/* IGD needs to be Bus Master, also enable IO accesss */
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	pci_write_config32(dev, PCI_COMMAND, reg32 |
			PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO);

	/* Permanently set tft_brightness to 0xff. Ignore nvramtool configuration */
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

const struct i915_gpu_controller_info *
intel_gma_get_controller_info(void)
{
	device_t dev = dev_find_slot(0, PCI_DEVFN(0x2,0));
	if (!dev) {
		return NULL;
	}
	struct northbridge_intel_i945_config *chip = dev->chip_info;
	if (!chip) {
		return NULL;
	}
	return &chip->gfx;
}

static void gma_ssdt(device_t device)
{
	const struct i915_gpu_controller_info *gfx = intel_gma_get_controller_info();
	if (!gfx) {
		return;
	}

	drivers_intel_gma_displays_ssdt_generate(gfx);
}

static struct pci_operations gma_pci_ops = {
	.set_subsystem    = gma_set_subsystem,
};

static struct device_operations gma_func0_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= gma_func0_init,
	.acpi_fill_ssdt_generator = gma_ssdt,
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

static const unsigned short pci_device_ids[] = { 0x27a2, 0x27ae, 0 };

static const struct pci_driver i945_gma_func0_driver __pci_driver = {
	.ops	= &gma_func0_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.devices	= pci_device_ids,
};

static const struct pci_driver i945_gma_func1_driver __pci_driver = {
	.ops	= &gma_func1_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x27a6,
};
