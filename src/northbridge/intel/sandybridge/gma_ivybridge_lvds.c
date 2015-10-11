/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013, 2014 Vladimir Serbinenko
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 or (at your option)
 *  any later version of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/io.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <string.h>

#include <drivers/intel/gma/edid.h>
#include <drivers/intel/gma/i915.h>
#include <drivers/intel/gma/intel_bios.h>
#include "gma.h"
#include "chip.h"
#include <pc80/vga.h>
#include <pc80/vga_io.h>
#include <device/pci_def.h>
#include <device/pci_rom.h>

static void link_train(u8 *mmio)
{
	write32(mmio+0xf000c,0x40);
	write32(mmio+0x60100,0x40000);
	write32(mmio+0xf0018,0x8ff);
	write32(mmio+0xf1018,0x8ff);
	write32(mmio+0xf000c,0x2040);
	mdelay(1);
	write32(mmio+0xf000c,0x2050);
	write32(mmio+0x60100,0x44000);
	mdelay(1);
	write32(mmio+0x70008,0x40);
	write32(mmio+0xe0300,0x60000418);
	write32(mmio+0xf000c,0x22050);
	write32(mmio+0x70008,0x50);
	write32(mmio+0x70008,0x80000050);
}

static void link_normal_operation(u8 *mmio)
{
	write32(mmio + FDI_TX_CTL(0), 0x80044f02);
	write32(mmio + FDI_RX_CTL(0),
		FDI_RX_ENABLE | FDI_6BPC
		| 0x2f50);
}

static void enable_port(u8 *mmio)
{
	write32(mmio + 0xec008, 0x2c010000);
	write32(mmio + 0xec020, 0x2c010000);
	write32(mmio + 0xec038, 0x2c010000);
	write32(mmio + 0xec050, 0x2c010000);
	write32(mmio + 0xec408, 0x2c010000);
	write32(mmio + 0xec420, 0x2c010000);
	write32(mmio + 0xec438, 0x2c010000);
	write32(mmio + 0xec450, 0x2c010000);
	write32(mmio + 0xf0010, 0x200090);
	write32(mmio + 0xf1010, 0x200090);
	write32(mmio + 0xf2010, 0x200090);
	write32(mmio + 0xfd034, 0x8420000);
	write32(mmio + 0x45010, 0x3);
	write32(mmio + 0xf0060, 0x10);
	write32(mmio + 0xf1060, 0x10);
	write32(mmio + 0xf2060, 0x10);
	write32(mmio + 0x9840, 0x0);
	write32(mmio + 0x42000, 0xa0000000);
	write32(mmio + 0xe4f00, 0x100030c);
	write32(mmio + 0xe4f04, 0xb8230c);
	write32(mmio + 0xe4f08, 0x6f8930c);
	write32(mmio + 0xe4f0c, 0x5f8e38e);
	write32(mmio + 0xe4f10, 0xb8030c);
	write32(mmio + 0xe4f14, 0xb78830c);
	write32(mmio + 0xe4f18, 0x9f8d3cf);
	write32(mmio + 0xe4f1c, 0x1e8030c);
	write32(mmio + 0xe4f20, 0x9f863cf);
	write32(mmio + 0xe4f24, 0xff803cf);
	read32(mmio + 0xe4100);
	write32(mmio + 0xc4030, 0x10);
	write32(mmio + 0xe4110, 0x8004003e);
	write32(mmio + 0xe4114, 0x80060000);
	write32(mmio + 0xe4118, 0x1000000);
	read32(mmio + 0xe4110);
	write32(mmio + 0xe4110, 0x5344003e);
	read32(mmio + 0xe4110);
	write32(mmio + 0xe4110, 0x8054003e);
	read32(mmio + 0xe4110);
	read32(mmio + 0xe4110);
	write32(mmio + 0xe4110, 0x5344003e);
	mdelay(1);
	read32(mmio + 0xe4110);
	write32(mmio + 0xe4110, 0x8054003e);
	read32(mmio + 0xe4110);
	read32(mmio + 0xe4110);
	write32(mmio + 0xe4110, 0x5344003e);
	mdelay(1);
	read32(mmio + 0xe4110);
	write32(mmio + 0xe4110, 0x8054003e);
	read32(mmio + 0xe4110);
	read32(mmio + 0xe4110);
	write32(mmio + 0xe4110, 0x5344003e);
	mdelay(1);
	read32(mmio + 0xc4000);
	read32(mmio + 0xe1140);
	write32(mmio + 0xc4030, 0x10);
	read32(mmio + 0xc4000);
	write32(mmio + 0xe4f00, 0x100030c);
	write32(mmio + 0xe4f04, 0xb8230c);
	write32(mmio + 0xe4f08, 0x6f8930c);
	write32(mmio + 0xe4f0c, 0x5f8e38e);
	write32(mmio + 0xe4f10, 0xb8030c);
	write32(mmio + 0xe4f14, 0xb78830c);
	write32(mmio + 0xe4f18, 0x9f8d3cf);
	write32(mmio + 0xe4f1c, 0x1e8030c);
	write32(mmio + 0xe4f20, 0x9f863cf);
	write32(mmio + 0xe4f24, 0xff803cf);
	read32(mmio + 0xe4200);
	write32(mmio + 0xc4030, 0x1010);
	write32(mmio + 0xe4210, 0x8004003e);
	write32(mmio + 0xe4214, 0x80060000);
	write32(mmio + 0xe4218, 0x1002000);
	read32(mmio + 0xe4210);
	write32(mmio + 0xe4210, 0x5344003e);
	read32(mmio + 0xe4210);
	write32(mmio + 0xe4210, 0x8054003e);
	read32(mmio + 0xe4210);
	read32(mmio + 0xe4210);
	write32(mmio + 0xe4210, 0x5344003e);
	mdelay(1);
	read32(mmio + 0xe4210);
	write32(mmio + 0xe4210, 0x8054003e);
	read32(mmio + 0xe4210);
	read32(mmio + 0xe4210);
	write32(mmio + 0xe4210, 0x5344003e);
	mdelay(1);
	read32(mmio + 0xe4210);
	write32(mmio + 0xe4210, 0x8054003e);
	read32(mmio + 0xe4210);
	read32(mmio + 0xe4210);
	write32(mmio + 0xe4210, 0x5344003e);
	mdelay(1);
	read32(mmio + 0xc4000);
}

