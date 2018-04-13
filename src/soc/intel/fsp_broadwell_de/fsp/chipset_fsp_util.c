/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
 * Copyright (C) 2015-2016 Intel Corporation. All Rights Reserved.
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
#include <cbfs.h>
#include <cbmem.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <drivers/intel/fsp1_0/fsp_util.h>
#include <soc/pci_devs.h>
#include <soc/reset.h>
#include <soc/romstage.h>
#include <chip.h>
#include <fsp.h>

/* Copy the default UPD region and settings to a buffer for modification */
static void GetUpdDefaultFromFsp (FSP_INFO_HEADER *FspInfo, UPD_DATA_REGION *UpdData)
{
	VPD_DATA_REGION *VpdDataRgnPtr;
	UPD_DATA_REGION *UpdDataRgnPtr;
	VpdDataRgnPtr = (VPD_DATA_REGION *)(UINT32)(FspInfo->CfgRegionOffset  + FspInfo->ImageBase);
	UpdDataRgnPtr = (UPD_DATA_REGION *)(UINT32)(VpdDataRgnPtr->PcdUpdRegionOffset + FspInfo->ImageBase);
	memcpy((void *)UpdData, (void *)UpdDataRgnPtr, sizeof(UPD_DATA_REGION));
}

typedef struct soc_intel_fsp_broadwell_de_config config_t;

/**
 * Update the UPD data based on values from devicetree.cb
 *
 * @param UpdData Pointer to the UPD Data structure
 */
static void ConfigureDefaultUpdData(UPD_DATA_REGION *UpdData)
{
	/*
	 * Serial Port
	 */
	if (IS_ENABLED(CONFIG_INTEGRATED_UART)) {
		UpdData->SerialPortConfigure = 1;
		/* values are from FSP .bsf file */
		if (IS_ENABLED(CONFIG_CONSOLE_SERIAL_9600))
			UpdData->SerialPortBaudRate = 8;
		else if (IS_ENABLED(CONFIG_CONSOLE_SERIAL_19200))
			UpdData->SerialPortBaudRate = 9;
		else if (IS_ENABLED(CONFIG_CONSOLE_SERIAL_38400))
			UpdData->SerialPortBaudRate = 10;
		else if (IS_ENABLED(CONFIG_CONSOLE_SERIAL_57600))
			UpdData->SerialPortBaudRate = 11;
		else if (IS_ENABLED(CONFIG_CONSOLE_SERIAL_115200))
			UpdData->SerialPortBaudRate = 12;
	}

	/*
	 * Memory Down
	 */
	if (IS_ENABLED(CONFIG_FSP_MEMORY_DOWN)) {
		UpdData->MemDownEnable = 1;

		if (IS_ENABLED(CONFIG_FSP_MEMORY_DOWN_CH0DIMM0_SPD_PRESENT))
			UpdData->MemDownCh0Dimm0SpdPtr
			= (UINT32)cbfs_boot_map_with_leak("spd_ch0_dimm0.bin", CBFS_TYPE_SPD, NULL);
		if (IS_ENABLED(CONFIG_FSP_MEMORY_DOWN_CH0DIMM1_SPD_PRESENT))
			UpdData->MemDownCh0Dimm1SpdPtr
			= (UINT32)cbfs_boot_map_with_leak("spd_ch0_dimm1.bin", CBFS_TYPE_SPD, NULL);
		if (IS_ENABLED(CONFIG_FSP_MEMORY_DOWN_CH1DIMM0_SPD_PRESENT))
			UpdData->MemDownCh1Dimm0SpdPtr
			= (UINT32)cbfs_boot_map_with_leak("spd_ch1_dimm0.bin", CBFS_TYPE_SPD, NULL);
		if (IS_ENABLED(CONFIG_FSP_MEMORY_DOWN_CH1DIMM1_SPD_PRESENT))
			UpdData->MemDownCh1Dimm1SpdPtr
			= (UINT32)cbfs_boot_map_with_leak("spd_ch1_dimm1.bin", CBFS_TYPE_SPD, NULL);
	} else {
		UpdData->MemDownEnable = 0;
	}
	printk(FSP_INFO_LEVEL, "Memory Down Support: %s\n",
		UpdData->MemDownEnable ? "Enabled" : "Disabled");

	/*
	 * Fast Boot
	 */
	if (IS_ENABLED(CONFIG_ENABLE_MRC_CACHE))
		UpdData->MemFastBoot = 1;
	else
		UpdData->MemFastBoot = 0;

	/*
	 * Hyper-Threading
	 */
	if (IS_ENABLED(CONFIG_FSP_HYPERTHREADING))
		UpdData->HyperThreading = 1;
	else
		UpdData->HyperThreading = 0;
}

/* Set up the Broadwell-DE specific structures for the call into the FSP */
void chipset_fsp_early_init(FSP_INIT_PARAMS *pFspInitParams, FSP_INFO_HEADER *fsp_ptr)
{
	FSP_INIT_RT_BUFFER *pFspRtBuffer = pFspInitParams->RtBufferPtr;

	/* Initialize the UPD Data */
	GetUpdDefaultFromFsp (fsp_ptr, pFspRtBuffer->Common.UpdDataRgnPtr);
	ConfigureDefaultUpdData(pFspRtBuffer->Common.UpdDataRgnPtr);
	pFspInitParams->NvsBufferPtr = NULL;

#if IS_ENABLED(CONFIG_ENABLE_MRC_CACHE)
	/* Find the fastboot cache that was saved in the ROM */
	pFspInitParams->NvsBufferPtr = find_and_set_fastboot_cache();
#endif

	return;
}

/* The FSP returns here after the fsp_early_init call */
void ChipsetFspReturnPoint(EFI_STATUS Status, VOID *HobListPtr)
{
	*(void **)CBMEM_FSP_HOB_PTR = HobListPtr;

	if (Status == 0xFFFFFFFF) {
		warm_reset();
	}

	romstage_main_continue(Status, HobListPtr);
}
