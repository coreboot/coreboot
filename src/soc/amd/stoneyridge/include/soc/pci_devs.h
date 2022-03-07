/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_STONEYRIDGE_PCI_DEVS_H
#define AMD_STONEYRIDGE_PCI_DEVS_H

#include <device/pci_def.h>
#include <amdblocks/pci_devs.h>

/* GNB Root Complex: GNB_DEVID 0x1576 */
#define GNB_DEV			0x0
#define GNB_FUNC		0
#define GNB_DEVFN		PCI_DEVFN(GNB_DEV, GNB_FUNC)
#define SOC_GNB_DEV		_SOC_DEV(GNB_DEV, GNB_FUNC)

/* IOMMU: IOMMU_DEVID 0x1577 */
#define IOMMU_DEV		0x0
#define IOMMU_FUNC		2
#define IOMMU_DEVFN		PCI_DEVFN(IOMMU_DEV, IOMMU_FUNC)
#define SOC_IOMMU_DEV		_SOC_DEV(IOMMU_DEV, IOMMU_FUNC)

/*
 * Internal Graphics
 * Device IDs subject to SKU/OPN variation
 * GFX_DEVID for merlinfalcon		PCI_DID_AMD_15H_MODEL_606F_GFX
 * GFX_DEVID for stoneyridge		PCI_DID_AMD_15H_MODEL_707F_GFX
 */
#define GFX_DEV			0x1
#define GFX_FUNC		0
#define GFX_DEVFN		PCI_DEVFN(GFX_DEV, GFX_FUNC)
#define SOC_GFX_DEV		_SOC_DEV(GFX_DEV, GFX_FUNC)

/* HD Audio 0
 * Device IDs
 * HDA0_DEVID			PCI_DID_AMD_15H_MODEL_606F_HDA
 * HDA0_DEVID			PCI_DID_AMD_15H_MODEL_707F_HDA
 */
#define HDA0_DEV		0x1
#define HDA0_FUNC		1
#define HDA0_DEVFN		PCI_DEVFN(HDA0_DEV, HDA0_FUNC)
#define SOC_HDA0_DEV		_SOC_DEV(HDA0_DEV, HDA0_FUNC)

/* Host Bridge: HOST_DEVID 0x157b */
#define HOST_DEV		0x2
#define HOST_FUNC		0
#define HOST_DEVFN		PCI_DEVFN(HOST_DEV, HOST_FUNC)
#define SOC_HOST_DEV		_SOC_DEV(HOST_DEV, HOST_FUNC)

/* PCIe GPP Bridge 0: PCIE0_DEVID 0x157c */
#define PCIE0_DEV		0x2
#define PCIE0_FUNC		1
#define PCIE0_DEVFN		PCI_DEVFN(PCIE0_DEV, PCIE0_FUNC)
#define SOC_PCIE0_DEV		_SOC_DEV(PCIE0_DEV, PCIE0_FUNC)

/* PCIe GPP Bridge 1: PCIE1_DEVID 0x157c */
#define PCIE1_DEV		0x2
#define PCIE1_FUNC		2
#define PCIE1_DEVFN		PCI_DEVFN(PCIE1_DEV, PCIE1_FUNC)
#define SOC_PCIE1_DEV		_SOC_DEV(PCIE1_DEV, PCIE1_FUNC)

/* PCIe GPP Bridge 2: PCIE2_DEVID 0x157c */
#define PCIE2_DEV		0x2
#define PCIE2_FUNC		3
#define PCIE2_DEVFN		PCI_DEVFN(PCIE2_DEV, PCIE2_FUNC)
#define SOC_PCIE2_DEV		_SOC_DEV(PCIE2_DEV, PCIE2_FUNC)

/* PCIe GPP Bridge 3: PCIE3_DEVID 0x157c */
#define PCIE3_DEV		0x2
#define PCIE3_FUNC		4
#define PCIE3_DEVFN		PCI_DEVFN(PCIE3_DEV, PCIE3_FUNC)
#define SOC_PCIE3_DEV		_SOC_DEV(PCIE3_DEV, PCIE3_FUNC)

/* PCIe GPP Bridge 4: PCIE4_DEVID 0x157c */
#define PCIE4_DEV		0x2
#define PCIE4_FUNC		5
#define PCIE4_DEVFN		PCI_DEVFN(PCIE4_DEV, PCIE4_FUNC)
#define SOC_PCIE4_DEV		_SOC_DEV(PCIE4_DEV, PCIE4_FUNC)

/* Platform Security Processor: PSP_DEVID 0x1578 */
#define PSP_DEV			0x8
#define PSP_FUNC		0
#define PSP_DEVFN		PCI_DEVFN(PSP_DEV, PSP_FUNC)
#define SOC_PSP_DEV		_SOC_DEV(PSP_DEV, PSP_FUNC)

/* HD Audio 1: HDA1_DEVID 0x157a */
#define HDA1_DEV		0x9
#define HDA1_FUNC		2
#define HDA1_DEVFN		PCI_DEVFN(HDA1_DEV, HDA1_FUNC)
#define SOC_HDA1_DEV		_SOC_DEV(HDA1_DEV, HDA1_FUNC)

