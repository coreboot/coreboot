/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Chromium OS Authors
 * Copyright (C) 2013 Vladimir Serbinenko
 * Copyright (C) 2015 Damien Zammit <damien@zamaudio.com>
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

#include "drivers/intel/gma/i915_reg.h"
#include "chip.h"
#include "x4x.h"
#include <drivers/intel/gma/intel_bios.h>
#include <drivers/intel/gma/edid.h>
#include <drivers/intel/gma/i915.h>
#include <drivers/intel/gma/opregion.h>
#include <drivers/intel/gma/libgfxinit.h>
#include <pc80/vga.h>
#include <pc80/vga_io.h>

#if IS_ENABLED(CONFIG_SOUTHBRIDGE_INTEL_I82801JX)
#include <southbridge/intel/i82801jx/nvs.h>
#elif IS_ENABLED(CONFIG_SOUTHBRIDGE_INTEL_I82801GX)
#include <southbridge/intel/i82801gx/nvs.h>
#endif

#define BASE_FREQUENCY 96000

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

static u8 edid_is_present(u8 *edid, u32 edid_size)
{
	u32 i;
	for (i = 0; i < edid_size; i++) {
		if (*(edid + i) != 0)
			return 1;
	}
	return 0;
}
static void intel_gma_init(const struct northbridge_intel_x4x_config *info,
			u8 *mmio, u32 physbase, u16 piobase, u32 lfb)
{


	int i;
	u8 edid_data[128];
	struct edid edid;
	struct edid_mode *mode;
	u8 edid_is_found;

	/* Initialise mode variables for 640 x 480 @ 60Hz */
	u32 hactive = 640, vactive = 480;
	u32 right_border = 0, bottom_border = 0;
	int hpolarity = 0, vpolarity = 0;
	u32 hsync = 96, vsync = 2;
	u32 hblank = 160, vblank = 45;
	u32 hfront_porch = 16, vfront_porch = 10;
	u32 target_frequency = 25175;

	u32 err_most = 0xffffffff;
	u32 pixel_p1 = 1;
	u32 pixel_p2;
	u32 pixel_n = 1;
	u32 pixel_m1 = 1;
	u32 pixel_m2 = 1;

	u8 vga_gmbus = GMBUS_PORT_VGADDC;

	if (IS_ENABLED(CONFIG_GFX_GMA_ANALOG_I2C_HDMI_B))
		vga_gmbus = GMBUS_PORT_DPB;
	else if (IS_ENABLED(CONFIG_GFX_GMA_ANALOG_I2C_HDMI_C))
		vga_gmbus = GMBUS_PORT_DPC;
	else if (IS_ENABLED(CONFIG_GFX_GMA_ANALOG_I2C_HDMI_D))
		vga_gmbus = GMBUS_PORT_DPD;

	vga_gr_write(0x18, 0);

	/* Set up GTT */
	for (i = 0; i < 0x1000; i++) {
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
			| ADPA_DPMS_ON
			);

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

	/*
	 * TODO: check if it is actually an analog display.
	 * No harm is done but the console output could be confusing.
	 */
	intel_gmbus_read_edid(mmio + GMBUS0, vga_gmbus, 0x50, edid_data,
			sizeof(edid_data));
	intel_gmbus_stop(mmio + GMBUS0);
	decode_edid(edid_data,
		    sizeof(edid_data), &edid);
	mode = &edid.mode;


	/* Disable screen memory to prevent garbage from appearing.  */
	vga_sr_write(1, vga_sr_read(1) | 0x20);

	edid_is_found = edid_is_present(edid_data, sizeof(edid_data));
	if (edid_is_found) {
		printk(BIOS_DEBUG, "EDID is not null");
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
		target_frequency = mode->pixel_clock;
	} else
		printk(BIOS_DEBUG, "EDID is null, using 640 x 480 @ 60Hz mode");

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

		edid.bytes_per_line = (edid.bytes_per_line + 63) & ~63;

		write32(mmio + DSPCNTR(0), DISPLAY_PLANE_ENABLE
			| DISPPLANE_BGRX888);
		write32(mmio + DSPADDR(0), 0);
		write32(mmio + DSPSTRIDE(0), edid.bytes_per_line);
		write32(mmio + DSPSURF(0), 0);
		for (i = 0; i < 0x100; i++)
			write32(mmio + LGC_PALETTE(0) + 4 * i, i * 0x010101);
	} else {
		vga_textmode_init();
	}

	pixel_p2 = target_frequency <= 225000 ? 10 : 5;

	u32 candn, candm1, candm2, candp1;
	for (candn = 1; candn <= 4; candn++) {
		for (candm1 = 23; candm1 >= 16; candm1--) {
			for (candm2 = 11; candm2 >= 5; candm2--) {
				for (candp1 = 8; candp1 >= 1; candp1--) {
					u32 m = 5 * (candm1 + 2) + (candm2 + 2);
					u32 p = candp1 * pixel_p2;
					u32 vco = DIV_ROUND_CLOSEST(
						BASE_FREQUENCY * m, candn + 2);
					u32 dot = DIV_ROUND_CLOSEST(vco, p);
					u32 this_err = MAX(dot, target_frequency) -
						MIN(dot, target_frequency);
					if (this_err < err_most) {
						err_most = this_err;
						pixel_n = candn;
						pixel_m1 = candm1;
						pixel_m2 = candm2;
						pixel_p1 = candp1;
					}
				}
			}
		}
	}

	if (err_most == 0xffffffff) {
		printk(BIOS_ERR, "Couldn't find GFX clock divisors\n");
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
	printk(BIOS_DEBUG, "Polarities %d, %d\n",
	       hpolarity, vpolarity);
	printk(BIOS_DEBUG, "Pixel N=%d, M1=%d, M2=%d, P1=%d, P2=%d\n",
		pixel_n, pixel_m1, pixel_m2, pixel_p1, pixel_p2);
	printk(BIOS_DEBUG, "Pixel clock %d kHz\n",
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
			hactive * vactive * 4);
		set_vbe_mode_info_valid(&edid, lfb);
	}
}

