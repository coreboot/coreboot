/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __PI_PICASSO_PCI_DEVS_H__
#define __PI_PICASSO_PCI_DEVS_H__

#include <device/pci_def.h>

#if !defined(__SIMPLE_DEVICE__)
#include <device/device.h>
#define _SOC_DEV(slot, func)	pcidev_on_root(slot, func)
#else
#define _SOC_DEV(slot, func)	PCI_DEV(0, slot, func)
#endif

/* GNB Root Complex */
#define GNB_DEV			0x0
#define GNB_FUNC		0
#define GNB_DEVID		0x1576
#define GNB_DEVFN		PCI_DEVFN(GNB_DEV, GNB_FUNC)
#define SOC_GNB_DEV		_SOC_DEV(GNB_DEV, GNB_FUNC)

/* IOMMU */
#define IOMMU_DEV		0x0
#define IOMMU_FUNC		2
#define IOMMU_DEVID		0x1577
#define IOMMU_DEVFN		PCI_DEVFN(IOMMU_DEV, IOMMU_FUNC)
#define SOC_IOMMU_DEV		_SOC_DEV(IOMMU_DEV, IOMMU_FUNC)

/* Internal Graphics */
#define GFX_DEV			0x1
#define GFX_FUNC		0
#define GFX_DEVID		0x15d8
#define GFX_DEVFN		PCI_DEVFN(GFX_DEV, GFX_FUNC)
#define SOC_GFX_DEV		_SOC_DEV(GFX_DEV, GFX_FUNC)

/* HD Audio 0 */
#define HDA0_DEV		0x1
#define HDA0_FUNC		1
#define HDA0_DEVID		0x15b3
#define HDA0_DEVFN		PCI_DEVFN(HDA0_DEV, HDA0_FUNC)
#define SOC_HDA0_DEV		_SOC_DEV(HDA0_DEV, HDA0_FUNC)

/* Host Bridge */
#define HOST_DEV		0x2
#define HOST_FUNC		0
#define HOST_DEVID		0x157b
#define HOST_DEVFN		PCI_DEVFN(HOST_DEV, HOST_FUNC)
#define SOC_HOST_DEV		_SOC_DEV(HOST_DEV, HOST_FUNC)

/* PCIe GPP Bridge 0 */
#define PCIE0_DEV		0x2
#define PCIE0_FUNC		1
#define PCIE0_DEVID		0x157c
#define PCIE0_DEVFN		PCI_DEVFN(PCIE0_DEV, PCIE0_FUNC)
#define SOC_PCIE0_DEV		_SOC_DEV(PCIE0_DEV, PCIE0_FUNC)

/* PCIe GPP Bridge 1 */
#define PCIE1_DEV		0x2
#define PCIE1_FUNC		2
#define PCIE1_DEVID		0x157c
#define PCIE1_DEVFN		PCI_DEVFN(PCIE1_DEV, PCIE1_FUNC)
#define SOC_PCIE1_DEV		_SOC_DEV(PCIE1_DEV, PCIE1_FUNC)

/* PCIe GPP Bridge 2 */
#define PCIE2_DEV		0x2
#define PCIE2_FUNC		3
#define PCIE2_DEVID		0x157c
#define PCIE2_DEVFN		PCI_DEVFN(PCIE2_DEV, PCIE2_FUNC)
#define SOC_PCIE2_DEV		_SOC_DEV(PCIE2_DEV, PCIE2_FUNC)

/* PCIe GPP Bridge 3 */
#define PCIE3_DEV		0x2
#define PCIE3_FUNC		4
#define PCIE3_DEVID		0x157c
#define PCIE3_DEVFN		PCI_DEVFN(PCIE3_DEV, PCIE3_FUNC)
#define SOC_PCIE3_DEV		_SOC_DEV(PCIE3_DEV, PCIE3_FUNC)

/* PCIe GPP Bridge 4 */
#define PCIE4_DEV		0x2
#define PCIE4_FUNC		5
#define PCIE4_DEVID		0x157c
#define PCIE4_DEVFN		PCI_DEVFN(PCIE4_DEV, PCIE4_FUNC)
#define SOC_PCIE4_DEV		_SOC_DEV(PCIE4_DEV, PCIE4_FUNC)

