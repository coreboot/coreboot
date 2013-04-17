/*
 * This file is part of the coreboot project.
 *
 * Copyright 2012 Google Inc.
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

#include "chip.h"
#include "haswell.h"

/* some vga option roms are used for several chipsets but they only have one
 * PCI ID in their header. If we encounter such an option rom, we need to do
 * the mapping ourselfes
 */

u32 map_oprom_vendev(u32 vendev)
{
	u32 new_vendev=vendev;

	switch (vendev) {
	case 0x80860402:		/* GT1 Desktop */
	case 0x80860406:		/* GT1 Mobile */
	case 0x8086040a:		/* GT1 Server */
	case 0x80860a06:		/* GT1 ULT */

	case 0x80860412:		/* GT2 Desktop */
	case 0x80860416:		/* GT2 Mobile */
	case 0x8086041a:		/* GT2 Server */
	case 0x80860a16:		/* GT2 ULT */

	case 0x80860422:		/* GT3 Desktop */
	case 0x80860426:		/* GT3 Mobile */
	case 0x8086042a:		/* GT3 Server */
	case 0x80860a26:		/* GT3 ULT */

		new_vendev=0x80860406;	/* GT1 Mobile */
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
	printk(BIOS_DEBUG, "GT Power Management Init\n");

	gtt_res = find_resource(dev, PCI_BASE_ADDRESS_0);
	if (!gtt_res || !gtt_res->base)
		return;

	/*
	 * Enable RC6
	 */

	/* Enable Force Wake */
	gtt_write(0x0a180, 1 << 5);
	gtt_write(0x0a188, 0x00010001);
	gtt_poll(0x130044, 1 << 0, 1 << 0);

	/* Enable counters and lock */
	gtt_write(0x0a248, 0x80000016);
	gtt_write(0x0a000, 0x00070020);
	gtt_write(0x0a180, 0xc5000020);

	/* Enable DOP clock gating */
	gtt_write(0x09424, 0x00000001);

	/* Enable unit level clock gating */
	gtt_write(0x09400, 0x00000080);
	gtt_write(0x09404, 0x40401000);
	gtt_write(0x09408, 0x00000000);
	gtt_write(0x0940c, 0x02000001);

	/* Configure max ilde count */
	gtt_write(0x02054, 0x0000000a);
	gtt_write(0x12054, 0x0000000a);
	gtt_write(0x22054, 0x0000000a);

	gtt_write(0x0a008, 0x10000000);
	gtt_write(0x0a024, 0x00000b92);

	/* Enable RC6 in idle */
	gtt_write(0x0a094, 0x00040000);
}

static void gma_pm_init_post_vbios(struct device *dev)
{
	struct northbridge_intel_haswell_config *conf = dev->chip_info;
	u32 reg32;

	printk(BIOS_DEBUG, "GT Power Management Init (post VBIOS)\n");

	/* Disable Force Wake */
	gtt_write(0x0a188, 0x00010000);
	gtt_poll(0x130044, 1 << 0, 0 << 0);

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

	/* Enable Backlight if needed */
	if (conf->gpu_cpu_backlight) {
		gtt_write(0x48250, (1 << 31));
		gtt_write(0x48254, conf->gpu_cpu_backlight);
	}
	if (conf->gpu_pch_backlight) {
		gtt_write(0xc8250, (1 << 31));
		gtt_write(0xc8254, conf->gpu_pch_backlight);
	}
}

static void gma_func0_init(struct device *dev)
{
	int lightup_ok = 0;
	u32 reg32;
	u32 graphics_base; //, graphics_size;
	/* IGD needs to be Bus Master */
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	reg32 |= PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO;
	pci_write_config32(dev, PCI_COMMAND, reg32);


	/* the BAR for graphics space is a well known number for
	 * sandy and ivy. And the resource code renumbers it.
	 * So it's almost like having two hardcodes.
	 */
	graphics_base = dev->resource_list[1].base;
	/* Init graphics power management */
	gma_pm_init_pre_vbios(dev);

#if CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT
	printk(BIOS_SPEW, "NATIVE graphics, run native enable\n");
	u32 iobase, mmiobase, physbase;
	iobase = dev->resource_list[2].base;
	mmiobase = dev->resource_list[0].base;
	physbase = pci_read_config32(dev, 0x5c) & ~0xf;

	int i915lightup(u32 physbase, u32 iobase, u32 mmiobase, u32 gfx);
	lightup_ok = i915lightup(physbase, iobase, mmiobase, graphics_base);
#endif
	if (! lightup_ok) {
		printk(BIOS_SPEW, "FUI did not run; using VBIOS\n");
		pci_dev_init(dev);
	}

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

static void gma_read_resources(struct device *dev)
{
	pci_dev_read_resources(dev);

#if CONFIG_MARK_GRAPHICS_MEM_WRCOMB
	struct resource *res;

	/* Set the graphics memory to write combining. */
	res = find_resource(dev, PCI_BASE_ADDRESS_2);
	if (res == NULL) {
		printk(BIOS_DEBUG, "gma: memory resource not found.\n");
		return;
	}
	res->flags |= IORESOURCE_WRCOMB;
#endif
}

static struct pci_operations gma_pci_ops = {
	.set_subsystem    = gma_set_subsystem,
};

static struct device_operations gma_func0_ops = {
	.read_resources		= gma_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= gma_func0_init,
	.scan_bus		= 0,
	.enable			= 0,
	.ops_pci		= &gma_pci_ops,
};

static const unsigned short pci_device_ids[] = {
	0x0402, /* Desktop GT1 */
	0x0412, /* Desktop GT2 */
	0x0422, /* Desktop GT3 */
	0x0406, /* Mobile GT1 */
	0x0416, /* Mobile GT2 */
	0x0426, /* Mobile GT3 */
	0x0d16, /* Mobile 4+3 GT1 */
	0x0d26, /* Mobile 4+3 GT2 */
	0x0d36, /* Mobile 4+3 GT3 */
	0x0a06, /* ULT GT1 */
	0x0a16, /* ULT GT2 */
	0x0a26, /* ULT GT3 */
	0,
};

static const struct pci_driver pch_lpc __pci_driver = {
	.ops	 = &gma_func0_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
