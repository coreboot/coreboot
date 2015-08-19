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
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <kconfig.h>

#include "drivers/intel/gma/i915_reg.h"
#include "chip.h"
#include "x4x.h"
#include <drivers/intel/gma/intel_bios.h>
#include <drivers/intel/gma/i915.h>
#include <pc80/vga.h>
#include <pc80/vga_io.h>

static struct resource *gtt_res = NULL;

void gtt_write(u32 reg, u32 data)
{
	write32(res2mmio(gtt_res, reg, 0), data);
}

static void intel_gma_init(const struct northbridge_intel_x4x_config *info,
			   u8 *mmio, u32 physbase, u16 piobase, u32 lfb)
{

	int i;
	u32 hactive, vactive;
	vga_gr_write(0x18, 0);

	/* Setup GTT.  */
	for (i = 0; i < 0x2000; i++)
	{
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

	/* Disable screen memory to prevent garbage from appearing.  */
	vga_sr_write(1, vga_sr_read(1) | 0x20);

	hactive = 640;
	vactive = 400;

	vga_textmode_init();

	mdelay(1);
	write32(mmio + FP0(0), 0x31108);
	write32(mmio + DPLL(0),
		DPLL_VCO_ENABLE | DPLLB_MODE_DAC_SERIAL
		| DPLL_DAC_SERIAL_P2_CLOCK_DIV_10
		| 0x10601
		);
	mdelay(1);
	write32(mmio + DPLL(0),
		DPLL_VCO_ENABLE | DPLLB_MODE_DAC_SERIAL
		| DPLL_DAC_SERIAL_P2_CLOCK_DIV_10
		| 0x10601
		);

	write32(mmio + ADPA, ADPA_DAC_ENABLE
			| ADPA_PIPE_A_SELECT
			| ADPA_CRT_HOTPLUG_MONITOR_COLOR
			| ADPA_CRT_HOTPLUG_ENABLE
			| ADPA_USE_VGA_HVPOLARITY
			| ADPA_VSYNC_CNTL_ENABLE
			| ADPA_HSYNC_CNTL_ENABLE
			| ADPA_DPMS_ON
			);

	write32(mmio + HTOTAL(0),
		((hactive - 1) << 16)
		| (hactive - 1));
	write32(mmio + HBLANK(0),
		((hactive - 1) << 16)
		| (hactive - 1));
	write32(mmio + HSYNC(0),
		((hactive - 1) << 16)
		| (hactive - 1));

	write32(mmio + VTOTAL(0), ((vactive - 1) << 16)
		| (vactive - 1));
	write32(mmio + VBLANK(0), ((vactive - 1) << 16)
		| (vactive - 1));
	write32(mmio + VSYNC(0),
		((vactive - 1) << 16)
		| (vactive - 1));

	write32(mmio + PIPECONF(0), PIPECONF_DISABLE);

	write32(mmio + PF_WIN_POS(0), 0);

	write32(mmio + PIPESRC(0), (639 << 16) | 399);
	write32(mmio + PF_CTL(0),PF_ENABLE | PF_FILTER_MED_3x3);
	write32(mmio + PF_WIN_SZ(0), vactive | (hactive << 16));
	write32(mmio + PFIT_CONTROL, 0xa0000000);

	mdelay(1);

	write32(mmio + 0x000f000c, 0x00002040);
	mdelay(1);
	write32(mmio + 0x000f000c, 0x00002050);
	write32(mmio + 0x00060100, 0x00044000);
	mdelay(1);
	write32(mmio + PIPECONF(0), PIPECONF_ENABLE | PIPECONF_BPP_6 | PIPECONF_DITHER_EN);

	write32(mmio + VGACNTRL, 0x0);
	write32(mmio + DSPCNTR(0), DISPLAY_PLANE_ENABLE | DISPPLANE_BGRX888);
	mdelay(1);

	write32(mmio + ADPA, ADPA_DAC_ENABLE
			| ADPA_PIPE_A_SELECT
			| ADPA_CRT_HOTPLUG_MONITOR_COLOR
			| ADPA_CRT_HOTPLUG_ENABLE
			| ADPA_USE_VGA_HVPOLARITY
			| ADPA_VSYNC_CNTL_ENABLE
			| ADPA_HSYNC_CNTL_ENABLE
			| ADPA_DPMS_ON
			);

	write32(mmio + PP_CONTROL, PANEL_POWER_ON);

	/* Enable screen memory.  */
	vga_sr_write(1, vga_sr_read(1) & ~0x20);

	/* Clear interrupts. */
	write32(mmio + DEIIR, 0xffffffff);
	write32(mmio + SDEIIR, 0xffffffff);
}

static void gma_func0_init(struct device *dev)
{
	u32 reg32;

	/* IGD needs to be Bus Master */
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	reg32 |= PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO;
	pci_write_config32(dev, PCI_COMMAND, reg32);

	/* Init graphics power management */
	gtt_res = find_resource(dev, PCI_BASE_ADDRESS_0);

	struct northbridge_intel_x4x_config *conf = dev->chip_info;

	if (!IS_ENABLED(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT)) {
		/* PCI Init, will run VBIOS */
		pci_dev_init(dev);
	} else {
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
		generate_fake_intel_oprom(&conf->gfx, dev, "$VBT EAGLELAKE      ");
	}

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
	struct northbridge_intel_x4x_config *chip = dev->chip_info;
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
	0x2e32, 0
};

static const struct pci_driver gma __pci_driver = {
	.ops = &gma_func0_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
