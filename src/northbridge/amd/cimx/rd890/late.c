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


/**
 * Global RD890 CIMX Configuration structure
 */
static NB_CONFIG nb_cfg[MAX_NB_COUNT];
static HT_CONFIG ht_cfg[MAX_NB_COUNT];
static PCIE_CONFIG pcie_cfg[MAX_NB_COUNT];
static AMD_NB_CONFIG_BLOCK gConfig;


/**
 * Reset PCIE Cores, Training the Ports selected by port_enable of devicetree
 * After this call EP are fully operational on particular NB
 */
void nb_Pcie_Early_Init(void)
{
	LibSystemApiCall(AmdPcieEarlyInit, &gConfig); //AmdPcieEarlyInit(&gConfig);
}

void nb_Pcie_Late_Init(void)
{
	LibSystemApiCall(AmdPcieLateInit, &gConfig);
}

void nb_Early_Post_Init(void)
{
	LibSystemApiCall(AmdEarlyPostInit, &gConfig);
}

void nb_Mid_Post_Init(void)
{
	LibSystemApiCall(AmdMidPostInit, &gConfig);
}

void nb_Late_Post_Init(void)
{
	LibSystemApiCall(AmdLatePostInit, &gConfig);
}

static void rd890_enable(device_t dev)
{
	u32 address = 0;
	u32 mask;
	u32 val;
	u32 devfn;
	u32 port;
	AMD_NB_CONFIG *NbConfigPtr = NULL;

	u8 nb_index = 0; /* The first IO Hub, TODO: other NBs */
	address = MAKE_SBDFO(0, 0x0, 0x0, 0x0, 0x0);
	NbConfigPtr = &(gConfig.Northbridges[nb_index]);

	devfn = dev->path.pci.devfn;
	port = devfn >> 3;
	printk(BIOS_INFO, "rd890_enable  ");
	printk(BIOS_INFO, "Bus-%x Dev-%X Fun-%X, enable=%x\n",
			0, (devfn >> 3), (devfn & 0x07), dev->enabled);
	if (port != 0) {
		if (dev->enabled) {
			NbConfigPtr->pPcieConfig->PortConfiguration[port].ForcePortDisable = OFF;
		} else {
			NbConfigPtr->pPcieConfig->PortConfiguration[port].ForcePortDisable = ON;
		}
	}

	switch (port) {
		case 0x0: /* Root Complex, and ClkConfig */

			if ((devfn & 0x07) == 1) { /* skip dev-0 fun-1 */
				break;
			}

			/* CIMX configuration defualt initialize */
			rd890_cimx_config(&gConfig, &nb_cfg[0], &ht_cfg[0], &pcie_cfg[0]);
			if (gConfig.StandardHeader.CalloutPtr != NULL) {
				/* NOTE: not use LibNbCallBack */
				gConfig.StandardHeader.CalloutPtr(CB_AmdSetPcieEarlyConfig, (u32)dev, (VOID*)NbConfigPtr);
			}
			/* Reset PCIE Cores, Training the Ports selected by port_enable of devicetree
			 * After this call EP are fully operational on particular NB
			 */
			nb_Pcie_Early_Init();
			break;

		case 0x2: /* Gpp1 Port0 */
		case 0x3: /* Gpp1 Port1 */
			mask = ~(1 << port);
			val = (dev->enabled ? 0 : 1) << port;
			LibNbPciIndexRMW(address | NB_MISC_INDEX, NB_MISC_REG0C, AccessS3SaveWidth32, mask, val, NbConfigPtr);
			break;

		case 0x4: /* Gpp3a Port0 */
		case 0x5: /* Gpp3a Port1 */
		case 0x6: /* Gpp3a Port2 */
		case 0x7: /* Gpp3a Port3 */
			mask = ~(1 << port);
			val = (dev->enabled ? 0 : 1) << port;
			LibNbPciIndexRMW(address | NB_MISC_INDEX, NB_MISC_REG0C, AccessS3SaveWidth32, mask, val, NbConfigPtr);
			break;

		case 0x8: /* SB ALink */
			mask = ~(1 << 6);
			val = (dev->enabled ? 1 : 0) << 6;
			LibNbPciIndexRMW(address | NB_MISC_INDEX, NB_MISC_REG0C, AccessS3SaveWidth32, mask, val, NbConfigPtr);
			break;

		case 0x9: /* Gpp3a Port4 */
		case 0xa: /* Gpp3a Port5 */
			mask = ~(1 << (7 + port));
			val = (dev->enabled ? 0 : 1) << (7 + port);
			LibNbPciIndexRMW(address | NB_MISC_INDEX, NB_MISC_REG0C, AccessS3SaveWidth32, mask, val, NbConfigPtr);
			break;

		case 0xb: /* Gpp2 Port0 */
		case 0xc: /* Gpp2 Port1 */
			mask = ~(1 << (7 + port));
			val = (dev->enabled ? 0 : 1) << (7 + port);
			LibNbPciIndexRMW(address | NB_MISC_INDEX, NB_MISC_REG0C, AccessS3SaveWidth32, mask, val, NbConfigPtr);
			break;

		case 0xd: /* Gpp3b */
			mask = ~(1 << (7 + port));
			val = (dev->enabled ? 0 : 1) << (7 + port);
			LibNbPciIndexRMW(address | NB_MISC_INDEX, NB_MISC_REG0C, AccessS3SaveWidth32, mask, val, NbConfigPtr);

			/* Init NB at Early Post */
			if (gConfig.StandardHeader.CalloutPtr != NULL) {
				gConfig.StandardHeader.CalloutPtr(CB_AmdSetEarlyPostConfig, 0, (VOID*)NbConfigPtr);
			}
			nb_Early_Post_Init();//
			if (gConfig.StandardHeader.CalloutPtr != NULL) {
				gConfig.StandardHeader.CalloutPtr(CB_AmdSetMidPostConfig, 0, (VOID*)NbConfigPtr);
			}
			nb_Mid_Post_Init();
			nb_Pcie_Late_Init();
			if (gConfig.StandardHeader.CalloutPtr != NULL) {
				gConfig.StandardHeader.CalloutPtr(CB_AmdSetLatePostConfig, 0, (VOID*)NbConfigPtr);
			}
			nb_Late_Post_Init();
			break;

		default:
			printk(BIOS_INFO, "Buggy Device Tree\n");
			break;
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

static const struct pci_driver ht_driver_sr5690 __pci_driver = {
	.ops = &ht_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_AMD_SR5690_HT,
};

static const struct pci_driver ht_driver_sr5670 __pci_driver = {
	.ops = &ht_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_AMD_SR5670_HT,
};

static const struct pci_driver ht_driver_sr5650 __pci_driver = {
	.ops = &ht_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_AMD_SR5650_HT,
};

static const struct pci_driver ht_driver_rd890tv __pci_driver = {
	.ops = &ht_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_AMD_RD890TV_HT,
};

static const struct pci_driver ht_driver_rx780 __pci_driver = {
	.ops = &ht_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_AMD_RX780_HT,
};

static const struct pci_driver ht_driver_rd780 __pci_driver = {
	.ops = &ht_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_AMD_RD780_HT,
};

static const struct pci_driver ht_driver_rd890 __pci_driver = {
	.ops = &ht_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_AMD_RD890_HT,
};

static const struct pci_driver ht_driver_990fx __pci_driver = {
	.ops = &ht_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_AMD_990FX_HT,
};
