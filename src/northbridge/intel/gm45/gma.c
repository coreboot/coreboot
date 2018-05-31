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
#include <commonlib/helpers.h>
#include <cbmem.h>
#include <southbridge/intel/i82801ix/nvs.h>

#include "drivers/intel/gma/i915_reg.h"
#include "chip.h"
#include "gm45.h"
#include <drivers/intel/gma/intel_bios.h>
#include <drivers/intel/gma/edid.h>
#include <drivers/intel/gma/i915.h>
#include <drivers/intel/gma/opregion.h>
#include <pc80/vga.h>
#include <pc80/vga_io.h>

#define BASE_FREQUENCY 96000

static struct resource *gtt_res = NULL;

u32 gtt_read(u32 reg)
{
	return read32(res2mmio(gtt_res, reg, 0));
}

void gtt_write(u32 reg, u32 data)
{
	write32(res2mmio(gtt_res, reg, 0), data);
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

static void gma_init_lvds(const struct northbridge_intel_gm45_config *info,
			u8 *mmio, u32 physbase, u16 piobase, u32 lfb,
			struct edid *edid)
{
	int i;
	struct edid_mode *mode;
	u32 hactive, vactive, right_border, bottom_border;
	int hpolarity, vpolarity;
	u32 vsync, hsync, vblank, hblank, hfront_porch, vfront_porch;
	u32 smallest_err = 0xffffffff;
	u32 target_frequency;
	u32 pixel_p1 = 1;
	u32 pixel_n = 1;
	u32 pixel_m1 = 1;
	u32 pixel_m2 = 1;
	u32 pixel_p2;

	vga_gr_write(0x18, 0);

	/* Setup GTT.  */
	for (i = 0; i < 0x2000; i++)
	{
		outl((i << 2) | 1, piobase);
		outl(physbase + (i << 12) + 1, piobase + 4);
	}

	write32(mmio + ADPA, 0x40008c18);
	write32(mmio + 0x7041c, 0x0);
	write32(mmio + _DPLL_B_MD, 0x3);

	vga_misc_write(0x67);

	const u8 cr[] = { 0x5f, 0x4f, 0x50, 0x82, 0x55, 0x81, 0xbf, 0x1f,
		    0x00, 0x4f, 0x0d, 0x0e, 0x00, 0x00, 0x00, 0x00,
		    0x9c, 0x8e, 0x8f, 0x28, 0x1f, 0x96, 0xb9, 0xa3,
		    0xff
	};
	vga_cr_write(0x11, 0);

	for (i = 0; i <= 0x18; i++)
		vga_cr_write(i, cr[i]);

	/* Disable screen memory to prevent garbage from appearing. */
	vga_sr_write(1, vga_sr_read(1) | 0x20);

	mode = &edid->mode;

	hactive = edid->x_resolution;
	vactive = edid->y_resolution;
	right_border = mode->hborder;
	bottom_border = mode->vborder;
	hpolarity = (mode->phsync == '-');
	vpolarity = (mode->pvsync == '-');
	vsync = mode->vspw;
	hsync = mode->hspw;
	vblank = mode->vbl;
	hblank = mode->hbl;
	hfront_porch = mode->hso;
	vfront_porch = mode->vso;

	if (IS_ENABLED(CONFIG_LINEAR_FRAMEBUFFER)) {
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

		edid->bytes_per_line = (edid->bytes_per_line + 63) & ~63;

		write32(mmio + DSPCNTR(0), DISPPLANE_BGRX888);
		write32(mmio + DSPADDR(0), 0);
		write32(mmio + DSPSTRIDE(0), edid->bytes_per_line);
		write32(mmio + DSPSURF(0), 0);
		for (i = 0; i < 0x100; i++)
			write32(mmio + LGC_PALETTE(0) + 4 * i, i * 0x010101);
	} else {
		vga_textmode_init();
	}

	target_frequency = mode->pixel_clock;
	/*
	 * p2 divisor must 7 for dual channel LVDS
	 * and 14 for single channel LVDS
	*/
	pixel_p2 = mode->lvds_dual_channel ? 7 : 14;

	/*
	 * Find suitable divisors, m1, m2, p1, n.
	 * refclock * (5 * (m1 + 2) + (m1 + 2)) / (n + 2) / p1 / p2
	 * should be closest to target frequency as possible
	 */
	u32 candn, candm1, candm2, candp1;
	for (candn = 1; candn <= 3; candn++) {
		for (candm1 = 23; candm1 >= 17; candm1--) {
			for (candm2 = 11; candm2 >= 5; candm2--) {
				for (candp1 = mode->lvds_dual_channel ? 6 : 8;
				     candp1 >= 2; candp1--) {
					u32 m = 5 * (candm1 + 2) + (candm2 + 2);
					u32 p = candp1 * pixel_p2;
					u32 vco = DIV_ROUND_CLOSEST(BASE_FREQUENCY * m, candn + 2);
					u32 dot = DIV_ROUND_CLOSEST(vco, p);
					u32 this_err = MAX(dot, target_frequency) -
						MIN(dot, target_frequency);
					if (this_err < smallest_err) {
						smallest_err = this_err;
						pixel_n = candn;
						pixel_m1 = candm1;
						pixel_m2 = candm2;
						pixel_p1 = candp1;
					}
				}
			}
		}
	}

	if (smallest_err == 0xffffffff) {
		printk (BIOS_ERR, "Couldn't find GFX clock divisors\n");
		return;
	}

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
	printk(BIOS_DEBUG, (info->gfx.use_spread_spectrum_clock
			    ? "Spread spectrum clock\n" : "DREF clock\n"));
	printk(BIOS_DEBUG,
	       mode->lvds_dual_channel ? "Dual channel\n" : "Single channel\n");
	printk(BIOS_DEBUG, "Polarities %d, %d\n",
	       hpolarity, vpolarity);
	printk(BIOS_DEBUG, "Pixel N=%d, M1=%d, M2=%d, P1=%d\n",
	       pixel_n, pixel_m1, pixel_m2, pixel_p1);
	printk(BIOS_DEBUG, "Pixel clock %d kHz\n",
		BASE_FREQUENCY * (5 * (pixel_m1 + 2) + (pixel_m2 + 2)) /
			(pixel_n + 2) / (pixel_p1 * pixel_p2));

	write32(mmio + LVDS,
		(hpolarity << 20) | (vpolarity << 21)
		| (mode->lvds_dual_channel ? LVDS_CLOCK_B_POWERUP_ALL
		   | LVDS_CLOCK_BOTH_POWERUP_ALL : 0)
		| LVDS_ENABLE_DITHER
		| LVDS_CLOCK_A_POWERUP_ALL
		| LVDS_PIPE(0));
	mdelay(1);
	write32(mmio + PP_CONTROL, PANEL_UNLOCK_REGS
		| (read32(mmio + PP_CONTROL) & ~PANEL_UNLOCK_MASK));
	write32(mmio + FP0(0),
		(pixel_n << 16)
		| (pixel_m1 << 8) | (pixel_m2));
	write32(mmio + DPLL(0),
		DPLL_VCO_ENABLE | DPLLB_MODE_LVDS
		| DPLL_VGA_MODE_DIS
		| (mode->lvds_dual_channel ? DPLLB_LVDS_P2_CLOCK_DIV_7
		   : DPLLB_LVDS_P2_CLOCK_DIV_14)
		| (0x10000 << (pixel_p1 - 1))
		| ((info->gfx.use_spread_spectrum_clock ? 3 : 0) << 13)
		| (6 << PLL_LOAD_PULSE_PHASE_SHIFT));
	mdelay(1);
	write32(mmio + DPLL(0),
		DPLL_VCO_ENABLE | DPLLB_MODE_LVDS
		| DPLL_VGA_MODE_DIS
		| (mode->lvds_dual_channel ? DPLLB_LVDS_P2_CLOCK_DIV_7
		   : DPLLB_LVDS_P2_CLOCK_DIV_14)
		| (0x10000 << (pixel_p1 - 1))
		| ((info->gfx.use_spread_spectrum_clock ? 3 : 0) << 13)
		| (6 << PLL_LOAD_PULSE_PHASE_SHIFT));
	/* Re-lock the registers.  */
	write32(mmio + PP_CONTROL,
		(read32(mmio + PP_CONTROL) & ~PANEL_UNLOCK_MASK));

	write32(mmio + LVDS,
		(hpolarity << 20) | (vpolarity << 21)
		| (mode->lvds_dual_channel ? LVDS_CLOCK_B_POWERUP_ALL
		   | LVDS_CLOCK_BOTH_POWERUP_ALL : 0)
		| LVDS_CLOCK_A_POWERUP_ALL
		| LVDS_ENABLE_DITHER
		| LVDS_PIPE(0));

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
		((vactive + bottom_border + vfront_porch + vsync - 1) << 16)
		| (vactive + bottom_border + vfront_porch - 1));

	write32(mmio + PIPECONF(0), PIPECONF_DISABLE);

	write32(mmio + PF_WIN_POS(0), 0);
	if (IS_ENABLED(CONFIG_LINEAR_FRAMEBUFFER)) {
		write32(mmio + PIPESRC(0), ((hactive - 1) << 16)
			| (vactive - 1));
		write32(mmio + PF_CTL(0), 0);
		write32(mmio + PF_WIN_SZ(0), 0);
		write32(mmio + PFIT_CONTROL, 0);
	} else {
		write32(mmio + PIPESRC(0), (639 << 16) | 399);
		write32(mmio + PF_CTL(0), PF_ENABLE | PF_FILTER_MED_3x3);
		write32(mmio + PF_WIN_SZ(0), vactive | (hactive << 16));
		write32(mmio + PFIT_CONTROL, 0x80000000);
	}

	mdelay(1);
	write32(mmio + PIPECONF(0), PIPECONF_BPP_6);
	write32(mmio + PIPECONF(0), PIPECONF_BPP_6 | PIPECONF_DITHER_EN);
	write32(mmio + PIPECONF(0), PIPECONF_ENABLE | PIPECONF_BPP_6 | PIPECONF_DITHER_EN);

	if (IS_ENABLED(CONFIG_LINEAR_FRAMEBUFFER)) {
		write32(mmio + VGACNTRL, VGA_DISP_DISABLE);
		write32(mmio + DSPCNTR(0), DISPLAY_PLANE_ENABLE
			| DISPPLANE_BGRX888);
		mdelay(1);
	} else {
		write32(mmio + VGACNTRL, 0xc4008e);
	}

	write32(mmio + LVDS, LVDS_PORT_ENABLE
		| (hpolarity << 20) | (vpolarity << 21)
		| (mode->lvds_dual_channel ? LVDS_CLOCK_B_POWERUP_ALL
		   | LVDS_CLOCK_BOTH_POWERUP_ALL : 0)
		| LVDS_CLOCK_A_POWERUP_ALL
		| LVDS_ENABLE_DITHER
		| LVDS_PIPE(0));

	write32(mmio + PP_CONTROL, PANEL_POWER_ON | PANEL_POWER_RESET);

	/* Enable screen memory.  */
	vga_sr_write(1, vga_sr_read(1) & ~0x20);

	/* Clear interrupts. */
	write32(mmio + DEIIR, 0xffffffff);
	write32(mmio + SDEIIR, 0xffffffff);

	if (IS_ENABLED(CONFIG_LINEAR_FRAMEBUFFER)) {
		memset((void *) lfb, 0,
		       edid->x_resolution * edid->y_resolution * 4);
		set_vbe_mode_info_valid(edid, lfb);
	}
}

