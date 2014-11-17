/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <types.h>
#include <string.h>
#include <console/console.h>
#include <bootstate.h>
#include <cbmem.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <baytrail/pci_devs.h>
#include <drivers/intel/fsp/fsp_util.h>
#include "../chip.h"
#include <arch/io.h>
#include <baytrail/reset.h>
#include <baytrail/pmc.h>
#include <baytrail/acpi.h>
#include <baytrail/iomap.h>

#ifdef __PRE_RAM__
#include <baytrail/romstage.h>
#endif

#ifdef __PRE_RAM__

/* Copy the default UPD region and settings to a buffer for modification */
static void GetUpdDefaultFromFsp (FSP_INFO_HEADER *FspInfo, UPD_DATA_REGION   *UpdData)
{
	VPD_DATA_REGION *VpdDataRgnPtr;
	UPD_DATA_REGION *UpdDataRgnPtr;
	VpdDataRgnPtr = (VPD_DATA_REGION *)(UINT32)(FspInfo->CfgRegionOffset  + FspInfo->ImageBase);
	UpdDataRgnPtr = (UPD_DATA_REGION *)(UINT32)(VpdDataRgnPtr->PcdUpdRegionOffset + FspInfo->ImageBase);
	memcpy((void*)UpdData, (void*)UpdDataRgnPtr, sizeof(UPD_DATA_REGION));
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

/**
 * Update the UPD data based on values from devicetree.cb
 *
 * @param UpdData Pointer to the UPD Data structure
 */
static void ConfigureDefaultUpdData(UPD_DATA_REGION *UpdData)
{
	ROMSTAGE_CONST struct device *dev;
	ROMSTAGE_CONST config_t *config;
	printk(BIOS_DEBUG, "Configure Default UPD Data\n");

	dev = dev_find_slot(0, SOC_DEV_FUNC);
	config = dev->chip_info;

	/* Set up default verb tables - Just HDMI audio */
	UpdData->AzaliaConfigPtr = (UINT32)&mAzaliaConfig;

	/* Set SPD addresses */
	if (config->PcdMrcInitSPDAddr1 == SPD_ADDR_DISABLED)
		UpdData->PcdMrcInitSPDAddr1 = 0x00;
	else if (config->PcdMrcInitSPDAddr1 != SPD_ADDR_DEFAULT)
		UpdData->PcdMrcInitSPDAddr1 = config->PcdMrcInitSPDAddr1;
	printk(BIOS_DEBUG, "SPD Addr1:\t\t0x%02x\n", UpdData->PcdMrcInitSPDAddr1);

	if (config->PcdMrcInitSPDAddr2 == SPD_ADDR_DISABLED)
		UpdData->PcdMrcInitSPDAddr2 = 0x00;
	else if (config->PcdMrcInitSPDAddr2 != SPD_ADDR_DEFAULT)
		UpdData->PcdMrcInitSPDAddr2 = config->PcdMrcInitSPDAddr2;
	printk(BIOS_DEBUG, "SPD Addr2:\t\t0x%02x\n", UpdData->PcdMrcInitSPDAddr2);

	if (config->PcdSataMode != SATA_MODE_DEFAULT)
		UpdData->PcdSataMode = config->PcdSataMode - SATA_MODE_IDE;

	if ((config->PcdeMMCBootMode != EMMC_USE_DEFAULT) ||
			(config->PcdeMMCBootMode != EMMC_FOLLOWS_DEVICETREE))
		UpdData->PcdeMMCBootMode = config->PcdeMMCBootMode;

	if (config->PcdLpssSioEnablePciMode != LPSS_PCI_MODE_DEFAULT)
		UpdData->PcdLpssSioEnablePciMode = config->PcdLpssSioEnablePciMode -
		LPSS_PCI_MODE_DISABLE;

	if (config->PcdMrcInitTsegSize != TSEG_SIZE_DEFAULT)
		UpdData->PcdMrcInitTsegSize = config->PcdMrcInitTsegSize;
	printk(BIOS_DEBUG, "Tseg Size:\t\t%d MB\n", UpdData->PcdMrcInitTsegSize);

	if (config->PcdMrcInitMmioSize != MMIO_SIZE_DEFAULT)
		UpdData->PcdMrcInitMmioSize = config->PcdMrcInitMmioSize;
	printk(BIOS_DEBUG, "MMIO Size:\t\t%d MB\n", UpdData->PcdMrcInitMmioSize);

	if (config->PcdIgdDvmt50PreAlloc != IGD_MEMSIZE_DEFAULT)
		UpdData->PcdIgdDvmt50PreAlloc = config->PcdIgdDvmt50PreAlloc;
	printk(BIOS_DEBUG, "IGD Memory Size:\t%d MB\n",
		UpdData->PcdIgdDvmt50PreAlloc * IGD_MEMSIZE_MULTIPLIER);

	if (config->PcdApertureSize != APERTURE_SIZE_DEFAULT)
		UpdData->PcdApertureSize = config->PcdApertureSize;
	printk(BIOS_DEBUG, "Aperture Size:\t\t%d MB\n",
		APERTURE_SIZE_BASE << UpdData->PcdApertureSize);

	if (config->PcdGttSize != GTT_SIZE_DEFAULT)
		UpdData->PcdGttSize = config->PcdGttSize;
	printk(BIOS_DEBUG, "GTT Size:\t\t%d MB\n", UpdData->PcdGttSize);

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
			case MIPI_DEV_FUNC:	/* Camera / Image Signal Processing */
				UpdData->ISPEnable = dev->enabled;
				printk(BIOS_DEBUG, "MIPI/ISP:\t\t%s\n",
						UpdData->PcdEnableSdio?"Enabled":"Disabled");
				break;
			case EMMC_DEV_FUNC: /* EMMC 4.1*/
				if ((dev->enabled) &&
						(config->PcdeMMCBootMode == EMMC_FOLLOWS_DEVICETREE))
					UpdData->PcdeMMCBootMode = EMMC_4_1 - EMMC_DISABLED;
				break;
			case SDIO_DEV_FUNC:
				UpdData->PcdEnableSdio = dev->enabled;
				printk(BIOS_DEBUG, "Sdio:\t\t\t%s\n",
						UpdData->PcdEnableSdio?"Enabled":"Disabled");
				break;
			case SD_DEV_FUNC:
				UpdData->PcdEnableSdcard = dev->enabled;
				printk(BIOS_DEBUG, "Sdcard:\t\t\t%s\n",
						UpdData->PcdEnableSdcard?"Enabled":"Disabled");
				break;
			case SATA_DEV_FUNC:
				UpdData->PcdEnableSata = dev->enabled;
				printk(BIOS_DEBUG, "Sata:\t\t\t%s\n",
						UpdData->PcdEnableSata?"Enabled":"Disabled");
				if (UpdData->PcdEnableSata)
					printk(BIOS_DEBUG, "SATA Mode:\t\t%s\n",
						UpdData->PcdSataMode?"AHCI":"IDE");
				break;
			case XHCI_DEV_FUNC:
				UpdData->PcdEnableXhci = dev->enabled;
				break;
			case LPE_DEV_FUNC:
				if (dev->enabled && config->LpeAcpiModeEnable ==
						LPE_ACPI_MODE_ENABLED)
					UpdData->PcdEnableLpe = LPE_ACPI_MODE_ENABLED;
				else
				UpdData->PcdEnableLpe = dev->enabled;
				printk(BIOS_DEBUG, "Lpe:\t\t\t%s\n",
						UpdData->PcdEnableLpe?"Enabled":"Disabled");
				printk(BIOS_DEBUG, "Lpe mode:\t\t%s\n",
						UpdData->PcdEnableLpe == LPE_ACPI_MODE_ENABLED?
						"ACPI":"PCI");
				break;
			case MMC45_DEV_FUNC: /* MMC 4.5*/
				if ((dev->enabled) &&
						(config->PcdeMMCBootMode == EMMC_FOLLOWS_DEVICETREE))
					UpdData->PcdeMMCBootMode = EMMC_4_5 - EMMC_DISABLED;
				break;
			case SIO_DMA1_DEV_FUNC:
				UpdData->PcdEnableDma0 = dev->enabled;
				printk(BIOS_DEBUG, "SIO Dma 0:\t\t%s\n",
						UpdData->PcdEnableDma0?"Enabled":"Disabled");
				break;
			case I2C1_DEV_FUNC:
				UpdData->PcdEnableI2C0 = dev->enabled;
				printk(BIOS_DEBUG, "SIO I2C0:\t\t%s\n",
						UpdData->PcdEnableI2C0?"Enabled":"Disabled");
				break;
			case I2C2_DEV_FUNC:
				UpdData->PcdEnableI2C1 = dev->enabled;
				printk(BIOS_DEBUG, "SIO I2C1:\t\t%s\n",
						UpdData->PcdEnableI2C1?"Enabled":"Disabled");
				break;
			case I2C3_DEV_FUNC:
				UpdData->PcdEnableI2C2 = dev->enabled;
				printk(BIOS_DEBUG, "SIO I2C2:\t\t%s\n",
						UpdData->PcdEnableI2C2?"Enabled":"Disabled");
				break;
			case I2C4_DEV_FUNC:
				UpdData->PcdEnableI2C3 = dev->enabled;
				printk(BIOS_DEBUG, "SIO I2C3:\t\t%s\n",
						UpdData->PcdEnableI2C3?"Enabled":"Disabled");
				break;
			case I2C5_DEV_FUNC:
				UpdData->PcdEnableI2C4 = dev->enabled;
				printk(BIOS_DEBUG, "SIO I2C4:\t\t%s\n",
						UpdData->PcdEnableI2C4?"Enabled":"Disabled");
				break;
			case I2C6_DEV_FUNC:
				UpdData->PcdEnableI2C5 = dev->enabled;
				printk(BIOS_DEBUG, "SIO I2C5:\t\t%s\n",
						UpdData->PcdEnableI2C5?"Enabled":"Disabled");
				break;
			case I2C7_DEV_FUNC:
				UpdData->PcdEnableI2C6 = dev->enabled;
				printk(BIOS_DEBUG, "SIO I2C6:\t\t%s\n",
						UpdData->PcdEnableI2C6?"Enabled":"Disabled");
				break;
			case TXE_DEV_FUNC: /* TXE */
				break;
			case HDA_DEV_FUNC:
				if (config->AzaliaAutoEnable) {
					UpdData->PcdEnableAzalia = 2;
					printk(BIOS_DEBUG, "Azalia:\t\t\tAuto\n");
				} else {
					UpdData->PcdEnableAzalia = dev->enabled;
					printk(BIOS_DEBUG, "Azalia:\t\t\t%s\n",
						UpdData->PcdEnableAzalia?"Enabled":"Disabled");
				}
				break;
			case PCIE_PORT1_DEV_FUNC:
			case PCIE_PORT2_DEV_FUNC:
			case PCIE_PORT3_DEV_FUNC:
			case PCIE_PORT4_DEV_FUNC:
				break;
			case EHCI_DEV_FUNC:
				UpdData->PcdEnableXhci = !(dev->enabled);
				break;
			case SIO_DMA2_DEV_FUNC:
				UpdData->PcdEnableDma1 = dev->enabled;
				printk(BIOS_DEBUG, "SIO Dma1:\t\t%s\n",
						UpdData->PcdEnableDma1?"Enabled":"Disabled");
				break;
			case PWM1_DEV_FUNC:
				UpdData->PcdEnablePwm0 = dev->enabled;
				printk(BIOS_DEBUG, "Pwm0\t\t\t%s\n",
						UpdData->PcdEnablePwm0?"Enabled":"Disabled");
				break;
			case PWM2_DEV_FUNC:
				UpdData->PcdEnablePwm1 = dev->enabled;
				printk(BIOS_DEBUG, "Pwm1:\t\t\t%s\n",
						UpdData->PcdEnablePwm1?"Enabled":"Disabled");
				break;
			case HSUART1_DEV_FUNC:
				UpdData->PcdEnableHsuart0 = dev->enabled;
				printk(BIOS_DEBUG, "Hsuart0:\t\t%s\n",
						UpdData->PcdEnableHsuart0?"Enabled":"Disabled");
				break;
			case HSUART2_DEV_FUNC:
				UpdData->PcdEnableHsuart1 = dev->enabled;
				printk(BIOS_DEBUG, "Hsuart1:\t\t%s\n",
						UpdData->PcdEnableHsuart1?"Enabled":"Disabled");
				break;
			case SPI_DEV_FUNC:
				UpdData->PcdEnableSpi = dev->enabled;
				printk(BIOS_DEBUG, "Spi:\t\t\t%s\n",
						UpdData->PcdEnableSpi?"Enabled":"Disabled");
				break;
			case LPC_DEV_FUNC: /* LPC */
				break;
			case SMBUS_DEV_FUNC:
				break;
		}
	}

	if(UpdData->PcdeMMCBootMode == EMMC_AUTO - EMMC_DISABLED) {
		printk(BIOS_DEBUG, "eMMC Mode:\t\tAuto");
	} else {
		printk(BIOS_DEBUG, "eMMC 4.1:\t\t%s\n",
			UpdData->PcdeMMCBootMode == EMMC_4_1 - EMMC_DISABLED?
			"Enabled":"Disabled");
		printk(BIOS_DEBUG, "eMMC 4.5:\t\t%s\n",
			UpdData->PcdeMMCBootMode == EMMC_4_5 - EMMC_DISABLED?
			"Enabled":"Disabled");
	}
	printk(BIOS_DEBUG, "Xhci:\t\t\t%s\n",
		UpdData->PcdEnableXhci?"Enabled":"Disabled");

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
	ConfigureDefaultUpdData(pFspRtBuffer->Common.UpdDataRgnPtr);
	pFspInitParams->NvsBufferPtr = NULL;

#if IS_ENABLED(CONFIG_ENABLE_MRC_CACHE)
	/* Find the fastboot cache that was saved in the ROM */
	pFspInitParams->NvsBufferPtr = find_and_set_fastboot_cache();
#endif

	if (prev_sleep_state == 3) {
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
	if (Status == 0xFFFFFFFF) {
		warm_reset();
	}
	romstage_main_continue(Status, HobListPtr);
}

#endif	/* __PRE_RAM__ */
