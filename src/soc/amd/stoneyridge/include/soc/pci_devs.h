/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Sage Electronic Engineering, LLC.
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

#ifndef _PI_STONEYRIDGE_PCI_DEVS_H_
#define _PI_STONEYRIDGE_PCI_DEVS_H_

#include <device/pci_def.h>
#include <rules.h>

/* HT Configuration */
#define HT_DEV			0x18
#define HT_FUNC			0
#define HT_DEVID		0x15b0
#define HT_DEVFN		PCI_DEVFN(HT_DEV, HT_FUNC)

/* Address Maps */
#define ADDR_DEV		0x18
#define ADDR_FUNC		1
#define ADDR_DEVID		0x15b1
#define ADDR_DEVFN		PCI_DEVFN(ADDR_DEV, ADDR_FUNC)

/* DRAM Configuration */
#define DCT_DEV			0x18
#define DCT_FUNC		2
#define DCT_DEVID		0x15b2
#define DCT_DEVFN		PCI_DEVFN(DCT_DEV, DCT_FUNC)

/* Misc. Configuration */
#define MISC_DEV		0x18
#define MISC_FUNC		3
#define MISC_DEVID		0x15b3
#define MISC_DEVFN		PCI_DEVFN(MISC_DEV, MISC_FUNC)

/* PM Configuration */
#define PM_DEV			0x18
#define PM_FUNC			4
#define PM_DEVID		0x15b4
#define PM_DEVFN		PCI_DEVFN(PM_DEV, PM_FUNC)
#if !defined(__SIMPLE_DEVICE__)
 #include <device/device.h>
 #define DEV_D18F4 dev_find_slot(0, PM_DEVFN)
#else
 #define DEV_D18F4 PCI_DEV(0, PM_DEV, PM_FUNC)
#endif

/* Northbridge Configuration */
#define NB_DEV			0x18
#define NB_FUNC			5
#define NB_DEVID		0x15b5
#define NB_DEVFN		PCI_DEVFN(NB_DEV, NB_FUNC)

/* GNB Root Complex */
#define GNB_DEV			0x0
#define GNB_FUNC		0
#define GNB_DEVID		0x1576
#define GNB_DEVFN		PCI_DEVFN(GNB_DEV, GNB_FUNC)

/* IOMMU */
#define IOMMU_DEV		0x0
#define IOMMU_FUNC		2
#define IOMMU_DEVID		0x1577
#define IOMMU_DEVFN		PCI_DEVFN(IOMMU_DEV, IOMMU_FUNC)

/* Internal Graphics */
#define GFX_DEV			0x1
#define GFX_FUNC		0
#define GFX_DEVID		098e4 /* subject to SKU/OPN variation */
#define GFX_DEVFN		PCI_DEVFN(GFX_DEV, GFX_FUNC)

/* HD Audio 0 */
#define HDA0_DEV		0x1
#define HDA0_FUNC		1
#define HDA0_DEVID		015b3
#define HDA0_DEVFN		PCI_DEVFN(HDA_DEV, HDA_FUNC)

/* Host Bridge */
#define HOST_DEV		0x2
#define HOST_FUNC		0
#define HOST_DEVID		0x157b
#define HOST_DEVFN		PCI_DEVFN(HOST_DEV, HOST_FUNC)

/* PCIe GPP Bridge 0 */
#define PCIE0_DEV		0x2
#define PCIE0_FUNC		1
#define PCIE0_DEVID		0x157c
#define PCIE0_DEVFN		PCI_DEVFN(PCIE0_DEV, PCIE0_FUNC)

/* PCIe GPP Bridge 1 */
#define PCIE1_DEV		0x2
#define PCIE1_FUNC		2
#define PCIE1_DEVID		0x157c
#define PCIE1_DEVFN		PCI_DEVFN(PCIE1_DEV, PCIE1_FUNC)