static void gma_init_vga(const struct northbridge_intel_gm45_config *info,
			u8 *mmio, u32 physbase, u16 piobase, u32 lfb,
			struct edid *edid)
{

	int i;
	struct edid_mode *mode;
	u32 hactive, vactive, right_border, bottom_border;
	int hpolarity, vpolarity;
	u32 vsync, hsync, vblank, hblank, hfront_porch, vfront_porch;
	u32 target_frequency;
	u32 smallest_err = 0xffffffff;
	u32 pixel_p1 = 1;
	u32 pixel_p2;
	u32 pixel_n = 1;
	u32 pixel_m1 = 1;
	u32 pixel_m2 = 1;

	vga_gr_write(0x18, 0);

	/* Set up GTT.  */
	for (i = 0; i < 0x2000; i++) {
		outl((i << 2) | 1, piobase);
		outl(physbase + (i << 12) + 1, piobase + 4);
	}


	write32(mmio + VGA0, 0x31108);
	write32(mmio + VGA1, 0x31406);

	write32(mmio + ADPA, ADPA_DAC_ENABLE
			| ADPA_PIPE_A_SELECT
			| ADPA_CRT_HOTPLUG_MONITOR_COLOR
			| ADPA_CRT_HOTPLUG_ENABLE
			| ADPA_USE_VGA_HVPOLARITY
			| ADPA_VSYNC_CNTL_ENABLE
			| ADPA_HSYNC_CNTL_ENABLE
			| ADPA_DPMS_ON);

