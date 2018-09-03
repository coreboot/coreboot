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

static void gma_func0_init(struct device *dev)
{
	u16 reg16;
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

	int vga_disable = (pci_read_config16(dev, D0F0_GGC) & 2) >> 1;

	if (IS_ENABLED(CONFIG_MAINBOARD_USE_LIBGFXINIT)) {
		if (vga_disable) {
			printk(BIOS_INFO,
			       "IGD is not decoding legacy VGA MEM and IO: skipping NATIVE graphic init\n");
		} else {
			int lightup_ok;
			gma_gfxinit(&lightup_ok);
		}
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
