/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <bootstate.h>
#include <cpu/amd/mtrr.h>
#include <device/device.h>
#include <device/pci.h>
#include <drivers/i2c/designware/dw_i2c.h>
#include <romstage_handoff.h>
#include <soc/acpi.h>
#include <soc/cpu.h>
#include <soc/northbridge.h>
#include <soc/pci_devs.h>
#include <soc/southbridge.h>
#include "chip.h"

/* Supplied by i2c.c */
extern struct device_operations picasso_i2c_mmio_ops;
extern const char *i2c_acpi_name(const struct device *dev);

struct device_operations cpu_bus_ops = {
	.read_resources	  = noop_read_resources,
	.set_resources	  = noop_set_resources,
	.init		  = picasso_init_cpus,
	.acpi_fill_ssdt   = generate_cpu_entries,
};

const char *soc_acpi_name(const struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		return "PCI0";

	if (dev->path.type == DEVICE_PATH_USB) {
		switch (dev->path.usb.port_type) {
		case 0:
			/* Root Hub */
			return "RHUB";
		case 3:
			/* USB3 ports */
			switch (dev->path.usb.port_id) {
			case 0: return "SS01";
			case 1: return "SS02";
			case 2: return "SS03";
			}
			break;
		}
		return NULL;
	}

	if (dev->path.type != DEVICE_PATH_PCI)
		return NULL;

	switch (dev->path.pci.devfn) {
	case GFX_DEVFN:
		return "IGFX";
	case PCIE0_DEVFN:
		return "PBR4";
	case PCIE1_DEVFN:
		return "PBR5";
	case PCIE2_DEVFN:
		return "PBR6";
	case PCIE3_DEVFN:
		return "PBR7";
	case PCIE4_DEVFN:
		return "PBR8";
	case HDA1_DEVFN:
		return "AZHD";
	case LPC_DEVFN:
		return "LPCB";
	case SATA_DEVFN:
		return "STCR";
	case SMBUS_DEVFN:
		return "SBUS";
	case XHCI0_DEVFN:
		return "XHC0";
	case XHCI1_DEVFN:
		return "XHC1";
	default:
		return NULL;
	}
};

struct device_operations pci_domain_ops = {
	.read_resources	  = pci_domain_read_resources,
	.set_resources	  = domain_set_resources,
	.scan_bus	  = pci_domain_scan_bus,
	.acpi_name	  = soc_acpi_name,
};

static void enable_dev(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		dev->ops = &pci_domain_ops;
	else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER)
		dev->ops = &cpu_bus_ops;
	else if (dev->path.type == DEVICE_PATH_PCI)
		sb_enable(dev);
	else if (dev->path.type == DEVICE_PATH_MMIO)
		if (i2c_acpi_name(dev) != NULL)
			dev->ops = &picasso_i2c_mmio_ops;
}

static void soc_init(void *chip_info)
{
	southbridge_init(chip_info);
	setup_bsp_ramtop();
}

static void soc_final(void *chip_info)
{
	southbridge_final(chip_info);
}

struct chip_operations soc_amd_picasso_ops = {
	CHIP_NAME("AMD Picasso SOC")
	.enable_dev = enable_dev,
	.init = soc_init,
	.final = soc_final
};
