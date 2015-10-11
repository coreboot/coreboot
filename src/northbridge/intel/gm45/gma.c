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
 * Foundation, Inc.
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
#include <kconfig.h>

#include "drivers/intel/gma/i915_reg.h"
#include "chip.h"
#include "gm45.h"
#include <drivers/intel/gma/intel_bios.h>
#include <drivers/intel/gma/edid.h>
#include <drivers/intel/gma/i915.h>
#include <pc80/vga.h>
#include <pc80/vga_io.h>

static struct resource *gtt_res = NULL;

void gtt_write(u32 reg, u32 data)
{
	write32(res2mmio(gtt_res, reg, 0), data);
}

#if IS_ENABLED(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT)

static void power_port(u8 *mmio)
{
	read32(mmio + 0x00061100); // = 0x00000000
	write32(mmio + 0x00061100, 0x00000000);
	write32(mmio + 0x00061100, 0x00010000);
	read32(mmio + 0x00061100); // = 0x00010000
	read32(mmio + 0x00061100); // = 0x00010000
	read32(mmio + 0x00061100); // = 0x00000000
	write32(mmio + 0x00061100, 0x00000000);
	read32(mmio + 0x00061100); // = 0x00000000
	read32(mmio + 0x00064200); // = 0x0000001c
	write32(mmio + 0x00064210, 0x8004003e);
	write32(mmio + 0x00064214, 0x80060002);
	write32(mmio + 0x00064218, 0x01000000);
	read32(mmio + 0x00064210); // = 0x5144003e
	write32(mmio + 0x00064210, 0x5344003e);
	read32(mmio + 0x00064210); // = 0x0144003e
	write32(mmio + 0x00064210, 0x8074003e);
	read32(mmio + 0x00064210); // = 0x5144003e
	read32(mmio + 0x00064210); // = 0x5144003e
	write32(mmio + 0x00064210, 0x5344003e);
	read32(mmio + 0x00064210); // = 0x0144003e
	write32(mmio + 0x00064210, 0x8074003e);
	read32(mmio + 0x00064210); // = 0x5144003e
	read32(mmio + 0x00064210); // = 0x5144003e
	write32(mmio + 0x00064210, 0x5344003e);
	read32(mmio + 0x00064210); // = 0x0144003e
	write32(mmio + 0x00064210, 0x8074003e);
	read32(mmio + 0x00064210); // = 0x5144003e
	read32(mmio + 0x00064210); // = 0x5144003e
	write32(mmio + 0x00064210, 0x5344003e);
	write32(mmio + 0x00064f00, 0x0100030c);
	write32(mmio + 0x00064f04, 0x00b8230c);
	write32(mmio + 0x00064f08, 0x06f8930c);
	write32(mmio + 0x00064f0c, 0x09f8e38e);
	write32(mmio + 0x00064f10, 0x00b8030c);
	write32(mmio + 0x00064f14, 0x0b78830c);
	write32(mmio + 0x00064f18, 0x0ff8d3cf);
	write32(mmio + 0x00064f1c, 0x01e8030c);
	write32(mmio + 0x00064f20, 0x0ff863cf);
	write32(mmio + 0x00064f24, 0x0ff803cf);
	write32(mmio + 0x000c4030, 0x00001000);
	read32(mmio + 0x00044000); // = 0x00000000
	write32(mmio + 0x00044030, 0x00001000);
	read32(mmio + 0x00061150); // = 0x0000001c
	write32(mmio + 0x00061150, 0x0000089c);
	write32(mmio + 0x000fcc00, 0x01986f00);
	write32(mmio + 0x000fcc0c, 0x01986f00);
	write32(mmio + 0x000fcc18, 0x01986f00);
	write32(mmio + 0x000fcc24, 0x01986f00);
	read32(mmio + 0x00044000); // = 0x00000000
	read32(mmio + LVDS); // = 0x40000002
}

