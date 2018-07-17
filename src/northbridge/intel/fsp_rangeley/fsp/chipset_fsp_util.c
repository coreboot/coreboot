/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
 * Copyright (C) 2015 Intel Corporation
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

#include <types.h>
#include <string.h>
#include <console/console.h>
#include <bootstate.h>
#include <cbmem.h>
#include <device/device.h>
#include <southbridge/intel/fsp_rangeley/pci_devs.h>
#include <drivers/intel/fsp1_0/fsp_util.h>
#include <fspvpd.h>
#include <fspbootmode.h>
#include <reset.h>
#include "../chip.h"

#ifdef __PRE_RAM__
#include <southbridge/intel/fsp_rangeley/romstage.h>
#endif

#ifdef __PRE_RAM__

/* Copy the default UPD region and settings to a buffer for modification */
static void GetUpdDefaultFromFsp
	(FSP_INFO_HEADER *FspInfo, UPD_DATA_REGION *UpdData)
{
	VPD_DATA_REGION *VpdDataRgnPtr;
	UPD_DATA_REGION *UpdDataRgnPtr;
	VpdDataRgnPtr = (VPD_DATA_REGION *)(UINT32)(FspInfo->CfgRegionOffset
			+ FspInfo->ImageBase);
	UpdDataRgnPtr = (UPD_DATA_REGION *)(UINT32)
			(VpdDataRgnPtr->PcdUpdRegionOffset + FspInfo->ImageBase);
	memcpy((void *)UpdData, (void *)UpdDataRgnPtr, sizeof(UPD_DATA_REGION));
}

typedef struct northbridge_intel_fsp_rangeley_config config_t;

/**
 * Update the UPD data based on values from devicetree.cb
 *
 * @param UpdData Pointer to the UPD Data structure
 */
static void ConfigureDefaultUpdData(UPD_DATA_REGION *UpdData)
{
	DEVTREE_CONST struct device *dev;
	DEVTREE_CONST config_t *config;
	printk(BIOS_DEBUG, "Configure Default UPD Data\n");

	dev = dev_find_slot(0, SOC_DEV_FUNC);
	config = dev->chip_info;

	/* Set SPD addresses */
	if (config->SpdBaseAddress_0_0) {
		UpdData->PcdSpdBaseAddress_0_0 = config->SpdBaseAddress_0_0;
	}
	if (config->SpdBaseAddress_0_1) {
		UpdData->PcdSpdBaseAddress_0_1 = config->SpdBaseAddress_0_1;
	}
	if (config->SpdBaseAddress_1_0) {
		UpdData->PcdSpdBaseAddress_1_0 = config->SpdBaseAddress_1_0;
	}
	if (config->SpdBaseAddress_1_1) {
		UpdData->PcdSpdBaseAddress_1_1 = config->SpdBaseAddress_1_1;
	}
	if (config->EccSupport) {
		UpdData->PcdEccSupport = config->EccSupport;
	}
	if (config->PrintDebugMessages) {
		UpdData->PcdPrintDebugMessages = config->PrintDebugMessages;
	}
	if (config->Bifurcation) {
		UpdData->PcdBifurcation = config->Bifurcation;
	}
	if (config->MemoryDown) {
		UpdData->PcdMemoryDown = config->MemoryDown;
	}

	UpdData->PcdMrcInitTsegSize = CONFIG_SMM_TSEG_SIZE >> 20;

	if (config->MrcRmtCpgcExpLoopCntValue) {
		UpdData->PcdMrcRmtCpgcExpLoopCntValue =
				config->MrcRmtCpgcExpLoopCntValue;
	}
	if (config->MrcRmtCpgcNumBursts) {
		UpdData->PcdMrcRmtCpgcNumBursts = config->MrcRmtCpgcNumBursts;
	}
#if IS_ENABLED(CONFIG_ENABLE_FSP_FAST_BOOT)
	UpdData->PcdFastboot = UPD_ENABLE;
#endif
	/*
	 * Loop through all the SOC devices in the devicetree
	 *  enabling and disabling them as requested.
	 */
	for (; dev; dev = dev->sibling) {

		if (dev->path.type != DEVICE_PATH_PCI)
			continue;

		switch (dev->path.pci.devfn) {
			case GBE1_DEV_FUNC:
			case GBE2_DEV_FUNC:
			case GBE3_DEV_FUNC:
			case GBE4_DEV_FUNC:
				UpdData->PcdEnableLan |= dev->enabled;
				printk(BIOS_DEBUG, "PcdEnableLan %d\n",
						UpdData->PcdEnableLan);
				break;
			case SATA2_DEV_FUNC:
				UpdData->PcdEnableSata2 = dev->enabled;
				printk(BIOS_DEBUG, "PcdEnableSata2 %d\n",
						UpdData->PcdEnableSata2);
				break;
			case SATA3_DEV_FUNC:
				UpdData->PcdEnableSata3 = dev->enabled;
				printk(BIOS_DEBUG, "PcdEnableSata3 %d\n",
						UpdData->PcdEnableSata3);
				break;
			case IQAT_DEV_FUNC:
				UpdData->PcdEnableIQAT |= dev->enabled;
				printk(BIOS_DEBUG, "PcdEnableIQAT %d\n",
					UpdData->PcdEnableIQAT);
				break;
			case USB2_DEV_FUNC:
				UpdData->PcdEnableUsb20 = dev->enabled;
				printk(BIOS_DEBUG, "PcdEnableUsb20 %d\n",
						UpdData->PcdEnableUsb20);
				break;
		}
	}

	/* Set PCIe de-emphasis */
	UPD_DEFAULT_CHECK(PcdPcieRootPort1DeEmphasis);
	UPD_DEFAULT_CHECK(PcdPcieRootPort2DeEmphasis);
	UPD_DEFAULT_CHECK(PcdPcieRootPort3DeEmphasis);
	UPD_DEFAULT_CHECK(PcdPcieRootPort4DeEmphasis);
}

/* Set up the Rangeley specific structures for the call into the FSP */
void chipset_fsp_early_init(FSP_INIT_PARAMS *pFspInitParams,
		FSP_INFO_HEADER *fsp_ptr)
{
	FSP_INIT_RT_BUFFER *pFspRtBuffer = pFspInitParams->RtBufferPtr;

	/* Initialize the UPD Data */
	GetUpdDefaultFromFsp (fsp_ptr, pFspRtBuffer->Common.UpdDataRgnPtr);
	ConfigureDefaultUpdData(pFspRtBuffer->Common.UpdDataRgnPtr);
	pFspInitParams->NvsBufferPtr = NULL;
	pFspRtBuffer->Common.BootMode = BOOT_WITH_FULL_CONFIGURATION;

	/* Find the fastboot cache that was saved in the ROM */
	pFspInitParams->NvsBufferPtr = find_and_set_fastboot_cache();

	return;
}

/* The FSP returns here after the fsp_early_init call */
void ChipsetFspReturnPoint(EFI_STATUS Status,
		VOID *HobListPtr)
{
	*(void **)CBMEM_FSP_HOB_PTR = HobListPtr;

	if (Status == 0xFFFFFFFF) {
		soft_reset();
	}
	romstage_main_continue(Status, HobListPtr);
}

#endif	/* __PRE_RAM__ */