/* PCIe GPP Bridge 2 */
#define PCIE2_DEV		0x2
#define PCIE2_FUNC		3
#define PCIE2_DEVID		0x157c
#define PCIE2_DEVFN		PCI_DEVFN(PCIE2_DEV, PCIE2_FUNC)

/* PCIe GPP Bridge 3 */
#define PCIE3_DEV		0x2
#define PCIE3_FUNC		4
#define PCIE3_DEVID		0x157c
#define PCIE3_DEVFN		PCI_DEVFN(PCIE3_DEV, PCIE3_FUNC)

/* PCIe GPP Bridge 4 */
#define PCIE4_DEV		0x2
#define PCIE4_FUNC		5
#define PCIE4_DEVID		0x157c
#define PCIE4_DEVFN		PCI_DEVFN(PCIE4_DEV, PCIE4_FUNC)

/* Platform Security Processor */
#define PSP_DEV			0x8
#define PSP_FUNC		0
#define PSP_DEVID		0x1578
#define PSP_DEVFN		PCI_DEVFN(PSP_DEV, PSP_FUNC)

/* HD Audio 1 */
#define HDA1_DEV		0x9
#define HDA1_FUNC		2
#define HDA1_DEVID		0x157a
#define HDA1_DEVFN		PCI_DEVFN(HDA1_DEV, HDA1_FUNC)

/* HT Configuration */
#define HT_DEV			0x18
#define HT_FUNC			0
#define HT_DEVID		0x15b0
#define HT_DEVFN		PCI_DEVFN(HT_DEV, HT_FUNC)

/* Address Maps */
#define ADDR_DEV		0x18
#define ADDR_FUNC		1
#define ADDR_DEVID		0x15b1
#define ADDR_DEVFN		PCI_DEVFN(ADDR_DEV, ADDR_FUNC)

/* DRAM Configuration */
#define DCT_DEV			0x18
#define DCT_FUNC		2
#define DCT_DEVID		0x15b2
#define DCT_DEVFN		PCI_DEVFN(DCT_DEV, DCT_FUNC)

/* Misc. Configuration */
#define MISC_DEV		0x18
#define MISC_FUNC		3
#define MISC_DEVID		0x15b3
#define MISC_DEVFN		PCI_DEVFN(MISC_DEV, MISC_FUNC)

/* PM Configuration */
#define PM_DEV			0x18
#define PM_FUNC			4
#define PM_DEVID		0x15b4
#define PM_DEVFN		PCI_DEVFN(PM_DEV, PM_FUNC)
#if !defined(__SIMPLE_DEVICE__)
 #include <device/device.h>
 #define DEV_D18F4 dev_find_slot(0, PM_DEVFN)
#else
 #define DEV_D18F4 PCI_DEV(0, PM_DEV, PM_FUNC)
#endif

/* Northbridge Configuration */
#define NB_DEV			0x18
#define NB_FUNC			5
#define NB_DEVID		0x15b5
#define NB_DEVFN		PCI_DEVFN(NB_DEV, NB_FUNC)

/* GNB Root Complex */
#define GNB_DEV			0x0
#define GNB_FUNC		0
#define GNB_DEVID		0x1576
#define GNB_DEVFN		PCI_DEVFN(GNB_DEV, GNB_FUNC)

/* IOMMU */
#define IOMMU_DEV		0x0
#define IOMMU_FUNC		2
#define IOMMU_DEVID		0x1577
#define IOMMU_DEVFN		PCI_DEVFN(IOMMU_DEV, IOMMU_FUNC)

/* Internal Graphics */
#define GFX_DEV			0x1
#define GFX_FUNC		0
#define GFX_DEVID		098e4 /* subject to SKU/OPN variation */
#define GFX_DEVFN		PCI_DEVFN(GFX_DEV, GFX_FUNC)

/* HD Audio 0 */
#define HDA0_DEV		0x1
#define HDA0_FUNC		1
#define HDA0_DEVID		015b3
#define HDA0_DEVFN		PCI_DEVFN(HDA_DEV, HDA_FUNC)