	write32(mmio + 0x7041c, 0x0);
	write32(mmio + DPLL_MD(0), 0x3);
	write32(mmio + DPLL_MD(1), 0x3);

	vga_misc_write(0x67);

	const u8 cr[] = { 0x5f, 0x4f, 0x50, 0x82, 0x55, 0x81, 0xbf, 0x1f,
		    0x00, 0x4f, 0x0d, 0x0e, 0x00, 0x00, 0x00, 0x00,
		    0x9c, 0x8e, 0x8f, 0x28, 0x1f, 0x96, 0xb9, 0xa3,
		    0xff
	};
	vga_cr_write(0x11, 0);

	for (i = 0; i <= 0x18; i++)
		vga_cr_write(i, cr[i]);

	udelay(1);

	/* Disable screen memory to prevent garbage from appearing.  */
	vga_sr_write(1, vga_sr_read(1) | 0x20);

	mode = &edid->mode;

	hactive = edid->x_resolution;
	vactive = edid->y_resolution;
	right_border = mode->hborder;
	bottom_border = mode->vborder;
	hpolarity = (mode->phsync == '-');
	vpolarity = (mode->pvsync == '-');
	vsync = mode->vspw;
	hsync = mode->hspw;
	vblank = mode->vbl;
	hblank = mode->hbl;
	hfront_porch = mode->hso;
	vfront_porch = mode->vso;
	target_frequency = mode->pixel_clock;

