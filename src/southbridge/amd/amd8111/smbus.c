/*
 * (C) 2004 Linux Networx
 */
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/smbus.h>
#include <arch/io.h>
#include "amd8111.h"


static void lpci_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	pci_write_config32(dev, 0x44,
		((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct smbus_bus_operations lops_smbus_bus = {
	/* I haven't seen the 2.0 SMBUS controller used yet. */
};
static struct pci_operations lops_pci = {
	.set_subsystem = lpci_set_subsystem,
};
static struct device_operations smbus_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources	   = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init		   = 0,
	.scan_bus	   = scan_static_bus,
	.enable	   = amd8111_enable,
	.ops_pci	   = &lops_pci,
	.ops_smbus_bus	   = &lops_smbus_bus,
};

static const struct pci_driver smbus_driver __pci_driver = {
	.ops = &smbus_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_8111_SMB,
};