/* HT Configuration
 * Device IDs
 * HT_DEVID for merlinfalcon	PCI_DID_AMD_15H_MODEL_606F_NB_HT
 * HT_DEVID for stoneyridge	PCI_DID_AMD_15H_MODEL_707F_NB_HT
 */
#define HT_DEV			0x18
#define HT_FUNC			0
#define HT_DEVFN		PCI_DEVFN(HT_DEV, HT_FUNC)
#define SOC_HT_DEV		_SOC_DEV(HT_DEV, HT_FUNC)

/* Address Maps
 * Device IDs
 * ADDR_DEVID for merlinfalcon	0x1571
 * ADDR_DEVID for stoneyridge	0x15b1
 */
#define ADDR_DEV		0x18
#define ADDR_FUNC		1
#define ADDR_DEVFN		PCI_DEVFN(ADDR_DEV, ADDR_FUNC)
#define SOC_ADDR_DEV		_SOC_DEV(ADDR_DEV, ADDR_FUNC)

/* DRAM Configuration
 * Device IDs
 * DCT_DEVID for merlinfalcon	0x1572
 * DCT_DEVID for stoneyridge	0x15b2
 */
#define DCT_DEV			0x18
#define DCT_FUNC		2
#define DCT_DEVFN		PCI_DEVFN(DCT_DEV, DCT_FUNC)
#define SOC_DCT_DEV		_SOC_DEV(DCT_DEV, DCT_FUNC)

/* Misc. Configuration
 * Device IDs
 * MISC_DEVID for merlinfalcon	0x1573
 * MISC_DEVID for stoneyridge	0x15b3
 */
#define MISC_DEV		0x18
#define MISC_FUNC		3
#define MISC_DEVFN		PCI_DEVFN(MISC_DEV, MISC_FUNC)
#define SOC_MISC_DEV		_SOC_DEV(MISC_DEV, MISC_FUNC)

/* PM Configuration
 * Device IDs
 * PM_DEVID for merlinfalcon	0x1574
 * PM_DEVID for stoneyridge	0x15b4
 */
#define PM_DEV			0x18
#define PM_FUNC			4
#define PM_DEVFN		PCI_DEVFN(PM_DEV, PM_FUNC)
#define SOC_PM_DEV		_SOC_DEV(PM_DEV, PM_FUNC)

/* Northbridge Configuration
 * Device IDs
 * NB_DEVID for merlinfalcon	0x1575
 * NB_DEVID for stoneyridge	0x15b5
 */
#define NB_DEV			0x18
#define NB_FUNC			5
#define NB_DEVFN		PCI_DEVFN(NB_DEV, NB_FUNC)
#define SOC_NB_DEV		_SOC_DEV(NB_DEV, NB_FUNC)

/* XHCI: XHCI_DEVID 0x7914 */
#define XHCI_DEV		0x10
#define XHCI_FUNC		0
#define XHCI_DEVFN		PCI_DEVFN(XHCI_DEV, XHCI_FUNC)
#define SOC_XHCI_DEV		_SOC_DEV(XHCI_DEV, XHCI_FUNC)

/*
 * SATA:
 * SATA_IDE_IDEVID		0x7900
 * AHCI_DEVID_MS		0x7901
 * AHCI_DEVID_AMD		0x7904
 */
#define SATA_DEV		0x11
#define SATA_FUNC		0
#define SATA_DEVFN		PCI_DEVFN(SATA_DEV, SATA_FUNC)
#define SOC_SATA_DEV		_SOC_DEV(SATA_DEV, SATA_FUNC)

/* EHCI: EHCI_DEVID 0x7908 */
#define EHCI_DEV		0x12
#define EHCI_FUNC		0
#define EHCI1_DEVFN		PCI_DEVFN(EHCI_DEV, EHCI_FUNC)
#define SOC_EHCI1_DEV		_SOC_DEV(EHCI_DEV, EHCI_FUNC)

/* SMBUS: SMBUS_DEVID 0x790b */
#define SMBUS_DEV		0x14
#define SMBUS_FUNC		0
#define SMBUS_DEVFN		PCI_DEVFN(SMBUS_DEV, SMBUS_FUNC)
#define SOC_SMBUS_DEV		_SOC_DEV(SMBUS_DEV, SMBUS_FUNC)

/* LPC BUS: LPC_DEVID 0x790e */
#define PCU_DEV			0x14
#define LPC_FUNC		3
#define LPC_DEVFN		PCI_DEVFN(PCU_DEV, LPC_FUNC)
#define SOC_LPC_DEV		_SOC_DEV(PCU_DEV, LPC_FUNC)

/* SD Controller: SD_DEVID 0x7906 */
#define SD_DEV			0x14
#define SD_FUNC			7
#define SD_DEVFN		PCI_DEVFN(SD_DEV, SD_FUNC)
#define SOC_SD_DEV		_SOC_DEV(SD_DEV, SD_FUNC)

#endif /* AMD_STONEYRIDGE_PCI_DEVS_H */