	if (IS_ENABLED(CONFIG_LINEAR_FRAMEBUFFER)) {
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

		edid->bytes_per_line = (edid->bytes_per_line + 63) & ~63;

		write32(mmio + DSPCNTR(0), DISPLAY_PLANE_ENABLE
			| DISPPLANE_BGRX888);
		write32(mmio + DSPADDR(0), 0);
		write32(mmio + DSPSTRIDE(0), edid->bytes_per_line);
		write32(mmio + DSPSURF(0), 0);
		for (i = 0; i < 0x100; i++)
			write32(mmio + LGC_PALETTE(0) + 4 * i, i * 0x010101);
	} else {
		vga_textmode_init();
	}

	pixel_p2 = target_frequency <= 225000 ? 10 : 5;

	u32 candn, candm1, candm2, candp1;
	for (candn = 1; candn <= 4; candn++) {
		for (candm1 = 23; candm1 >= 17; candm1--) {
			for (candm2 = 11; candm2 >= 5; candm2--) {
				for (candp1 = 8; candp1 >= 1; candp1--) {
					u32 m = 5 * (candm1 + 2) + (candm2 + 2);
					u32 p = candp1 * pixel_p2;
					u32 vco = DIV_ROUND_CLOSEST(BASE_FREQUENCY * m, candn + 2);
					u32 dot = DIV_ROUND_CLOSEST(vco, p);
					u32 this_err = MAX(dot, target_frequency) -
						MIN(dot, target_frequency);
					if (this_err < smallest_err) {
						smallest_err= this_err;
						pixel_n = candn;
						pixel_m1 = candm1;
						pixel_m2 = candm2;
						pixel_p1 = candp1;
					}
				}
			}
		}
	}