int i915lightup_sandy(const struct i915_gpu_controller_info *info,
		      u32 physbase, u16 piobase, u8 *mmio, u32 lfb)
{
	int i;
	u8 edid_data[128];
	struct edid edid;

	if (!IS_ENABLED(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT))
		return 0;

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

	u8 cr[] = { 0x5f, 0x4f, 0x50, 0x82, 0x55, 0x81, 0xbf, 0x1f,
		    0x00, 0x4f, 0x0d, 0x0e, 0x00, 0x00, 0x00, 0x00,
		    0x9c, 0x8e, 0x8f, 0x28, 0x1f, 0x96, 0xb9, 0xa3,
		    0xff
	};
	vga_cr_write(0x11, 0);

	for (i = 0; i <= 0x18; i++)
		vga_cr_write(i, cr[i]);

	enable_port(mmio);

	intel_gmbus_read_edid(mmio + PCH_GMBUS0, 3, 0x50, edid_data, 128);
	intel_gmbus_stop(mmio + PCH_GMBUS0);
	decode_edid(edid_data,
		    sizeof(edid_data), &edid);


	/* Disable screen memory to prevent garbage from appearing.  */
	vga_sr_write(1, vga_sr_read(1) | 0x20);

	u32 hactive = edid.x_resolution;
	u32 vactive = edid.y_resolution;
	u32 right_border = edid.mode.hborder;
	u32 bottom_border = edid.mode.vborder;
	int hpolarity = (edid.mode.phsync == '-');
	int vpolarity = (edid.mode.pvsync == '-');
	u32 vsync = edid.mode.vspw;
	u32 hsync = edid.mode.hspw;
	u32 vblank = edid.mode.vbl;
	u32 hblank = edid.mode.hbl;
	u32 hfront_porch = edid.mode.hso;
	u32 vfront_porch = edid.mode.vso;

	u32 candp1, candn;
	u32 best_delta = 0xffffffff;

	u32 target_frequency = (
		edid.mode.lvds_dual_channel ? edid.mode.pixel_clock
		: (2 * edid.mode.pixel_clock));
	u32 pixel_p1 = 1;
	u32 pixel_n = 1;
	u32 pixel_m1 = 1;
	u32 pixel_m2 = 1;

	vga_textmode_init();
	if (IS_ENABLED(CONFIG_FRAMEBUFFER_KEEP_VESA_MODE)) {
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
	}

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
		return 0;
	}

	u32 link_frequency = info->link_frequency_270_mhz ? 270000 : 162000;
	u32 data_m1;
	u32 data_n1 = 0x00800000;
	u32 link_m1;
	u32 link_n1 = 0x00080000;

	link_m1 = ((uint64_t)link_n1 * edid.mode.pixel_clock) / link_frequency;

	data_m1 = ((uint64_t)data_n1 * 18 * edid.mode.pixel_clock)
		/ (link_frequency * 8);

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
	printk(BIOS_DEBUG, (info->use_spread_spectrum_clock
			    ? "Spread spectrum clock\n" : "DREF clock\n"));
	printk(BIOS_DEBUG,
	       edid.mode.lvds_dual_channel ? "Dual channel\n" : "Single channel\n");
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
		| (edid.mode.lvds_dual_channel ? LVDS_CLOCK_B_POWERUP_ALL
		   | LVDS_CLOCK_BOTH_POWERUP_ALL : 0)
		| LVDS_BORDER_ENABLE | LVDS_CLOCK_A_POWERUP_ALL
		| LVDS_DETECTED);
	write32(mmio + BLC_PWM_CPU_CTL2, (1 << 31));
	write32(mmio + PCH_DREF_CONTROL, (info->use_spread_spectrum_clock
					  ? 0x1002 : 0x400));
	mdelay(1);
	write32(mmio + PCH_PP_CONTROL, PANEL_UNLOCK_REGS
		| (read32(mmio + PCH_PP_CONTROL) & ~PANEL_UNLOCK_MASK));
	write32(mmio + _PCH_FP0(0),
		((pixel_n - 2) << 16)
		| ((pixel_m1 - 2) << 8) | pixel_m2);
	write32(mmio + _PCH_DPLL(0),
		DPLL_VCO_ENABLE | DPLLB_MODE_LVDS
		| (edid.mode.lvds_dual_channel ? DPLLB_LVDS_P2_CLOCK_DIV_7
		   : DPLLB_LVDS_P2_CLOCK_DIV_14)
		| (0x10000 << (pixel_p1 - 1))
		| ((info->use_spread_spectrum_clock ? 3 : 0) << 13)
		| (0x1 << (pixel_p1 - 1)));

	mdelay(1);
	write32(mmio + 0xc7000, 0x8);

	mdelay(1);
	write32(mmio + _PCH_DPLL(0),
		DPLL_VCO_ENABLE | DPLLB_MODE_LVDS
		| (edid.mode.lvds_dual_channel ? DPLLB_LVDS_P2_CLOCK_DIV_7
		   : DPLLB_LVDS_P2_CLOCK_DIV_14)
		| (0x10000 << (pixel_p1 - 1))
		| ((info->use_spread_spectrum_clock ? 3 : 0) << 13)
		| (0x1 << (pixel_p1 - 1)));
	/* Re-lock the registers.  */
	write32(mmio + PCH_PP_CONTROL,
		(read32(mmio + PCH_PP_CONTROL) & ~PANEL_UNLOCK_MASK));

	write32(mmio + PCH_LVDS,
		(hpolarity << 20) | (vpolarity << 21)
		| (edid.mode.lvds_dual_channel ? LVDS_CLOCK_B_POWERUP_ALL
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

	write32(mmio + 0xf0008, 0);

	if (IS_ENABLED(CONFIG_FRAMEBUFFER_KEEP_VESA_MODE)) {
		write32(mmio + PIPESRC(0), ((hactive - 1) << 16) | (vactive - 1));
		write32(mmio + PF_CTL(0),0);
		write32(mmio + PF_WIN_SZ(0), 0);
		write32(mmio + PF_WIN_POS(0), 0);
	} else {
		write32(mmio + PIPESRC(0), (719 << 16) | 399);
		write32(mmio + PF_WIN_POS(0), 0);
		write32(mmio + PF_CTL(0),PF_ENABLE | PF_FILTER_MED_3x3);
		write32(mmio + PF_WIN_SZ(0), vactive | (hactive << 16));
	}

	mdelay(1);

	write32(mmio + PIPE_DATA_M1(0), 0x7e000000 | data_m1);
	write32(mmio + PIPE_DATA_N1(0), data_n1);
	write32(mmio + PIPE_LINK_M1(0), link_m1);
	write32(mmio + PIPE_LINK_N1(0), link_n1);

	link_train(mmio);

	if (IS_ENABLED(CONFIG_FRAMEBUFFER_KEEP_VESA_MODE))
		write32(mmio+CPU_VGACNTRL,0x298e | VGA_DISP_DISABLE);
	else
		write32(mmio+CPU_VGACNTRL,0x298e);

	write32(mmio+0x60100,0x44300);
	write32(mmio+0x60100,0x80044f00);

	mdelay(1);
	read32(mmio + 0x000f0014); // = 0x00000600

	if (IS_ENABLED(CONFIG_FRAMEBUFFER_KEEP_VESA_MODE)) {
		write32(mmio + DSPCNTR(0), DISPLAY_PLANE_ENABLE
					| DISPPLANE_BGRX888);
		mdelay(1);
	}

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

	link_normal_operation(mmio);

	write32(mmio + PCH_PP_CONTROL, PANEL_UNLOCK_REGS | PANEL_POWER_OFF);

	mdelay(1);

	if (IS_ENABLED(CONFIG_FRAMEBUFFER_KEEP_VESA_MODE))
		write32(mmio + PCH_TRANSCONF(0), TRANS_ENABLE | TRANS_6BPC
						| TRANS_STATE_MASK);
	else
		write32(mmio + PCH_TRANSCONF(0), TRANS_ENABLE | TRANS_6BPC);

	write32(mmio + PCH_LVDS,
		LVDS_PORT_ENABLE
		| (hpolarity << 20) | (vpolarity << 21)
		| (edid.mode.lvds_dual_channel ? LVDS_CLOCK_B_POWERUP_ALL
		   | LVDS_CLOCK_BOTH_POWERUP_ALL : 0)
		| LVDS_BORDER_ENABLE | LVDS_CLOCK_A_POWERUP_ALL
		| LVDS_DETECTED);

	write32(mmio + PCH_PP_CONTROL, PANEL_UNLOCK_REGS | PANEL_POWER_OFF);
	write32(mmio + PCH_PP_CONTROL, PANEL_UNLOCK_REGS | PANEL_POWER_RESET);
	mdelay(1);
	write32(mmio + PCH_PP_CONTROL, PANEL_UNLOCK_REGS
		| PANEL_POWER_ON | PANEL_POWER_RESET);

	printk (BIOS_DEBUG, "waiting for panel powerup\n");
	while (1)
	{
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

	if (IS_ENABLED(CONFIG_FRAMEBUFFER_KEEP_VESA_MODE)) {
		memset ((void *) lfb, 0, edid.x_resolution
					* edid.y_resolution * 4);
		set_vbe_mode_info_valid(&edid, lfb);
	}

	/* Doesn't change any hw behaviour but vga oprom expects it there. */
	write32(mmio + 0x0004f040, 0x01000008);
	write32(mmio + 0x0004f044, 0x00001800);
	write32(mmio + 0x0004f04c, 0x7f7f0000);
	write32(mmio + 0x0004f054, 0x0000422d);
	write32(mmio + 0x0004f05c, 0x00000008);

	/* Linux relies on VBT for panel info.  */
	generate_fake_intel_oprom(info, dev_find_slot(0, PCI_DEVFN(2, 0)),
				  "$VBT SNB/IVB-MOBILE ");

	return 1;
}