/* Host Bridge */
#define HOST_DEV		0x2
#define HOST_FUNC		0
#define HOST_DEVID		0x157b
#define HOST_DEVFN		PCI_DEVFN(HOST_DEV, HOST_FUNC)

/* PCIe GPP Bridge 0 */
#define PCIE0_DEV		0x2
#define PCIE0_FUNC		1
#define PCIE0_DEVID		0x157c
#define PCIE0_DEVFN		PCI_DEVFN(PCIE0_DEV, PCIE0_FUNC)

/* PCIe GPP Bridge 1 */
#define PCIE1_DEV		0x2
#define PCIE1_FUNC		2
#define PCIE1_DEVID		0x157c
#define PCIE1_DEVFN		PCI_DEVFN(PCIE1_DEV, PCIE1_FUNC)

/* PCIe GPP Bridge 2 */
#define PCIE2_DEV		0x2
#define PCIE2_FUNC		3
#define PCIE2_DEVID		0x157c
#define PCIE2_DEVFN		PCI_DEVFN(PCIE2_DEV, PCIE2_FUNC)

/* PCIe GPP Bridge 3 */
#define PCIE3_DEV		0x2
#define PCIE3_FUNC		4
#define PCIE3_DEVID		0x157c
#define PCIE3_DEVFN		PCI_DEVFN(PCIE3_DEV, PCIE3_FUNC)

/* PCIe GPP Bridge 4 */
#define PCIE4_DEV		0x2
#define PCIE4_FUNC		5
#define PCIE4_DEVID		0x157c
#define PCIE4_DEVFN		PCI_DEVFN(PCIE4_DEV, PCIE4_FUNC)

/* Platform Security Processor */
#define PSP_DEV			0x8
#define PSP_FUNC		0
#define PSP_DEVID		0x1578
#define PSP_DEVFN		PCI_DEVFN(PSP_DEV, PSP_FUNC)

/* HD Audio 1 */
#define HDA1_DEV		0x9
#define HDA1_FUNC		2
#define HDA1_DEVID		0x157a
#define HDA1_DEVFN		PCI_DEVFN(HDA1_DEV, HDA1_FUNC)

/* XHCI */
#define XHCI_DEV		0x10
#define XHCI_FUNC		0
#define XHCI_DEVID		0x7914
#define XHCI_DEVFN		PCI_DEVFN(XHCI_DEV, XHCI_FUNC)

/* SATA */
#define SATA_DEV		0x11
#define SATA_FUNC		0
#define SATA_IDE_DEVID		0x7900
#define AHCI_DEVID_MS		0x7901
#define AHCI_DEVID_AMD		0x7904
#define SATA_DEVFN		PCI_DEVFN(SATA_DEV, SATA_FUNC)

/* EHCI */
#define EHCI_DEV		0x12
#define EHCI_FUNC		0
#define EHCI_DEVID		0x7908
#define EHCI1_DEVFN		PCI_DEVFN(EHCI_DEV, EHCI_FUNC)

/* SMBUS */
#define SMBUS_DEV		0x14
#define SMBUS_FUNC		0
#define SMBUS_DEVID		0x790b
#define SMBUS_DEVFN		PCI_DEVFN(SMBUS_DEV, SMBUS_FUNC)

/* LPC BUS */
#define PCU_DEV			0x14
#define LPC_FUNC		3
#define LPC_DEVID		0x790e
#define LPC_DEVFN		PCI_DEVFN(PCU_DEV, LPC_FUNC)

/* SD Controller */
#define SD_DEV			0x14
#define SD_FUNC			7
#define SD_DEVID		0x7906
#define SD_DEVFN		PCI_DEVFN(SD_DEV, SD_FUNC)

#endif /* _PI_STONEYRIDGE_PCI_DEVS_H_ */