	if (smallest_err == 0xffffffff) {
		printk(BIOS_ERR, "Error: Couldn't find GFX clock divisors\n");
		return;
	}

	printk(BIOS_INFO, "Bringing up panel at resolution %d x %d\n",
	       hactive, vactive);
	printk(BIOS_SPEW, "Borders %d x %d\n",
	       right_border, bottom_border);
	printk(BIOS_SPEW, "Blank %d x %d\n",
	       hblank, vblank);
	printk(BIOS_SPEW, "Sync %d x %d\n",
	       hsync, vsync);
	printk(BIOS_SPEW, "Front porch %d x %d\n",
	       hfront_porch, vfront_porch);
	printk(BIOS_SPEW, (info->gfx.use_spread_spectrum_clock
			    ? "Spread spectrum clock\n" : "DREF clock\n"));
	printk(BIOS_SPEW, "Polarities %d, %d\n",
	       hpolarity, vpolarity);
	printk(BIOS_SPEW, "Pixel N=%d, M1=%d, M2=%d, P1=%d, P2=%d\n",
		pixel_n, pixel_m1, pixel_m2, pixel_p1, pixel_p2);
	printk(BIOS_SPEW, "Pixel clock %d kHz\n",
		BASE_FREQUENCY * (5 * (pixel_m1 + 2) + (pixel_m2 + 2)) /
			(pixel_n + 2) / (pixel_p1 * pixel_p2));

	mdelay(1);
	write32(mmio + FP0(0), (pixel_n << 16)
		| (pixel_m1 << 8) | pixel_m2);
	write32(mmio + DPLL(0), DPLL_VCO_ENABLE
		| DPLL_VGA_MODE_DIS | DPLLB_MODE_DAC_SERIAL
		| (pixel_p2 == 10 ? DPLL_DAC_SERIAL_P2_CLOCK_DIV_10 :
			DPLL_DAC_SERIAL_P2_CLOCK_DIV_5)
		| (0x10000 << (pixel_p1 - 1))
		| (6 << 9));

	mdelay(1);
	write32(mmio + DPLL(0), DPLL_VCO_ENABLE
		| DPLL_VGA_MODE_DIS | DPLLB_MODE_DAC_SERIAL
		| (pixel_p2 == 10 ? DPLL_DAC_SERIAL_P2_CLOCK_DIV_10 :
			DPLL_DAC_SERIAL_P2_CLOCK_DIV_5)
		| (0x10000 << (pixel_p1 - 1))
		| (6 << 9));

