/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
 * Copyright (C) 2014-2015 Intel Corporation
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
#include <device/pci_def.h>
#include <soc/pci_devs.h>
#include <drivers/intel/fsp1_0/fsp_util.h>
#include "../chip.h"
#include <arch/io.h>
#include <soc/reset.h>
#include <soc/pmc.h>
#include <soc/acpi.h>
#include <soc/iomap.h>
#include <soc/smm.h>

#ifdef __PRE_RAM__
#include <soc/romstage.h>
#endif

#ifdef __PRE_RAM__

/* Copy the default UPD region and settings to a buffer for modification */
static void GetUpdDefaultFromFsp (FSP_INFO_HEADER *FspInfo, UPD_DATA_REGION   *UpdData)
{
	VPD_DATA_REGION *VpdDataRgnPtr;
	UPD_DATA_REGION *UpdDataRgnPtr;
	VpdDataRgnPtr = (VPD_DATA_REGION *)(UINT32)(FspInfo->CfgRegionOffset  + FspInfo->ImageBase);
	UpdDataRgnPtr = (UPD_DATA_REGION *)(UINT32)(VpdDataRgnPtr->PcdUpdRegionOffset + FspInfo->ImageBase);
	memcpy((void *)UpdData, (void *)UpdDataRgnPtr, sizeof(UPD_DATA_REGION));
}

/* default to just enabling HDMI audio */
const PCH_AZALIA_CONFIG mAzaliaConfig = {
	.Pme = 1,
	.DS = 1,
	.DA = 0,
	.HdmiCodec = 1,
	.AzaliaVCi = 1,
	.Rsvdbits = 0,
	.AzaliaVerbTableNum = 0,
	.AzaliaVerbTable = NULL,
	.ResetWaitTimer = 300
};

typedef struct soc_intel_fsp_baytrail_config config_t;

static const char *acpi_pci_mode_strings[] = {
	"Disabled",
	"Enabled in PCI Mode",
	"Enabled in ACPI Mode"
};

static const char *emmc_mode_strings[] = {
	"Disabled",
	"Auto",
	"eMMC 4.1",
	"eMMC 4.5"
};

/**
 * Update the UPD data based on values from devicetree.cb
 *
 * @param UpdData Pointer to the UPD Data structure
 */
