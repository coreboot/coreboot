/* SPDX-License-Identifier: GPL-2.0-only */

#include <static_devices.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <drivers/intel/gma/opregion.h>
#include <drivers/intel/gma/gma.h>
#include <types.h>

#include "gm965.h"
#include "chip.h"

#define GDRST 0xc0
#define MSAC  0x62 /* Multi Size Aperture Control */

static void gma_func0_init(struct device *dev)
{
	intel_gma_init_igd_opregion();

	/* Unconditionally reset graphics */
	pci_write_config8(dev, GDRST, 1);
	udelay(50);
	pci_write_config8(dev, GDRST, 0);
	/* wait for device to finish */
	while (pci_read_config8(dev, GDRST) & 1)
		;

	if (!CONFIG(NO_GFX_INIT))
		pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);

	/* PCI Init, will run VBIOS */
	pci_dev_init(dev);
}

/* This doesn't reclaim stolen UMA memory, but IGD could still
   be re-enabled later. */
static void gma_func0_disable(struct device *dev)
{
	struct device *dev_host = __pci_0_00_0;

	pci_write_config16(dev, GCFGC_OFFSET, 0xa00);
	pci_write_config16(dev_host, D0F0_GGC, (1 << 1));

	pci_and_config32(dev_host, D0F0_DEVEN, ~(DEVEN_D2F0 | DEVEN_D2F1));

	dev->enabled = 0;
}

static void gma_func1_init(struct device *dev)
{
	if (!CONFIG(NO_GFX_INIT))
		pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);
}

static void gma_generate_ssdt(const struct device *device)
{
	const struct northbridge_intel_gm965_config *chip = device->chip_info;

	drivers_intel_gma_displays_ssdt_generate(&chip->gfx);
}

static void gma_func0_read_resources(struct device *dev)
{
	/* Set Untrusted Aperture Size to 256MB */
	pci_update_config8(dev, MSAC, ~0x3, 0x2);

	pci_dev_read_resources(dev);
}

static const char *gma_acpi_name(const struct device *dev)
{
	return "GFX0";
}

static struct device_operations gma_func0_ops = {
	.read_resources		= gma_func0_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= gma_func0_init,
	.acpi_fill_ssdt		= gma_generate_ssdt,
	.vga_disable		= gma_func0_disable,
	.ops_pci		= &pci_dev_ops_pci,
	.acpi_name		= gma_acpi_name,
};

static struct device_operations gma_func1_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= gma_func1_init,
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short gm965_gma_func0_ids[] = {
	PCI_DID_GM965_IGD,   /* 0x2A02 - Mobile Intel GM965/GL960 Express Integrated Graphics */
	0
};

static const unsigned short gm965_gma_func1_ids[] = {
	PCI_DID_GM965_IGD_1, /* 0x2A03 - Mobile Intel GM965/GL960 Express Integrated Graphics (alt) */
	0
};

static const struct pci_driver gm965_gma_func0_driver __pci_driver = {
	.ops		= &gma_func0_ops,
	.vendor		= PCI_VID_INTEL,
	.devices	= gm965_gma_func0_ids,
};

static const struct pci_driver gm965_gma_func1_driver __pci_driver = {
	.ops		= &gma_func1_ops,
	.vendor		= PCI_VID_INTEL,
	.devices	= gm965_gma_func1_ids,
};
