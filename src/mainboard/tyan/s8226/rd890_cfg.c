/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 - 2012 Advanced Micro Devices, Inc.
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

#include "NbPlatform.h"
#include "rd890_cfg.h"
#include "northbridge/amd/cimx/rd890/chip.h"
#include "nbInitializer.h"
#include <string.h>
#include <arch/ioapic.h>
#include <device/device.h>

extern ROMSTAGE_CONST struct device dev_root;
extern void set_pcie_reset(void *config);
extern void set_pcie_dereset(void *config);

/**
 * @brief Entry point of Northbridge CIMx callout/CallBack
 *
 * prototype AGESA_STATUS (*CALLOUT_ENTRY) (UINT32 Param1, UINTN Param2, VOID* ConfigPtr);
 *
 * @param[in] u32 func               Northbridge CIMx CallBackId
 * @param[in] u32 data               Northbridge Input Data.
 * @param[in] AMD_NB_CONFIG *config  Northbridge configuration structure pointer.
 *
 */
static u32 rd890_callout_entry(u32 func, u32 data, void *config)
{
	u32 ret = 0;
#ifndef __PRE_RAM__
	device_t nb_dev = (device_t)data;
#endif
	AMD_NB_CONFIG *nbConfigPtr = (AMD_NB_CONFIG*)config;

	switch (func) {
		case PHCB_AmdPortTrainingCompleted:
			break;

		case PHCB_AmdPortResetDeassert:
#ifndef __PRE_RAM__
			set_pcie_dereset(config);
#endif
			break;

		case PHCB_AmdPortResetAssert:
#ifndef __PRE_RAM__
			set_pcie_reset(config);
#endif
			break;

		case PHCB_AmdPortResetSupported:
			break;
		case PHCB_AmdGeneratePciReset:
			break;
		case PHCB_AmdGetExclusionTable:
			break;
		case PHCB_AmdAllocateBuffer:
			break;
		case PHCB_AmdUpdateApicInterruptMapping:
			break;
		case PHCB_AmdFreeBuffer:
			break;
		case PHCB_AmdLocateBuffer:
			break;
		case PHCB_AmdReportEvent:
			break;
		case PHCB_AmdPcieAsmpInfo:
			break;

		case CB_AmdSetNbPorConfig:
			break;
		case CB_AmdSetHtConfig:
			/*TODO: different HT path and deempasis for each NB */
			nbConfigPtr->pHtConfig->NbTransmitterDeemphasis = DEFAULT_HT_DEEMPASIES;

			break;
		case CB_AmdSetPcieEarlyConfig:
			break;

		case CB_AmdSetEarlyPostConfig:
			break;

		case CB_AmdSetMidPostConfig:
			nbConfigPtr->pNbConfig->IoApicBaseAddress = IO_APIC_ADDR;
#ifndef IOMMU_SUPPORT_DISABLE //TODO enable iommu
			/* SBIOS must alloc 16K memory for IOMMU MMIO */
			UINT32  MmcfgBarAddress; //using default IOmmuBaseAddress
			LibNbPciRead(nbConfigPtr->NbPciAddress.AddressValue | 0x1C,
					AccessWidth32,
					&MmcfgBarAddress,
					nbConfigPtr);
			MmcfgBarAddress &= ~0xf;
			if (MmcfgBarAddress != 0) {
				nbConfigPtr->IommuBaseAddress = MmcfgBarAddress;
			}
			nbConfigPtr->IommuBaseAddress = 0; //disable iommu
#endif
			break;

		case CB_AmdSetLatePostConfig:
			break;

		case CB_AmdSetRecoveryConfig:
			break;
	}

	return ret;
}


/**
 * @brief North Bridge CIMx configuration
 *
 * should be called before exeucte CIMx function.
 * this function will be called in romstage and ramstage.
 */
void rd890_cimx_config(AMD_NB_CONFIG_BLOCK *pConfig, NB_CONFIG *nbConfig, HT_CONFIG *htConfig, PCIE_CONFIG *pcieConfig)
{
	u16 i = 0;
	PCI_ADDR PciAddress;
	u32 val, sbNode, sbLink;

	if (!pConfig) {
		return;
	}

	memset(pConfig, 0, sizeof(AMD_NB_CONFIG_BLOCK));
	for (i = 0; i < MAX_NB_COUNT; i++) {
		pConfig->Northbridges[i].pNbConfig	= &nbConfig[i];
		pConfig->Northbridges[i].pHtConfig	= &htConfig[i];
		pConfig->Northbridges[i].pPcieConfig	= &pcieConfig[i];
		pConfig->Northbridges[i].ConfigPtr	= &pConfig;
	}

	/* Initialize all NB structures */
	AmdInitializer(pConfig);

	pConfig->NumberOfNorthbridges = MAX_NB_COUNT - 1; /* Support limited to primary NB only located at 0:0:0 */
	//pConfig->StandardHeader.ImageBasePtr = CIMX_B2_IMAGE_BASE_ADDRESS;
	pConfig->StandardHeader.PcieBasePtr = (VOID *)PCIEX_BASE_ADDRESS;
	pConfig->StandardHeader.CalloutPtr = &rd890_callout_entry;

	/*
	 * PCI Address to Access NB. Depends on HT topology and configuration for multi NB platform.
	 * Always 0:0:0 on single NB platform.
	 */
	pConfig->Northbridges[0].NbPciAddress.AddressValue = MAKE_SBDFO(0, 0x0, 0x0, 0x0, 0x0);

	/* Set HT path to NB by SbNode and SbLink */
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB, FUNC_0, 0x60);
	LibNbPciRead(PciAddress.AddressValue, AccessWidth32, &val, &(pConfig->Northbridges[0]));
	sbNode = (val >> 8) & 0x07;
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB, FUNC_0, 0x64);
	LibNbPciRead(PciAddress.AddressValue, AccessWidth32, &val, &(pConfig->Northbridges[0]));
	sbLink = (val >> 8) & 0x07; //assum ganged
	pConfig->Northbridges[0].NbHtPath.NodeID = sbNode;
	pConfig->Northbridges[0].NbHtPath.LinkID = sbLink;
	//TODO: other NBs
	pConfig->Northbridges[0].pNbConfig->IoApicBaseAddress = IO_APIC_ADDR;

#ifndef __PRE_RAM__
	/* If temporrary MMIO enable set up CPU MMIO */
	for (i = 0; i <= pConfig->NumberOfNorthbridges; i++) {
		UINT32  MmioBase;
		UINT32  LinkId;
		UINT32  SubLinkId;
		MmioBase = pConfig->Northbridges[i].pPcieConfig->TempMmioBaseAddress;
		if (MmioBase != 0) {
			LinkId = pConfig->Northbridges[i].NbHtPath.LinkID & 0xf;
			SubLinkId = ((pConfig->Northbridges[i].NbHtPath.LinkID & 0xF0) == 0x20) ? 1 : 0;
			/* Set Limit */
			LibNbPciRMW(MAKE_SBDFO (0, 0, 0x18, 0x1, (i * 4) + 0x84),
					AccessWidth32,
					0x0,
					((MmioBase << 12) + 0xF00) | (LinkId << 4) | (SubLinkId << 6),
					&(pConfig->Northbridges[i]));
			/* Set Base */
			LibNbPciRMW(MAKE_SBDFO (0, 0, 0x18, 0x1, (i * 4) + 0x80),
					AccessWidth32,
					0x0,
					(MmioBase << 12) | 0x3,
					&(pConfig->Northbridges[i]));
		}
	}
#endif
}

