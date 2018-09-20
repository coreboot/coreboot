/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Chromium OS Authors
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
#include <southbridge/intel/fsp_bd82x6x/nvs.h>
#include <drivers/intel/gma/opregion.h>
#include <drivers/intel/gma/intel_bios.h>

#include <cbmem.h>

#include "chip.h"
#include "northbridge.h"


/* some vga option roms are used for several chipsets but they only have one
 * PCI ID in their header. If we encounter such an option rom, we need to do
 * the mapping ourselves
 */

u32 map_oprom_vendev(u32 vendev)
{
	u32 new_vendev = vendev;

	switch (vendev) {
	case 0x80860102:		/* GT1 Desktop */
	case 0x8086010a:		/* GT1 Server */
	case 0x80860112:		/* GT2 Desktop */
	case 0x80860116:		/* GT2 Mobile */
	case 0x80860122:		/* GT2 Desktop >=1.3GHz */
	case 0x80860126:		/* GT2 Mobile >=1.3GHz */
	case 0x80860166:                /* IVB */
		new_vendev = 0x80860106;	/* GT1 Mobile */
		break;
	}

	return new_vendev;
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

static void gma_set_subsystem(struct device *dev, unsigned int vendor,
				unsigned int device)
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
	struct device *dev = dev_find_slot(0, PCI_DEVFN(0x2,0));
	if (!dev) {
		return NULL;
	}
	struct northbridge_intel_fsp_sandybridge_config *chip = dev->chip_info;
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

/* Enable SCI to ACPI _GPE._L06 */
static void gma_enable_swsci(void)
{
	u16 reg16;

	/* clear DMISCI status */
	reg16 = inw(DEFAULT_PMBASE + TCO1_STS);
	reg16 &= DMISCI_STS;
	outw(DEFAULT_PMBASE + TCO1_STS, reg16);

	/* clear acpi tco status */
	outl(DEFAULT_PMBASE + GPE0_STS, TCOSCI_STS);

	/* enable acpi tco scis */
	reg16 = inw(DEFAULT_PMBASE + GPE0_EN);
	reg16 |= TCOSCI_EN;
	outw(DEFAULT_PMBASE + GPE0_EN, reg16);
}

static void gma_init(struct device *dev)
{
	pci_dev_init(dev);

	gma_enable_swsci();
	intel_gma_restore_opregion();
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

static struct pci_operations gma_pci_ops = {
	.set_subsystem    = gma_set_subsystem,
};

static struct device_operations gma_func0_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.acpi_fill_ssdt_generator = gma_ssdt,
	.init			= gma_init,
	.scan_bus		= 0,
	.enable			= 0,
	.ops_pci		= &gma_pci_ops,
	.write_acpi_tables	= gma_write_acpi_tables,
};

static const unsigned short gma_ids[] = {
	0x0102, 0x0106, 0x010a, 0x0112, 0x0116, 0x0122, 0x0126, 0x166,
	0,
};
static const struct pci_driver gma_gt1_desktop __pci_driver = {
	.ops	= &gma_func0_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.devices= gma_ids,
};