/* HD Audio 1 */
#define HDA1_DEV		0x9
#define HDA1_FUNC		2
#define HDA1_DEVID		0x157a
#define HDA1_DEVFN		PCI_DEVFN(HDA1_DEV, HDA1_FUNC)
#define SOC_HDA1_DEV		_SOC_DEV(HDA1_DEV, HDA1_FUNC)

/* HT Configuration */
#define HT_DEV			0x18
#define HT_FUNC			0
#define HT_DEVID		0x15b0
#define HT_DEVFN		PCI_DEVFN(HT_DEV, HT_FUNC)
#define SOC_HT_DEV		_SOC_DEV(HT_DEV, HT_FUNC)

/* Address Maps */
#define ADDR_DEV		0x18
#define ADDR_FUNC		1
#define ADDR_DEVID		0x15b1
#define ADDR_DEVFN		PCI_DEVFN(ADDR_DEV, ADDR_FUNC)
#define SOC_ADDR_DEV		_SOC_DEV(ADDR_DEV, ADDR_FUNC)

/* DRAM Configuration */
#define DCT_DEV			0x18
#define DCT_FUNC		2
#define DCT_DEVID		0x15b2
#define DCT_DEVFN		PCI_DEVFN(DCT_DEV, DCT_FUNC)
#define SOC_DCT_DEV		_SOC_DEV(DCT_DEV, DCT_FUNC)

/* Misc. Configuration */
#define MISC_DEV		0x18
#define MISC_FUNC		3
#define MISC_DEVID		0x15b3
#define MISC_DEVFN		PCI_DEVFN(MISC_DEV, MISC_FUNC)
#define SOC_MISC_DEV		_SOC_DEV(MISC_DEV, MISC_FUNC)

/* PM Configuration */
#define PM_DEV			0x18
#define PM_FUNC			4
#define PM_DEVID		0x15b4
#define PM_DEVFN		PCI_DEVFN(PM_DEV, PM_FUNC)
#define SOC_PM_DEV		_SOC_DEV(PM_DEV, PM_FUNC)

/* Northbridge Configuration */
#define NB_DEV			0x18
#define NB_FUNC			5
#define NB_DEVID		0x15b5
#define NB_DEVFN		PCI_DEVFN(NB_DEV, NB_FUNC)
#define SOC_NB_DEV		_SOC_DEV(NB_DEV, NB_FUNC)

/* USB 3.1 */
#define XHCI0_DEV		0x0
#define XHCI0_FUNC		3
#define XHCI0_DEVID		0x15e0
#define XHCI0_DEVFN		PCI_DEVFN(XHCI0_DEV, XHCI0_FUNC)
#define SOC_XHCI0_DEV		_SOC_DEV(XHCI0_DEV, XHCI0_FUNC)

/* USB 3.1 */
#define XHCI1_DEV		0x0
#define XHCI1_FUNC		4
#define XHCI1_DEVID		0x15e1
#define XHCI1_DEVFN		PCI_DEVFN(XHCI1_DEV, XHCI1_FUNC)
#define SOC_XHCI1_DEV		_SOC_DEV(XHCI1_DEV, XHCI1_FUNC)

/* SATA */
#define SATA_DEV		0x11
#define SATA_FUNC		0
#define SATA_IDE_DEVID		0x7900
#define AHCI_DEVID_MS		0x7901
#define AHCI_DEVID_AMD		0x7904
#define SATA_DEVFN		PCI_DEVFN(SATA_DEV, SATA_FUNC)
#define SOC_SATA_DEV		_SOC_DEV(SATA_DEV, SATA_FUNC)

/* SMBUS */
#define SMBUS_DEV		0x14
#define SMBUS_FUNC		0
#define SMBUS_DEVID		0x790b
#define SMBUS_DEVFN		PCI_DEVFN(SMBUS_DEV, SMBUS_FUNC)
#define SOC_SMBUS_DEV		_SOC_DEV(SMBUS_DEV, SMBUS_FUNC)

/* LPC BUS */
#define PCU_DEV			0x14
#define LPC_FUNC		3
#define LPC_DEVID		0x790e
#define LPC_DEVFN		PCI_DEVFN(PCU_DEV, LPC_FUNC)
#define SOC_LPC_DEV		_SOC_DEV(PCU_DEV, LPC_FUNC)

#endif /* __PI_PICASSO_PCI_DEVS_H__ */