static void ConfigureDefaultUpdData(FSP_INFO_HEADER *FspInfo, UPD_DATA_REGION *UpdData)
{
	DEVTREE_CONST struct device *dev;
	DEVTREE_CONST config_t *config;
	printk(FSP_INFO_LEVEL, "Configure Default UPD Data\n");

	dev = dev_find_slot(0, SOC_DEV_FUNC);
	config = dev->chip_info;

	/* Set up default verb tables - Just HDMI audio */
	UpdData->AzaliaConfigPtr = (UINT32)&mAzaliaConfig;

	/* Set SPD addresses */
	UPD_SPD_CHECK(PcdMrcInitSPDAddr1);
	UPD_SPD_CHECK(PcdMrcInitSPDAddr2);

	UPD_DEFAULT_CHECK(PcdSataMode);
	UPD_DEFAULT_CHECK(PcdLpssSioEnablePciMode);
	UPD_DEFAULT_CHECK(PcdMrcInitMmioSize);
	UPD_DEFAULT_CHECK(PcdIgdDvmt50PreAlloc);
	UPD_DEFAULT_CHECK(PcdApertureSize);
	UPD_DEFAULT_CHECK(PcdGttSize);
	UPD_DEFAULT_CHECK(SerialDebugPortAddress);
	UPD_DEFAULT_CHECK(SerialDebugPortType);
	UPD_DEFAULT_CHECK(PcdMrcDebugMsg);
	UPD_DEFAULT_CHECK(PcdSccEnablePciMode);
	UPD_DEFAULT_CHECK(IgdRenderStandby);
	UPD_DEFAULT_CHECK(TxeUmaEnable);
	UPD_DEFAULT_CHECK(PcdOsSelection);
	UPD_DEFAULT_CHECK(PcdEMMC45DDR50Enabled);
	UPD_DEFAULT_CHECK(PcdEMMC45HS200Enabled);
	UPD_DEFAULT_CHECK(PcdEMMC45RetuneTimerValue);
	UPD_DEFAULT_CHECK(PcdEnableIgd);
	UPD_DEFAULT_CHECK(AutoSelfRefreshEnable);
	UPD_DEFAULT_CHECK(APTaskTimeoutCnt);

	if (config->PcdeMMCBootMode == EMMC_FOLLOWS_DEVICETREE)
		UpdData->PcdeMMCBootMode = 0;
	else if ((config->PcdeMMCBootMode != EMMC_USE_DEFAULT))
		UpdData->PcdeMMCBootMode = config->PcdeMMCBootMode - EMMC_DISABLED;

	UpdData->PcdMrcInitTsegSize = smm_region_size() >> 20;

	printk(FSP_INFO_LEVEL, "GTT Size:\t\t%d MB\n", UpdData->PcdGttSize);
	printk(FSP_INFO_LEVEL, "Tseg Size:\t\t%d MB\n", UpdData->PcdMrcInitTsegSize);
	printk(FSP_INFO_LEVEL, "Aperture Size:\t\t%d MB\n",
		APERTURE_SIZE_BASE << UpdData->PcdApertureSize);
	printk(FSP_INFO_LEVEL, "IGD Memory Size:\t%d MB\n",
		UpdData->PcdIgdDvmt50PreAlloc * IGD_MEMSIZE_MULTIPLIER);
	printk(FSP_INFO_LEVEL, "MMIO Size:\t\t%d MB\n", UpdData->PcdMrcInitMmioSize);

	/* Advance dev to PCI device 0.0 */
	for (dev = &dev_root; dev; dev = dev_find_next_pci_device(dev)){
		if (dev->path.type != DEVICE_PATH_PCI)
			continue;
		if (dev->path.pci.devfn == PCI_DEVFN(0x0,0))
			break;
	}

	/*
	 * Loop through all the SOC devices in the devicetree
	 *  enabling and disabling them as requested.
	 */
	for (; dev; dev = dev->sibling) {

		if (dev->path.type != DEVICE_PATH_PCI)
			continue;

		switch (dev->path.pci.devfn) {
			UPD_DEVICE_CHECK(SDIO_DEV_FUNC, PcdEnableSdio, "Sdio:\t\t\t");
			UPD_DEVICE_CHECK(SD_DEV_FUNC, PcdEnableSdcard, "Sdcard:\t\t\t");
			UPD_DEVICE_CHECK(SIO_DMA1_DEV_FUNC, PcdEnableDma0, "SIO Dma 0:\t\t");
			UPD_DEVICE_CHECK(I2C1_DEV_FUNC, PcdEnableI2C0, "SIO I2C0:\t\t");
			UPD_DEVICE_CHECK(I2C2_DEV_FUNC, PcdEnableI2C1, "SIO I2C1:\t\t");
			UPD_DEVICE_CHECK(I2C3_DEV_FUNC, PcdEnableI2C2, "SIO I2C2:\t\t");
			UPD_DEVICE_CHECK(I2C4_DEV_FUNC, PcdEnableI2C3, "SIO I2C3:\t\t");
			UPD_DEVICE_CHECK(I2C5_DEV_FUNC, PcdEnableI2C4, "SIO I2C4:\t\t");
			UPD_DEVICE_CHECK(I2C6_DEV_FUNC, PcdEnableI2C5, "SIO I2C5:\t\t");
			UPD_DEVICE_CHECK(I2C7_DEV_FUNC, PcdEnableI2C6, "SIO I2C6:\t\t");
			UPD_DEVICE_CHECK(SIO_DMA2_DEV_FUNC, PcdEnableDma1, "SIO Dma1:\t\t");
			UPD_DEVICE_CHECK(PWM1_DEV_FUNC, PcdEnablePwm0, "Pwm0:\t\t\t");
			UPD_DEVICE_CHECK(PWM2_DEV_FUNC, PcdEnablePwm1, "Pwm1:\t\t\t");
			UPD_DEVICE_CHECK(HSUART1_DEV_FUNC, PcdEnableHsuart0, "Hsuart0:\t\t");
			UPD_DEVICE_CHECK(HSUART2_DEV_FUNC, PcdEnableHsuart1, "Hsuart1:\t\t");
			UPD_DEVICE_CHECK(SPI_DEV_FUNC, PcdEnableSpi, "Spi:\t\t\t");
			UPD_DEVICE_CHECK(SATA_DEV_FUNC, PcdEnableSata, "SATA:\t\t\t");
			UPD_DEVICE_CHECK(HDA_DEV_FUNC, PcdEnableAzalia, "Azalia:\t\t\t");

			case MIPI_DEV_FUNC:	/* Camera / Image Signal Processing */
				if (FspInfo->ImageRevision >= FSP_GOLD3_REV_ID) {
					UpdData->ISPEnable = dev->enabled;
				} else {
					/* Gold2 and earlier FSP: ISPEnable is the field	*/
					/* next to PcdGttSize in UPD_DATA_REGION struct		*/
					*(&(UpdData->PcdGttSize)+sizeof(UINT8)) = dev->enabled;
					printk (FSP_INFO_LEVEL,
						"Baytrail Gold2 or earlier FSP, adjust ISPEnable offset.\n");
				}
				printk(FSP_INFO_LEVEL, "MIPI/ISP:\t\t%s\n",
						dev->enabled?"Enabled":"Disabled");
				break;
			case EMMC_DEV_FUNC: /* EMMC 4.1*/
				if ((dev->enabled) &&
						(config->PcdeMMCBootMode == EMMC_FOLLOWS_DEVICETREE))
					UpdData->PcdeMMCBootMode = EMMC_4_1 - EMMC_DISABLED;
				break;
			case MMC45_DEV_FUNC: /* MMC 4.5*/
				if ((dev->enabled) &&
						(config->PcdeMMCBootMode == EMMC_FOLLOWS_DEVICETREE))
					UpdData->PcdeMMCBootMode = EMMC_4_5 - EMMC_DISABLED;
				break;
			case XHCI_DEV_FUNC:
				UpdData->PcdEnableXhci = dev->enabled;
				break;
			case EHCI_DEV_FUNC:
				UpdData->PcdEnableXhci = !(dev->enabled);
				break;

			case LPE_DEV_FUNC:
				if (dev->enabled)
					UpdData->PcdEnableLpe = config->LpeAcpiModeEnable;
				else
					UpdData->PcdEnableLpe = 0;
				break;
		}
	}

	if (UpdData->PcdEnableLpe < sizeof(acpi_pci_mode_strings) / sizeof (char *))
		printk(FSP_INFO_LEVEL, "Lpe:\t\t\t%s\n",
			acpi_pci_mode_strings[UpdData->PcdEnableLpe]);
	else
		printk(FSP_INFO_LEVEL, "Lpe:\t\t\tUnknown (0x%02x)\n",
			UpdData->PcdEnableLpe);

	if (UpdData->PcdeMMCBootMode < sizeof(emmc_mode_strings) / sizeof (char *))
		printk(FSP_INFO_LEVEL, "eMMC Mode:\t\t%s\n",
			emmc_mode_strings[UpdData->PcdeMMCBootMode]);
	else
		printk(FSP_INFO_LEVEL, "eMMC Mode:\t\tUnknown (0x%02x)\n",
			UpdData->PcdeMMCBootMode);


	if (UpdData->PcdEnableSata)
		printk(FSP_INFO_LEVEL, "SATA Mode:\t\t%s\n",
			UpdData->PcdSataMode?"AHCI":"IDE");

	printk(FSP_INFO_LEVEL, "Xhci:\t\t\t%s\n",
		UpdData->PcdEnableXhci?"Enabled":"Disabled");

	/*
	 * set memory down parameters
	 * Skip setting values if memory down is disabled
	 * Skip setting values if FSP is earlier than gold 3
	 */
	if (FspInfo->ImageRevision >= FSP_GOLD3_REV_ID) {
		UPD_MEMDOWN_CHECK(EnableMemoryDown, DECREMENT_FOR_DEFAULT);
		if (UpdData->PcdMemoryParameters.EnableMemoryDown) {
			UPD_MEMDOWN_CHECK(DRAMSpeed,    DECREMENT_FOR_DEFAULT);
			UPD_MEMDOWN_CHECK(DRAMType,     DECREMENT_FOR_DEFAULT);
			UPD_MEMDOWN_CHECK(DIMM0Enable,  DECREMENT_FOR_DEFAULT);
			UPD_MEMDOWN_CHECK(DIMM1Enable,  DECREMENT_FOR_DEFAULT);
			UPD_MEMDOWN_CHECK(DIMMDWidth,   DECREMENT_FOR_DEFAULT);
			UPD_MEMDOWN_CHECK(DIMMDensity,  DECREMENT_FOR_DEFAULT);
			UPD_MEMDOWN_CHECK(DIMMBusWidth, DECREMENT_FOR_DEFAULT);
			UPD_MEMDOWN_CHECK(DIMMSides,    DECREMENT_FOR_DEFAULT);
			UPD_MEMDOWN_CHECK(DIMMtCL,      NO_DECREMENT_FOR_DEFAULT);
			UPD_MEMDOWN_CHECK(DIMMtRPtRCD,  NO_DECREMENT_FOR_DEFAULT);
			UPD_MEMDOWN_CHECK(DIMMtWR,      NO_DECREMENT_FOR_DEFAULT);
			UPD_MEMDOWN_CHECK(DIMMtWTR,     NO_DECREMENT_FOR_DEFAULT);
			UPD_MEMDOWN_CHECK(DIMMtRRD,     NO_DECREMENT_FOR_DEFAULT);
			UPD_MEMDOWN_CHECK(DIMMtRTP,     NO_DECREMENT_FOR_DEFAULT);
			UPD_MEMDOWN_CHECK(DIMMtFAW,     NO_DECREMENT_FOR_DEFAULT);

			printk (FSP_INFO_LEVEL,
				"Memory Down Data Existed : %s\n"\
				"- Speed (0: 800, 1: 1066, 2: 1333, 3: 1600): %d\n"\
				"- Type  (0: DDR3, 1: DDR3L) : %d\n"\
				"- DIMM0        : %s\n"\
				"- DIMM1        : %s\n"\
				"- Width        : x%d\n"\
				"- Density      : %dGbit\n"
				"- BudWidth     : %dbit\n"\
				"- Rank #       : %d\n"\
				"- tCL          : %02X\n"\
				"- tRPtRCD      : %02X\n"\
				"- tWR          : %02X\n"\
				"- tWTR         : %02X\n"\
				"- tRRD         : %02X\n"\
				"- tRTP         : %02X\n"\
				"- tFAW         : %02X\n"
				, (UpdData->PcdMemoryParameters.EnableMemoryDown) ? "Enabled" : "Disabled"
				, UpdData->PcdMemoryParameters.DRAMSpeed
				, UpdData->PcdMemoryParameters.DRAMType
				, (UpdData->PcdMemoryParameters.DIMM0Enable) ? "Enabled" : "Disabled"
				, (UpdData->PcdMemoryParameters.DIMM1Enable) ? "Enabled" : "Disabled"
				, 8 << (UpdData->PcdMemoryParameters.DIMMDWidth)
				, 1 << (UpdData->PcdMemoryParameters.DIMMDensity)
				, 8 << (UpdData->PcdMemoryParameters.DIMMBusWidth)
				, (UpdData->PcdMemoryParameters.DIMMSides) + 1
				, UpdData->PcdMemoryParameters.DIMMtCL
				, UpdData->PcdMemoryParameters.DIMMtRPtRCD
				, UpdData->PcdMemoryParameters.DIMMtWR
				, UpdData->PcdMemoryParameters.DIMMtWTR
				, UpdData->PcdMemoryParameters.DIMMtRRD
				, UpdData->PcdMemoryParameters.DIMMtRTP
				, UpdData->PcdMemoryParameters.DIMMtFAW
			);
		}
	}
}

