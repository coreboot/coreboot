/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <stdint.h>
#include <console/console.h>
#include <device/device.h>
#include <arch/ioapic.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "NbPlatform.h"
#include "nb_cimx.h"
#include "rd890_cfg.h"

void nb_Early_Post_Init(AMD_NB_CONFIG_BLOCK *pConfig)
{
	LibSystemApiCall(AmdEarlyPostInit, pConfig);
}

void nb_Mid_Post_Init(AMD_NB_CONFIG_BLOCK *pConfig)
{
	LibSystemApiCall(AmdMidPostInit, pConfig);
}

void nb_Late_Post_Init(AMD_NB_CONFIG_BLOCK *pConfig)
{
	LibSystemApiCall(AmdLatePostInit, pConfig);
}

static void rd890_enable(device_t dev)
{
	NB_CONFIG nb_cfg[MAX_NB_COUNT];
	HT_CONFIG ht_cfg[MAX_NB_COUNT];
	PCIE_CONFIG pcie_cfg[MAX_NB_COUNT];
	AMD_NB_CONFIG_BLOCK gConfig;
	u32 devfn = dev->path.pci.devfn;

	/* only do Post init once */
	if(devfn == 0)
	{
		printk(BIOS_INFO, "cimx Post init start...\n");
		rd890_cimx_config(&gConfig, &nb_cfg[0], &ht_cfg[0], &pcie_cfg[0]);
		nb_pcie_config(&gConfig);

		//nb_Pcie_Late_Init(&gConfig);
		nb_Early_Post_Init(&gConfig);
		nb_Mid_Post_Init(&gConfig);
		nb_Late_Post_Init(&gConfig);
		printk(BIOS_INFO, "cimx Post init exit...\n");
	}
}

struct chip_operations northbridge_amd_cimx_rd890_ops = {
	CHIP_NAME("ATI rd890")
	.enable_dev = rd890_enable,
};

static void ioapic_init(struct device *dev)
{
	u32 ioapic_base;

	pci_write_config32(dev, 0xF8, 0x1);
	ioapic_base = pci_read_config32(dev, 0xFC) & 0xfffffff0;
	setup_ioapic(ioapic_base, 1);
}

static void rd890_read_resource(struct device *dev)
{
	pci_dev_read_resources(dev);

	/* rpr6.2.(1). Write the Base Address Register (BAR) */
	pci_write_config32(dev, 0xF8, 0x1); /* set IOAPIC's index as 1 and make sure no one changes it. */
	pci_get_resource(dev, 0xFC); /* APIC located in sr5690 */
	compact_resources(dev);
}

/* If IOAPIC's index changes, we should replace the pci_dev_set_resource(). */
static void rd890_set_resources(struct device *dev)
{
	pci_write_config32(dev, 0xF8, 0x1); /* set IOAPIC's index as 1 and make sure no one changes it. */
	pci_dev_set_resources(dev);
}

static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

static struct device_operations ht_ops = {
	.read_resources = rd890_read_resource,
	.set_resources = rd890_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = ioapic_init,
	.scan_bus = 0,
	.ops_pci = &lops_pci,
};

static const unsigned short driver_ids[] = {
	PCI_DEVICE_ID_AMD_SR5690_HT,
	PCI_DEVICE_ID_AMD_SR5670_HT,
	PCI_DEVICE_ID_AMD_SR5650_HT,
	PCI_DEVICE_ID_AMD_RD890TV_HT,
	PCI_DEVICE_ID_AMD_RD890_HT,
	PCI_DEVICE_ID_AMD_990FX_HT,
	0
};

static const struct pci_driver ht_driver_sr5690 __pci_driver = {
	.ops = &ht_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.devices= driver_ids,
};