	write32(mmio + ADPA, ADPA_DAC_ENABLE
			| ADPA_PIPE_A_SELECT
			| ADPA_CRT_HOTPLUG_MONITOR_COLOR
			| ADPA_CRT_HOTPLUG_ENABLE
			| ADPA_VSYNC_CNTL_ENABLE
			| ADPA_HSYNC_CNTL_ENABLE
			| ADPA_DPMS_ON
			| (vpolarity ? ADPA_VSYNC_ACTIVE_LOW :
			   ADPA_VSYNC_ACTIVE_HIGH)
			| (hpolarity ? ADPA_HSYNC_ACTIVE_LOW :
			   ADPA_HSYNC_ACTIVE_HIGH));

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
		((vactive + bottom_border + vfront_porch + vsync - 1) << 16)
		| (vactive + bottom_border + vfront_porch - 1));

	write32(mmio + PIPECONF(0), PIPECONF_DISABLE);

	write32(mmio + PF_WIN_POS(0), 0);
	if (IS_ENABLED(CONFIG_LINEAR_FRAMEBUFFER)) {
		write32(mmio + PIPESRC(0), ((hactive - 1) << 16)
			| (vactive - 1));
		write32(mmio + PF_CTL(0), 0);
		write32(mmio + PF_WIN_SZ(0), 0);
		write32(mmio + PFIT_CONTROL, 0);
	} else {
		write32(mmio + PIPESRC(0), (639 << 16) | 399);
		write32(mmio + PF_CTL(0), PF_ENABLE | PF_FILTER_MED_3x3);
		write32(mmio + PF_WIN_SZ(0), vactive | (hactive << 16));
		write32(mmio + PFIT_CONTROL, 0x80000000);
	}

	mdelay(1);
	write32(mmio + PIPECONF(0), PIPECONF_BPP_6);
	write32(mmio + PIPECONF(0), PIPECONF_BPP_6 | PIPECONF_DITHER_EN);
	write32(mmio + PIPECONF(0), PIPECONF_ENABLE
			| PIPECONF_BPP_6 | PIPECONF_DITHER_EN);

	if (IS_ENABLED(CONFIG_LINEAR_FRAMEBUFFER)) {
		write32(mmio + VGACNTRL, VGA_DISP_DISABLE);
		write32(mmio + DSPCNTR(0), DISPLAY_PLANE_ENABLE
			| DISPPLANE_BGRX888);
		mdelay(1);
	} else {
		write32(mmio + VGACNTRL, 0xc4008e);
	}

	write32(mmio + ADPA, ADPA_DAC_ENABLE
			| ADPA_PIPE_A_SELECT
			| ADPA_CRT_HOTPLUG_MONITOR_COLOR
			| ADPA_CRT_HOTPLUG_ENABLE
			| ADPA_VSYNC_CNTL_ENABLE
			| ADPA_HSYNC_CNTL_ENABLE
			| ADPA_DPMS_ON
			| (vpolarity ? ADPA_VSYNC_ACTIVE_LOW :
			   ADPA_VSYNC_ACTIVE_HIGH)
			| (hpolarity ? ADPA_HSYNC_ACTIVE_LOW :
			   ADPA_HSYNC_ACTIVE_HIGH));

	write32(mmio + PP_CONTROL, PANEL_POWER_ON | PANEL_POWER_RESET);

	/* Enable screen memory. */
	vga_sr_write(1, vga_sr_read(1) & ~0x20);

	/* Clear interrupts. */
	write32(mmio + DEIIR, 0xffffffff);
	write32(mmio + SDEIIR, 0xffffffff);

	if (IS_ENABLED(CONFIG_LINEAR_FRAMEBUFFER)) {
		memset((void *) lfb, 0,
			edid->x_resolution * edid->y_resolution * 4);
		set_vbe_mode_info_valid(edid, lfb);
	}


}

