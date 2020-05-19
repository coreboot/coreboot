/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <cpu/amd/mtrr.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <drivers/i2c/designware/dw_i2c.h>
#include <romstage_handoff.h>
#include <soc/acpi.h>
#include <soc/cpu.h>
#include <soc/data_fabric.h>
#include <soc/pci_devs.h>
#include <soc/southbridge.h>
#include "chip.h"
#include <fsp/api.h>

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

	if (dev->bus->dev->path.type == DEVICE_PATH_DOMAIN) {
		switch (dev->path.pci.devfn) {
		case GNB_DEVFN:
			return "GNB";
		case IOMMU_DEVFN:
			return "IOMM";
		case PCIE_GPP_0_DEVFN:
			return "PBR0";
		case PCIE_GPP_1_DEVFN:
			return "PBR1";
		case PCIE_GPP_2_DEVFN:
			return "PBR2";
		case PCIE_GPP_3_DEVFN:
			return "PBR3";
		case PCIE_GPP_4_DEVFN:
			return "PBR4";
		case PCIE_GPP_5_DEVFN:
			return "PBR5";
		case PCIE_GPP_6_DEVFN:
			return "PBR6";
		case PCIE_GPP_A_DEVFN:
			return "PBRA";
		case PCIE_GPP_B_DEVFN:
			return "PBRB";
		case LPC_DEVFN:
			return "LPCB";
		case SMBUS_DEVFN:
			return "SBUS";
		default:
			printk(BIOS_WARNING, "Unknown root PCI device: dev: %d, fn: %d\n",
			       PCI_SLOT(dev->path.pci.devfn), PCI_FUNC(dev->path.pci.devfn));
			return NULL;
		}
	}

	if (dev->bus->dev->path.type == DEVICE_PATH_PCI
	    && dev->bus->dev->path.pci.devfn == PCIE_GPP_A_DEVFN) {
		switch (dev->path.pci.devfn) {
		case XHCI0_DEVFN:
			return "XHC0";
		case XHCI1_DEVFN:
			return "XHC1";
		default:
			printk(BIOS_WARNING, "Unknown Bus A PCI device: dev: %d, fn: %d\n",
			       PCI_SLOT(dev->path.pci.devfn), PCI_FUNC(dev->path.pci.devfn));
			return NULL;
		}
	}

	printk(BIOS_WARNING, "Unknown PCI device: dev: %d, fn: %d\n",
	       PCI_SLOT(dev->path.pci.devfn), PCI_FUNC(dev->path.pci.devfn));
	return NULL;
};

struct device_operations pci_domain_ops = {
	.read_resources	  = pci_domain_read_resources,
	.set_resources	  = pci_domain_set_resources,
	.scan_bus	  = pci_domain_scan_bus,
	.acpi_name	  = soc_acpi_name,
};

static struct device_operations pci_ops_ops_bus_ab = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.scan_bus		= pci_scan_bridge,
	.reset_bus		= pci_bus_reset,
	.acpi_fill_ssdt		= acpi_device_write_pci_dev,
};

static void enable_dev(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	} else if (dev->path.type == DEVICE_PATH_PCI) {
		if (dev->bus->dev->path.type == DEVICE_PATH_DOMAIN) {
			switch (dev->path.pci.devfn) {
			case PCIE_GPP_A_DEVFN:
			case PCIE_GPP_B_DEVFN:
				dev->ops = &pci_ops_ops_bus_ab;
			}
		}
		sb_enable(dev);
	} else if (dev->path.type == DEVICE_PATH_MMIO) {
		if (i2c_acpi_name(dev) != NULL)
			dev->ops = &picasso_i2c_mmio_ops;
	}
}

static void soc_init(void *chip_info)
{
	fsp_silicon_init(acpi_is_wakeup_s3());

	data_fabric_set_mmio_np();
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