static void native_init(struct device *dev)
{
	struct resource *lfb_res;
	struct resource *pio_res;
	u32 physbase;
	struct resource *gtt_res = find_resource(dev, PCI_BASE_ADDRESS_0);
	struct northbridge_intel_x4x_config *conf = dev->chip_info;

	lfb_res = find_resource(dev, PCI_BASE_ADDRESS_2);
	pio_res = find_resource(dev, PCI_BASE_ADDRESS_4);
	physbase = pci_read_config32(dev, 0x5c) & ~0xf;

	if (gtt_res && gtt_res->base) {
		printk(BIOS_SPEW,
			"Initializing VGA without OPROM. MMIO 0x%llx\n",
			gtt_res->base);
		intel_gma_init(conf, res2mmio(gtt_res, 0, 0),
			physbase, pio_res->base, lfb_res->base);
	}

	/* Linux relies on VBT for panel info.  */
	generate_fake_intel_oprom(&conf->gfx, dev, "$VBT EAGLELAKE");
}

static void gma_func0_init(struct device *dev)
{
	u16 reg16, ggc;
	u32 reg32;

	/* IGD needs to be Bus Master */
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	reg32 |= PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO;
	pci_write_config32(dev, PCI_COMMAND, reg32);

	/* configure GMBUSFREQ */
	reg16 = pci_read_config16(dev_find_slot(0, PCI_DEVFN(0x2, 0)), 0xcc);
	reg16 &= ~0x1ff;
	reg16 |= 0xbc;
	pci_write_config16(dev_find_slot(0, PCI_DEVFN(0x2, 0)), 0xcc, reg16);

	ggc = pci_read_config16(dev_find_slot(0, PCI_DEVFN(0, 0)), D0F0_GGC);

	if (IS_ENABLED(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT)) {
		if (ggc & (1 << 1)) {
			printk(BIOS_DEBUG, "VGA cycles not assigned to IGD. "
				"Not running native graphic init.\n");
			return;
		}
		native_init(dev);
	} else if (IS_ENABLED(CONFIG_MAINBOARD_USE_LIBGFXINIT)) {
		int lightup_ok;
		gma_gfxinit(&lightup_ok);
	} else {
		pci_dev_init(dev);
	}

	intel_gma_restore_opregion();
}

static void gma_func0_disable(struct device *dev)
{
	struct device *dev_host = dev_find_slot(0, PCI_DEVFN(0, 0));
	u16 ggc;

	ggc = pci_read_config16(dev_host, D0F0_GGC);
	ggc |= (1 << 1); /* VGA cycles to discrete GPU */
	pci_write_config16(dev_host, D0F0_GGC, ggc);
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

const struct i915_gpu_controller_info *
intel_gma_get_controller_info(void)
{
	struct device *dev = dev_find_slot(0, PCI_DEVFN(0x2, 0));
	if (!dev)
		return NULL;
	struct northbridge_intel_x4x_config *chip = dev->chip_info;
	return &chip->gfx;
}

static void gma_ssdt(struct device *device)
{
	const struct i915_gpu_controller_info *gfx = intel_gma_get_controller_info();
	if (!gfx)
		return;

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
	.ops_pci = &gma_pci_ops,
	.disable = gma_func0_disable,
	.acpi_name = gma_acpi_name,
	.write_acpi_tables = gma_write_acpi_tables,
};

static const unsigned short pci_device_ids[] = {
	0x2e02, /* Eaglelake */
	0x2e12, /* Q43/Q45 */
	0x2e22, /* G43/G45 */
	0x2e32, /* G41 */
	0x2e42, /* B43 */
	0x2e92, /* B43_I */
	0
};

static const struct pci_driver gma __pci_driver = {
	.ops = &gma_func0_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