static void gma_ngi(struct device *const dev, struct edid *edid_lvds)
{
	u8 edid_data_vga[128];
	struct edid edid_vga;
	int vga_edid_status;
	u8 *mmio;
	struct northbridge_intel_gm45_config *conf = dev->chip_info;

	mmio = res2mmio(gtt_res, 0, 0);
	printk(BIOS_DEBUG, "VGA EDID\n");
	intel_gmbus_read_edid(mmio + GMBUS0, GMBUS_PORT_VGADDC, 0x50,
			edid_data_vga, sizeof(edid_data_vga));
	intel_gmbus_stop(mmio + GMBUS0);
	vga_edid_status = decode_edid(edid_data_vga,
				sizeof(edid_data_vga), &edid_vga);

	u32 physbase;
	struct resource *lfb_res;
	struct resource *pio_res;

	lfb_res = find_resource(dev, PCI_BASE_ADDRESS_2);
	pio_res = find_resource(dev, PCI_BASE_ADDRESS_4);

	physbase = pci_read_config32(dev, 0x5c) & ~0xf;

	if (!(physbase && pio_res && pio_res->base && lfb_res && lfb_res->base))
		return;

	printk(BIOS_SPEW, "Initializing display without OPROM. MMIO 0x%llx\n",
		gtt_res->base);
	if (vga_edid_status != EDID_ABSENT) {
		printk(BIOS_DEBUG, "Initialising display on VGA output\n");
		gma_init_vga(conf, mmio, physbase, pio_res->base, lfb_res->base,
			&edid_vga);
	} else {
		printk(BIOS_DEBUG, "Initialising display on LVDS output\n");
		gma_init_lvds(conf, mmio, physbase, pio_res->base,
			lfb_res->base, edid_lvds);
	}

	/* Linux relies on VBT for panel info.  */
	generate_fake_intel_oprom(&conf->gfx, dev, "$VBT CANTIGA");
}

static u32 get_cdclk(struct device *const dev)
{
	const u16 cdclk_sel =
		pci_read_config16 (dev, GCFGC_OFFSET) & GCFGC_CD_MASK;
	switch (MCHBAR8(HPLLVCO_MCHBAR) & 0x7) {
	case VCO_2666:
	case VCO_4000:
	case VCO_5333:
		return cdclk_sel ? 333333333 : 222222222;
	case VCO_3200:
		return cdclk_sel ? 320000000 : 228571429;
	default:
		printk(BIOS_WARNING,
		       "Unknown VCO frequency, using default cdclk.\n");
		return 222222222;
	}
}

static u32 freq_to_blc_pwm_ctl(struct device *const dev,
			u16 pwm_freq, u8 duty_perc)
{
	u32 blc_mod;

	blc_mod = get_cdclk(dev) / (128 * pwm_freq);

	if (duty_perc <= 100)
		return (blc_mod << 16) | (blc_mod * duty_perc / 100);
	else
		return (blc_mod << 16) | blc_mod;
}

static void gma_pm_init_post_vbios(struct device *const dev,
				const char *edid_ascii_string)
{
	const struct northbridge_intel_gm45_config *const conf = dev->chip_info;

	u32 reg32;
	u8 reg8;
	const struct blc_pwm_t *blc_pwm;
	int blc_array_len, i;
	u16 pwm_freq = 0;

	/* Setup Panel Power On Delays */
	reg32 = gtt_read(PP_ON_DELAYS);
	if (!reg32) {
		reg32 = (conf->gpu_panel_power_up_delay & 0x1fff) << 16;
		reg32 |= (conf->gpu_panel_power_backlight_on_delay & 0x1fff);
		gtt_write(PP_ON_DELAYS, reg32);
	}

	/* Setup Panel Power Off Delays */
	reg32 = gtt_read(PP_OFF_DELAYS);
	if (!reg32) {
		reg32 = (conf->gpu_panel_power_down_delay & 0x1fff) << 16;
		reg32 |= (conf->gpu_panel_power_backlight_off_delay & 0x1fff);
		gtt_write(PP_OFF_DELAYS, reg32);
	}

	/* Setup Panel Power Cycle Delay */
	if (conf->gpu_panel_power_cycle_delay) {
		reg32 = (get_cdclk(dev) / 20000 - 1)
			<< PP_REFERENCE_DIVIDER_SHIFT;
		reg32 |= conf->gpu_panel_power_cycle_delay & 0x1f;
		gtt_write(PP_DIVISOR, reg32);
	}

	/* Enable Backlight  */
	gtt_write(BLC_PWM_CTL2, (1 << 31));
	reg8 = 100;
	if (conf->duty_cycle != 0)
		reg8 = conf->duty_cycle;
	blc_array_len = get_blc_values(&blc_pwm);
	if (conf->default_pwm_freq != 0)
		pwm_freq = conf->default_pwm_freq;

	/* Find EDID string and pwm freq in lookup table */
	for (i = 0; i < blc_array_len; i++) {
		if (!strncmp(blc_pwm[i].ascii_string, edid_ascii_string,
				strlen(blc_pwm[i].ascii_string))) {
			pwm_freq = blc_pwm[i].pwm_freq;
			printk(BIOS_DEBUG, "Found EDID string: %s in lookup table, pwm: %dHz\n",
				blc_pwm[i].ascii_string, pwm_freq);
			break;
		}
	}

	if (i == blc_array_len)
		printk(BIOS_NOTICE, "Your panels EDID `%s` wasn't found in the"
			"lookup table.\n You may have issues with your panels"
			"backlight.\n If you want to help improving coreboot"
			"please report: this EDID string\n and the result"
			"of `intel_read read BLC_PWM_CTL`"
			"(from intel-gpu-tools)\n while running vendor BIOS\n",
			edid_ascii_string);

	if (pwm_freq == 0)
		gtt_write(BLC_PWM_CTL, 0x06100610);
	else
		gtt_write(BLC_PWM_CTL, freq_to_blc_pwm_ctl(dev, pwm_freq,
								reg8));
}

