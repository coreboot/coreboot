/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/sram.h>
#include <soc/iomap.h>

__weak void soc_sram_init(struct device *dev) { /* no-op */ }

static void sram_read_resources(struct device *dev)
{
	struct resource *res;
	pci_dev_read_resources(dev);

	res = new_resource(dev, PCI_BASE_ADDRESS_0);
	res->base = SRAM_BASE_0;
	res->size = SRAM_SIZE_0;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, PCI_BASE_ADDRESS_2);
	res->base = SRAM_BASE_2;
	res->size = SRAM_SIZE_2;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static const struct device_operations device_ops = {
	.read_resources		= sram_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= soc_sram_init,
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_MTL_SOC_SRAM,
	PCI_DID_INTEL_MTL_IOE_M_SRAM,
	PCI_DID_INTEL_MTL_IOE_P_SRAM,
	PCI_DID_INTEL_MTL_CRASHLOG_SRAM,
	PCI_DID_INTEL_APL_SRAM,
	PCI_DID_INTEL_GLK_SRAM,
	PCI_DID_INTEL_ICL_SRAM,
	PCI_DID_INTEL_CMP_SRAM,
	PCI_DID_INTEL_CMP_H_SRAM,
	PCI_DID_INTEL_TGP_PMC_CRASHLOG_SRAM,
	PCI_DID_INTEL_TGL_H_SRAM,
	PCI_DID_INTEL_MCC_SRAM,
	PCI_DID_INTEL_JSP_SRAM,
	PCI_DID_INTEL_ADP_S_PMC_CRASHLOG_SRAM,
	PCI_DID_INTEL_ADP_P_PMC_CRASHLOG_SRAM,
	PCI_DID_INTEL_ADP_N_PMC_CRASHLOG_SRAM,
	0,
};

static const struct pci_driver sram __pci_driver = {
	.ops		= &device_ops,
	.vendor		= PCI_VID_INTEL,
	.devices	= pci_device_ids,
};
