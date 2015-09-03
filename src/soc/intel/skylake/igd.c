/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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

#include <arch/acpi.h>
#include <arch/io.h>
#include <chip.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <drivers/intel/gma/i915_reg.h>
#include <soc/cpu.h>
#include <soc/pm.h>
#include <soc/ramstage.h>
#include <soc/systemagent.h>
#include <stdlib.h>
#include <string.h>
#include <vendorcode/google/chromeos/chromeos.h>

u32 map_oprom_vendev(u32 vendev)
{
	return SA_IGD_OPROM_VENDEV;
}

static struct resource *gtt_res = NULL;

static unsigned long gtt_read(unsigned long reg)
{
	u32 val;
	val = read32((void *)(unsigned int)(gtt_res->base + reg));
	return val;
}

static void gtt_write(unsigned long reg, unsigned long data)
{
	write32((void *)(unsigned int)(gtt_res->base + reg), data);
}

static inline void gtt_rmw(u32 reg, u32 andmask, u32 ormask)
{
	u32 val = gtt_read(reg);
	val &= andmask;
	val |= ormask;
	gtt_write(reg, val);
}

static void igd_init(struct device *dev)
{
	/* IGD needs to be Bus Master */
	u32 reg32 = pci_read_config32(dev, PCI_COMMAND);
	reg32 |= PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO;
	pci_write_config32(dev, PCI_COMMAND, reg32);

	gtt_res = find_resource(dev, PCI_BASE_ADDRESS_0);
	if (!gtt_res || !gtt_res->base)
		return;

	/* Wait for any configured pre-graphics delay */
	if (!acpi_is_wakeup_s3()) {
#if IS_ENABLED(CONFIG_CHROMEOS)
		if (developer_mode_enabled() || recovery_mode_enabled() ||
		    vboot_wants_oprom())
			mdelay(CONFIG_PRE_GRAPHICS_DELAY);
#else
		mdelay(CONFIG_PRE_GRAPHICS_DELAY);
#endif
	}

	/* Initialize PCI device, load/execute BIOS Option ROM */
	pci_dev_init(dev);

#if IS_ENABLED(CONFIG_CHROMEOS)
	if (!gfx_get_init_done() && !acpi_is_wakeup_s3()) {
		/*
		 * Enable DDI-A if the Option ROM did not execute:
		 *
		 * bit 0: Display detected (RO)
		 * bit 4: DDI A supports 4 lanes and DDI E is not used
		 * bit 7: DDI buffer is idle
		 */
		gtt_write(DDI_BUF_CTL_A, DDI_BUF_IS_IDLE | DDI_A_4_LANES |
			  DDI_INIT_DISPLAY_DETECTED);
	}
#endif
}

static struct device_operations igd_ops = {
	.read_resources		= &pci_dev_read_resources,
	.set_resources		= &pci_dev_set_resources,
	.enable_resources	= &pci_dev_enable_resources,
	.init			= &igd_init,
	.ops_pci		= &soc_pci_ops,
};

static const unsigned short pci_device_ids[] = {
	IGD_SKYLAKE_GT1_SULTM,
	IGD_SKYLAKE_GT2_SULXM,
	IGD_SKYLAKE_GT2_SULTM,
	0,
};

static const struct pci_driver igd_driver __pci_driver = {
	.ops	 = &igd_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