static void gma_func0_init(struct device *dev)
{
	u32 reg32;
	u8 *mmio;
	u8 edid_data_lvds[128];
	struct edid edid_lvds;

	/* IGD needs to be Bus Master */
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	reg32 |= PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO;
	pci_write_config32(dev, PCI_COMMAND, reg32);

	gtt_res = find_resource(dev, PCI_BASE_ADDRESS_0);
	if (gtt_res == NULL)
		return;
	mmio = res2mmio(gtt_res, 0, 0);

	if (!IS_ENABLED(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT)) {
		/* PCI Init, will run VBIOS */
		printk(BIOS_DEBUG, "Initialising IGD using VBIOS\n");
		pci_dev_init(dev);
	}

	printk(BIOS_DEBUG, "LVDS EDID\n");
	intel_gmbus_read_edid(mmio + GMBUS0, GMBUS_PORT_PANEL, 0x50,
			edid_data_lvds, sizeof(edid_data_lvds));
	intel_gmbus_stop(mmio + GMBUS0);
	decode_edid(edid_data_lvds, sizeof(edid_data_lvds), &edid_lvds);

	/* Post VBIOS init */
	gma_pm_init_post_vbios(dev, edid_lvds.ascii_string);

	if (IS_ENABLED(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT))
		gma_ngi(dev, &edid_lvds);

	intel_gma_restore_opregion();
}

static void gma_set_subsystem(struct device *dev, unsigned vendor, unsigned device)
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

const struct i915_gpu_controller_info *
intel_gma_get_controller_info(void)
{
	struct device *dev = dev_find_slot(0, PCI_DEVFN(0x2,0));
	if (!dev) {
		return NULL;
	}
	struct northbridge_intel_gm45_config *chip = dev->chip_info;
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

static const char *gma_acpi_name(const struct device *dev)
{
	return "GFX0";
}

static struct pci_operations gma_pci_ops = {
	.set_subsystem = gma_set_subsystem,
};

static struct device_operations gma_func0_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.acpi_fill_ssdt_generator = gma_ssdt,
	.init = gma_func0_init,
	.scan_bus = 0,
	.enable = 0,
	.ops_pci = &gma_pci_ops,
	.acpi_name = gma_acpi_name,
	.write_acpi_tables = gma_write_acpi_tables,
};

static const unsigned short pci_device_ids[] =
{
	0x2a42, 0
};

static const struct pci_driver gma __pci_driver = {
	.ops = &gma_func0_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
