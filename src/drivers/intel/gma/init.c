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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>

#include "edid.h"
#include "i915.h"
#include "init.h"
#include <pc80/vga.h>
#include <pc80/vga_io.h>

#if IS_ENABLED(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT)

void intel_gma_init(const struct intel_gma_info *info,
		    u32 mmio, u32 physbase, u32 gttbase)
{
	int i;
	u8 edid_data[128];
	struct edid edid;

	write32(mmio + 0x00070080, 0x00000000);
	write32(mmio + 0x00070180, 0x00000000);
	write32(mmio + 0x00071180, 0x00000000);
	write32(mmio + 0x00041000, 0x8000298e);
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
	for (i = 0; i < 0x1fff; i++)
		write32(gttbase + (i << 2), physbase | (i << 12) | 1);

	vga_misc_write(0x67);

	u8 cr[] = { 0x5f, 0x4f, 0x50, 0x82, 0x55, 0x81, 0xbf, 0x1f,
		    0x00, 0x4f, 0x0d, 0x0e, 0x00, 0x00, 0x00, 0x00,
		    0x9c, 0x8e, 0x8f, 0x28, 0x1f, 0x96, 0xb9, 0xa3,
		    0xff
	};
	vga_cr_write(0x11, 0);

	for (i = 0; i <= 0x18; i++)
		vga_cr_write(i, cr[i]);

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

	intel_gmbus_read_edid(mmio, 3, 0x50, edid_data);
	decode_edid(edid_data,
		    sizeof(edid_data), &edid);

	/* Disable screen memory to prevent garbage from appearing.  */
	vga_sr_write(1, vga_sr_read(1) | 0x20);

	vga_textmode_init();

	unsigned hactive = edid.x_resolution;
	unsigned vactive = edid.y_resolution;
	unsigned right_border = edid.hborder;
	unsigned bottom_border = edid.vborder;
	int hpolarity = (edid.phsync == '-');
	int vpolarity = (edid.pvsync == '-');
	unsigned vsync = edid.vspw;
	unsigned hsync = edid.hspw;
	unsigned vblank = edid.vbl;
	unsigned hblank = edid.hbl;
	unsigned hfront_porch = edid.hso;
	unsigned vfront_porch = edid.vso;

	unsigned candp1, candn;
	unsigned best_delta = 0xffffffff;

	unsigned target_frequency = info->lvds_dual_channel ? edid.pixel_clock
		: (2 * edid.pixel_clock);
	unsigned pixel_p1 = 1;
	unsigned pixel_n = 1;
	unsigned pixel_m1 = 1;
	unsigned pixel_m2 = 1;

	/* Find suitable divisors.  */
	for (candp1 = 1; candp1 <= 8; candp1++)
		for (candn = 5; candn <= 10; candn++) {
			unsigned cur_frequency;
			unsigned m; /* 77 - 131.  */
			unsigned denom; /* 35 - 560.  */
			unsigned current_delta;

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

	if (best_delta == 0xffffffff) {
		printk (BIOS_ERR, "Couldn't find GFX clock divisors\n");
		return;
	}

	unsigned link_frequency = info->link_frequency_270_mhz ? 270000 : 162000;
	unsigned data_m1;
	unsigned data_n1 = 0x00800000;
	unsigned link_m1;
	unsigned link_n1 = 0x00080000;

	link_m1 = ((uint64_t)link_n1 * edid.pixel_clock) / link_frequency;
	data_m1 = ((uint64_t)data_n1 * 4 * edid.pixel_clock)
		/ (link_frequency * 7);

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
	       info->lvds_dual_channel ? "Dual channel\n" : "Single channel\n");
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
		| (info->lvds_dual_channel ? LVDS_CLOCK_B_POWERUP_ALL
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
		| (info->lvds_dual_channel ? DPLLB_LVDS_P2_CLOCK_DIV_7
		   : DPLLB_LVDS_P2_CLOCK_DIV_14)
		| (0x10000 << (pixel_p1 - 1))
		| ((info->use_spread_spectrum_clock ? 3 : 0) << 13)
		| (0x1 << (pixel_p1 - 1)));
	mdelay(1);
	write32(mmio + _PCH_DPLL(0),
		DPLL_VCO_ENABLE | DPLLB_MODE_LVDS
		| (info->lvds_dual_channel ? DPLLB_LVDS_P2_CLOCK_DIV_7
		   : DPLLB_LVDS_P2_CLOCK_DIV_14)
		| (0x10000 << (pixel_p1 - 1))
		| ((info->use_spread_spectrum_clock ? 3 : 0) << 13)
		| (0x1 << (pixel_p1 - 1)));
	/* Re-lock the registers.  */
	write32(mmio + PCH_PP_CONTROL,
		(read32(mmio + PCH_PP_CONTROL) & ~PANEL_UNLOCK_MASK));

	write32(mmio + PCH_LVDS,
		(hpolarity << 20) | (vpolarity << 21)
		| (info->lvds_dual_channel ? LVDS_CLOCK_B_POWERUP_ALL
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

	write32(mmio + PIPESRC(0), 0x02cf018f);
	write32(mmio + PIPECONF(0), PIPECONF_DISABLE);

	write32(mmio + 0x6001c,0x27f018f); // 02cf018f
	write32(mmio + 0x68080,0x80800000); // 00000000

	write32(mmio + PF_WIN_SZ(0), vactive | (hactive << 16));
	read32(mmio + 0x00070008);
	read32(mmio + 0x00070008);

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
	write32(mmio + 0x00070008, 0x00000040);
	write32(mmio + 0x000f0008, 0x00000040);
	write32(mmio + 0x000f000c, 0x00022050);
	write32(mmio + 0x00070008, 0x00000050);
	write32(mmio + 0x00070008, 0x80000050);
	write32(mmio + 0x00041000, 0x0020298e);


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

	write32(mmio + TRANS_HTOTAL(0),
		((hactive + right_border + hblank - 1) << 16)
		| (hactive - 1));
	write32(mmio + TRANS_HBLANK(0),
		((hactive + right_border + hblank - 1) << 16)
		| (hactive + right_border - 1));

	printk(BIOS_ERR, "computed: %x, %x, %x\n",
	       ((hactive + right_border + hfront_porch + hsync - 1) << 16)
	       | (hactive + right_border + hfront_porch - 1),
	       ((vactive + bottom_border + vblank - 1) << 16)
	       | (vactive - 1),
	       (vactive + bottom_border + vfront_porch + vsync - 1)
	       | (vactive + bottom_border + vfront_porch - 1));

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
	write32(mmio + 0x000f0008, 0x80000040);
	write32(mmio + PCH_LVDS,
		LVDS_PORT_ENABLE
		| (hpolarity << 20) | (vpolarity << 21)
		| (info->lvds_dual_channel ? LVDS_CLOCK_B_POWERUP_ALL
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
}

#endif