/* Set up the Baytrail specific structures for the call into the FSP */
void chipset_fsp_early_init(FSP_INIT_PARAMS *pFspInitParams,
		FSP_INFO_HEADER *fsp_ptr)
{
	FSP_INIT_RT_BUFFER *pFspRtBuffer = pFspInitParams->RtBufferPtr;
	uint32_t prev_sleep_state;

	/* Get previous sleep state but don't clear */
	prev_sleep_state = chipset_prev_sleep_state(0);
	printk(BIOS_INFO, "prev_sleep_state = S%d\n", prev_sleep_state);

	/* Initialize the UPD Data */
	GetUpdDefaultFromFsp (fsp_ptr, pFspRtBuffer->Common.UpdDataRgnPtr);
	ConfigureDefaultUpdData(fsp_ptr, pFspRtBuffer->Common.UpdDataRgnPtr);
	pFspInitParams->NvsBufferPtr = NULL;

#if IS_ENABLED(CONFIG_ENABLE_MRC_CACHE)
	/* Find the fastboot cache that was saved in the ROM */
	pFspInitParams->NvsBufferPtr = find_and_set_fastboot_cache();
#endif

	if (prev_sleep_state == ACPI_S3) {
		/* S3 resume */
		if ( pFspInitParams->NvsBufferPtr == NULL) {
			/* If waking from S3 and no cache then. */
			printk(BIOS_WARNING, "No MRC cache found in S3 resume path.\n");
			post_code(POST_RESUME_FAILURE);
			/* Clear Sleep Type */
			outl(inl(ACPI_BASE_ADDRESS + PM1_CNT) &
				~(SLP_TYP), ACPI_BASE_ADDRESS + PM1_CNT);
			/* Reboot */
			printk(BIOS_WARNING,"Rebooting..\n" );
			warm_reset();
			/* Should not reach here.. */
			die("Reboot System\n");
		}
		pFspRtBuffer->Common.BootMode = BOOT_ON_S3_RESUME;
	} else {
		/* Not S3 resume */
		pFspRtBuffer->Common.BootMode = BOOT_WITH_FULL_CONFIGURATION;
	}

	return;
}

/* The FSP returns here after the fsp_early_init call */
void ChipsetFspReturnPoint(EFI_STATUS Status,
		VOID *HobListPtr)
{
	*(void **)CBMEM_FSP_HOB_PTR=HobListPtr;

	if (Status == 0xFFFFFFFF) {
		warm_reset();
	}
	romstage_main_continue(Status, HobListPtr);
}

#endif	/* __PRE_RAM__ */