static void intel_gma_init(const struct northbridge_intel_gm45_config *info,
			   u8 *mmio, u32 physbase, u16 piobase, u32 lfb)
{

	int i;
	u8 edid_data[128];
	struct edid edid;
	struct edid_mode *mode;
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
	u32 link_frequency = info->gfx.link_frequency_270_mhz ? 270000 : 162000;
	u32 data_m1;
	u32 data_n1 = 0x00800000;
	u32 link_m1;
	u32 link_n1 = 0x00080000;

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

	power_port(mmio);

	intel_gmbus_read_edid(mmio + GMBUS0, 3, 0x50, edid_data, 128);
	decode_edid(edid_data,
		    sizeof(edid_data), &edid);
	mode = &edid.mode;

	/* Disable screen memory to prevent garbage from appearing.  */
	vga_sr_write(1, vga_sr_read(1) | 0x20);

	hactive = edid.x_resolution;
	vactive = edid.y_resolution;
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

	target_frequency = mode->lvds_dual_channel ? mode->pixel_clock
		: (2 * mode->pixel_clock);
#if IS_ENABLED(CONFIG_FRAMEBUFFER_KEEP_VESA_MODE)
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
#else
	vga_textmode_init();
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

	link_m1 = ((uint64_t)link_n1 * mode->pixel_clock) / link_frequency;
	data_m1 = ((uint64_t)data_n1 * 18 * mode->pixel_clock)
		/ (link_frequency * 8 * 4);

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

	write32(mmio + LVDS,
		(hpolarity << 20) | (vpolarity << 21)
		| (mode->lvds_dual_channel ? LVDS_CLOCK_B_POWERUP_ALL
		   | LVDS_CLOCK_BOTH_POWERUP_ALL : 0)
		| LVDS_BORDER_ENABLE | LVDS_CLOCK_A_POWERUP_ALL);
	mdelay(1);
	write32(mmio + PP_CONTROL, PANEL_UNLOCK_REGS
		| (read32(mmio + PP_CONTROL) & ~PANEL_UNLOCK_MASK));
	write32(mmio + FP0(0),
		((pixel_n - 2) << 16)
		| ((pixel_m1 - 2) << 8) | pixel_m2);
	write32(mmio + DPLL(0),
		DPLL_VCO_ENABLE | DPLLB_MODE_LVDS
		| (mode->lvds_dual_channel ? DPLLB_LVDS_P2_CLOCK_DIV_7
		   : DPLLB_LVDS_P2_CLOCK_DIV_14)
		| (0x10000 << (pixel_p1 - 1))
		| ((info->gfx.use_spread_spectrum_clock ? 3 : 0) << 13)
		| (0x1 << (pixel_p1 - 1)));
	mdelay(1);
	write32(mmio + DPLL(0),
		DPLL_VCO_ENABLE | DPLLB_MODE_LVDS
		| (mode->lvds_dual_channel ? DPLLB_LVDS_P2_CLOCK_DIV_7
		   : DPLLB_LVDS_P2_CLOCK_DIV_14)
		| (0x10000 << (pixel_p1 - 1))
		| ((info->gfx.use_spread_spectrum_clock ? 3 : 0) << 13)
		| (0x1 << (pixel_p1 - 1)));
	/* Re-lock the registers.  */
	write32(mmio + PP_CONTROL,
		(read32(mmio + PP_CONTROL) & ~PANEL_UNLOCK_MASK));

	write32(mmio + LVDS,
		(hpolarity << 20) | (vpolarity << 21)
		| (mode->lvds_dual_channel ? LVDS_CLOCK_B_POWERUP_ALL
		   | LVDS_CLOCK_BOTH_POWERUP_ALL : 0)
		| LVDS_BORDER_ENABLE | LVDS_CLOCK_A_POWERUP_ALL);

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
	write32(mmio + PFIT_CONTROL, 0x20000000);
#else
	write32(mmio + PIPESRC(0), (639 << 16) | 399);
	write32(mmio + PF_CTL(0),PF_ENABLE | PF_FILTER_MED_3x3);
	write32(mmio + PF_WIN_SZ(0), vactive | (hactive << 16));
	write32(mmio + PFIT_CONTROL, 0xa0000000);
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
	write32(mmio + VGACNTRL, 0x22c4008e | VGA_DISP_DISABLE);
	write32(mmio + DSPCNTR(0), DISPLAY_PLANE_ENABLE | DISPPLANE_BGRX888);
	mdelay(1);
#else
	write32(mmio + VGACNTRL, 0x22c4008e);
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
		);
	write32(mmio + LVDS,
		LVDS_PORT_ENABLE
		| (hpolarity << 20) | (vpolarity << 21)
		| (mode->lvds_dual_channel ? LVDS_CLOCK_B_POWERUP_ALL
		   | LVDS_CLOCK_BOTH_POWERUP_ALL : 0)
		| LVDS_BORDER_ENABLE | LVDS_CLOCK_A_POWERUP_ALL);

	write32(mmio + PP_CONTROL, PANEL_UNLOCK_REGS | PANEL_POWER_OFF);
	write32(mmio + PP_CONTROL, PANEL_UNLOCK_REGS | PANEL_POWER_RESET);
	mdelay(1);
	write32(mmio + PP_CONTROL, PANEL_UNLOCK_REGS
		| PANEL_POWER_ON | PANEL_POWER_RESET);

	printk (BIOS_DEBUG, "waiting for panel powerup\n");
	while (1) {
		u32 reg32;
		reg32 = read32(mmio + PP_STATUS);
		if (((reg32 >> 28) & 3) == 0)
			break;
	}
	printk (BIOS_DEBUG, "panel powered up\n");

	write32(mmio + PP_CONTROL, PANEL_POWER_ON | PANEL_POWER_RESET);

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
	gtt_res = find_resource(dev, PCI_BASE_ADDRESS_0);

	struct northbridge_intel_gm45_config *conf = dev->chip_info;

#if !CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT
	/* PCI Init, will run VBIOS */
	pci_dev_init(dev);
#else
	u32 physbase;
	struct resource *lfb_res;
	struct resource *pio_res;

	lfb_res = find_resource(dev, PCI_BASE_ADDRESS_2);
	pio_res = find_resource(dev, PCI_BASE_ADDRESS_4);

	physbase = pci_read_config32(dev, 0x5c) & ~0xf;

	if (gtt_res && gtt_res->base && physbase && pio_res && pio_res->base
	    && lfb_res && lfb_res->base) {
		printk(BIOS_SPEW, "Initializing VGA without OPROM. MMIO 0x%llx\n",
		       gtt_res->base);
		intel_gma_init(conf, res2mmio(gtt_res, 0, 0), physbase,
			       pio_res->base, lfb_res->base);
	}

	/* Linux relies on VBT for panel info.  */
	generate_fake_intel_oprom(&conf->gfx, dev, "$VBT IRONLAKE-MOBILE");
#endif

	/* Post VBIOS init */
	/* Enable Backlight  */
	gtt_write(BLC_PWM_CTL2, (1 << 31));
	if (conf->gfx.backlight == 0)
		gtt_write(BLC_PWM_CTL, 0x06100610);
	else
		gtt_write(BLC_PWM_CTL, conf->gfx.backlight);
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

const struct i915_gpu_controller_info *
intel_gma_get_controller_info(void)
{
	device_t dev = dev_find_slot(0, PCI_DEVFN(0x2,0));
	if (!dev) {
		return NULL;
	}
	struct northbridge_intel_gm45_config *chip = dev->chip_info;
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
