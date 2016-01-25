/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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

#ifndef _FSP_BAYTRAIL_CHIP_H_
#define _FSP_BAYTRAIL_CHIP_H_

#include <arch/acpi.h>
#include <drivers/intel/fsp1_0/fsp_values.h>

/* The devicetree parser expects chip.h to reside directly in the path
 * specified by the devicetree. */

struct soc_intel_fsp_baytrail_config {

/* ***** UPD Configuration ***** */

	/* Spd addresses */
	uint8_t  PcdMrcInitSPDAddr1;
	uint8_t  PcdMrcInitSPDAddr2;
	#define SPD_ADDR_DEFAULT	UPD_SPD_ADDR_DEFAULT
	#define SPD_ADDR_DISABLED	UPD_SPD_ADDR_DISABLED

	/* SataMode
	 *  NOTE: These are offset by 1 to set 0 as "use default".  This is so that
	 *  if the register value is not set in the devicetree.cb file, the default
	 *  value gets used.  This is fixed up in the chipset_fsp_util.c code.
	 *
	 *  0x0 "IDE"
	 *  0x1 "AHCI"
	 */
	uint8_t  PcdSataMode;
	#define SATA_MODE_DEFAULT	UPD_DEFAULT
	#define SATA_MODE_IDE		INCREMENT_FOR_DEFAULT(0)
	#define SATA_MODE_AHCI		INCREMENT_FOR_DEFAULT(1)

	/*
	 * MrcInitMmioSize
	 *  0x400, "1.0 GB"s
	 *  0x600, "1.5 GB"
	 *  0x800, "2.0 GB"
	 */
	uint16_t PcdMrcInitMmioSize;
	#define MMIO_SIZE_DEFAULT	UPD_DEFAULT
	#define MMIO_SIZE_1_0_GB	INCREMENT_FOR_DEFAULT(0x400)
	#define MMIO_SIZE_1_5_GB	INCREMENT_FOR_DEFAULT(0x600)
	#define MMIO_SIZE_2_0_GB	INCREMENT_FOR_DEFAULT(0x800)

	/*
	 * eMMCBootMode
	 *   NOTE: These are offset by 1 to set 0 as "use default".  This is so that
	 *   if the register value is not set in the devicetree.cb file, the default
	 *   value gets used.  This is fixed up in the chipset_fsp_util.c code
	 *
	 *   0x0 "Disabled"
	 *   0x1 "Auto"
	 *   0x2 "eMMC 4.1"
	 *   0x3 "eMMC 4.5"
	 */
	uint8_t PcdeMMCBootMode;
	#define EMMC_USE_DEFAULT		UPD_DEFAULT
	#define EMMC_DISABLED			UPD_DISABLE
	#define EMMC_AUTO			INCREMENT_FOR_DEFAULT(1)
	#define EMMC_4_1			INCREMENT_FOR_DEFAULT(2)
	#define EMMC_4_5			INCREMENT_FOR_DEFAULT(3)
	#define EMMC_FOLLOWS_DEVICETREE		UPD_USE_DEVICETREE

	/*
	 * IgdDvmt50PreAlloc
	 *  0x01, "32 MB"
	 *  0x02, "64 MB"
	 *  0x03, "96 MB"
	 *  0x04, "128 MB"
	 *  0x05, "160 MB"
	 *  0x06, "192 MB"
	 *  0x07, "224 MB"
	 *  0x08, "256 MB"
	 *  0x09, "288 MB"
	 *  0x0A, "320 MB"
	 *  0x0B, "352 MB"
	 *  0x0C, "384 MB"
	 *  0x0D, "416 MB"
	 *  0x0E, "448 MB"
	 *  0x0F, "480 MB"
	 *  0x10, "512 MB"
	 */
	uint8_t PcdIgdDvmt50PreAlloc;
	#define IGD_MEMSIZE_DEFAULT	UPD_DEFAULT
	#define IGD_MEMSIZE_32MB	INCREMENT_FOR_DEFAULT(0x01)
	#define IGD_MEMSIZE_64MB	INCREMENT_FOR_DEFAULT(0x02)
	#define IGD_MEMSIZE_96MB	INCREMENT_FOR_DEFAULT(0x03)
	#define IGD_MEMSIZE_128MB	INCREMENT_FOR_DEFAULT(0x04)
	#define IGD_MEMSIZE_160MB	INCREMENT_FOR_DEFAULT(0x05)
	#define IGD_MEMSIZE_192MB	INCREMENT_FOR_DEFAULT(0x06)
	#define IGD_MEMSIZE_224MB	INCREMENT_FOR_DEFAULT(0x07)
	#define IGD_MEMSIZE_256MB	INCREMENT_FOR_DEFAULT(0x08)
	#define IGD_MEMSIZE_288MB	INCREMENT_FOR_DEFAULT(0x09)
	#define IGD_MEMSIZE_320MB	INCREMENT_FOR_DEFAULT(0x0A)
	#define IGD_MEMSIZE_352MB	INCREMENT_FOR_DEFAULT(0x0B)
	#define IGD_MEMSIZE_384MB	INCREMENT_FOR_DEFAULT(0x0C)
	#define IGD_MEMSIZE_416MB	INCREMENT_FOR_DEFAULT(0x0D)
	#define IGD_MEMSIZE_448MB	INCREMENT_FOR_DEFAULT(0x0E)
	#define IGD_MEMSIZE_480MB	INCREMENT_FOR_DEFAULT(0x0F)
	#define IGD_MEMSIZE_512MB	INCREMENT_FOR_DEFAULT(0x10)
	#define IGD_MEMSIZE_MULTIPLIER	32

	/*
	 * Selection 0x1 , "128 MB"
	 * Selection 0x2 , "256 MB"
	 * Selection 0x3 , "512 MB"
	 */
	uint8_t PcdApertureSize;
	#define APERTURE_SIZE_DEFAULT	UPD_DEFAULT
	#define APERTURE_SIZE_128MB		INCREMENT_FOR_DEFAULT(1)
	#define APERTURE_SIZE_256MB		INCREMENT_FOR_DEFAULT(2)
	#define APERTURE_SIZE_512MB		INCREMENT_FOR_DEFAULT(3)
	#define APERTURE_SIZE_BASE		64

	/*
	 * Selection 0x1 , "1 MB"
	 * Selection 0x2 , "2 MB"
	 */
	uint8_t PcdGttSize;
	#define GTT_SIZE_DEFAULT	UPD_DEFAULT
	#define GTT_SIZE_1MB		INCREMENT_FOR_DEFAULT(1)
	#define GTT_SIZE_2MB		INCREMENT_FOR_DEFAULT(2)

	/*
	 *  Enable PCI Mode for LPSS SIO devices.
	 *  If disabled, LPSS SIO devices will run in ACPI mode.
	 */
	uint8_t PcdLpssSioEnablePciMode;
	#define LPSS_PCI_MODE_DEFAULT	UPD_DEFAULT
	#define LPSS_PCI_MODE_DISABLE	UPD_DISABLE
	#define LPSS_PCI_MODE_ENABLE	UPD_ENABLE

	/* modifiers for various enables */
	uint8_t AzaliaAutoEnable;
	#define AZALIA_FOLLOWS_DEVICETREE	UPD_USE_DEVICETREE
	#define AZALIA_FSP_AUTO_ENABLE		UPD_ENABLE

	uint8_t LpeAcpiModeEnable;
	#define LPE_ACPI_MODE_DISABLED	UPD_DISABLE
	#define LPE_ACPI_MODE_ENABLED	UPD_ENABLE

	uint32_t SerialDebugPortAddress;
	#define  SerialDebugPortAddress_DEFAULT	UPD_DEFAULT

	uint8_t  SerialDebugPortType;
	#define  SERIAL_DEBUG_PORT_DEFAULT	UPD_DEFAULT
	#define  SERIAL_DEBUG_PORT_TYPE_NONE	INCREMENT_FOR_DEFAULT(0)
	#define  SERIAL_DEBUG_PORT_TYPE_IO	INCREMENT_FOR_DEFAULT(1)
	#define  SERIAL_DEBUG_PORT_TYPE_MMIO	INCREMENT_FOR_DEFAULT(2)

	uint8_t  PcdMrcDebugMsg;
	#define  MRC_DEBUG_MSG_DEFAULT	UPD_DEFAULT
	#define  MRC_DEBUG_MSG_DISABLE	UPD_DISABLE
	#define  MRC_DEBUG_MSG_ENABLE	UPD_ENABLE

	uint8_t  PcdSccEnablePciMode;
	#define  SCC_PCI_MODE_DEFAULT	UPD_DEFAULT
	#define  SCC_PCI_MODE_DISABLE	UPD_DISABLE
	#define  SCC_PCI_MODE_ENABLE	UPD_ENABLE

	uint8_t  IgdRenderStandby;
	#define  IGD_RENDER_STANDBY_DEFAULT	UPD_DEFAULT
	#define  IGD_RENDER_STANDBY_DISABLE	UPD_DISABLE
	#define  IGD_RENDER_STANDBY_ENABLE	UPD_ENABLE

	uint8_t  TxeUmaEnable;
	#define  TXE_UMA_DEFAULT	UPD_DEFAULT
	#define  TXE_UMA_DISABLE	UPD_DISABLE
	#define  TXE_UMA_ENABLE		UPD_ENABLE

	/*
	 * PcdOsSelection
	 * Selection 0x1 , "Android"
	 * Selection 0x4 , "Linux OS"
	 */
	uint8_t  PcdOsSelection;
	#define  OS_SELECTION_DEFAULT	UPD_DEFAULT
	#define  OS_SELECTION_ANDROID	INCREMENT_FOR_DEFAULT(1)
	#define  OS_SELECTION_LINUX	INCREMENT_FOR_DEFAULT(4)

	/* PcdEMMC45DDR50Enabled */
	uint8_t  PcdEMMC45DDR50Enabled;
	#define  EMMC45_DDR50_DEFAULT	UPD_DEFAULT
	#define  EMMC45_DDR50_DISABLE	UPD_DISABLE
	#define  EMMC45_DDR50_ENABLE	UPD_ENABLE

	/* PcdEMMC45HS200Enabled */
	uint8_t  PcdEMMC45HS200Enabled;
	#define  EMMC45_HS200_DEFAULT	UPD_DEFAULT
	#define  EMMC45_HS200_DISABLE	UPD_DISABLE
	#define  EMMC45_HS200_ENABLE	UPD_ENABLE

	/* PcdEMMC45RetuneTimerValue */
	uint8_t  PcdEMMC45RetuneTimerValue;
	#define  EMMC45_RETURN_TIMER_DEFAULT UPD_DEFAULT

	/* PcdEnableIgd */
	uint8_t  PcdEnableIgd;
	#define  ENABLE_IGD_DEFAULT	UPD_DEFAULT
	#define  ENABLE_IGD_DISABLE	UPD_DISABLE
	#define  ENABLE_IGD_ENABLE	UPD_ENABLE

	/* Memory down data */
	uint8_t  EnableMemoryDown;
	#define  MEMORY_DOWN_DEFAULT	UPD_DEFAULT
	#define  MEMORY_DOWN_DISABLE	UPD_DISABLE
	#define  MEMORY_DOWN_ENABLE	UPD_ENABLE

	/*
	 * PcdDRAMSpeed
	 * Selection 0x0 , "800 MHz"
	 * Selection 0x1 , "1066 MHz"
	 * Selection 0x2 , "1333 MHz"
	 * Selection 0x3 , "1600 MHz"
	 */
	uint8_t  DRAMSpeed;
	#define  DRAM_SPEED_DEFAULT	UPD_DEFAULT
	#define  DRAM_SPEED_800MHZ	INCREMENT_FOR_DEFAULT(0)
	#define  DRAM_SPEED_1066MHZ	INCREMENT_FOR_DEFAULT(1)
	#define  DRAM_SPEED_1333MHZ	INCREMENT_FOR_DEFAULT(2)
	#define  DRAM_SPEED_1600MHZ	INCREMENT_FOR_DEFAULT(3)

	/*
	 * PcdDRAMType
	 * Selection 0x0 , "DDR3"
	 * Selection 0x1 , "DDR3L"
	 * Selection 0x2 , "DDR3U"
	 * Selection 0x4 , "LPDDR2"
	 * Selection 0x5 , "LPDDR3"
	 * Selection 0x6 , "DDR4"
	 */
	uint8_t  DRAMType;
	#define  DRAM_TYPE_DEFAULT	UPD_DEFAULT
	#define  DRAM_TYPE_DDR3		INCREMENT_FOR_DEFAULT(0)
	#define  DRAM_TYPE_DDR3L	INCREMENT_FOR_DEFAULT(1)

	uint8_t  DIMM0Enable;
	#define  DIMM0_ENABLE_DEFAULT	UPD_DEFAULT
	#define  DIMM0_DISABLE		UPD_DISABLE
	#define  DIMM0_ENABLE		UPD_ENABLE

	uint8_t  DIMM1Enable;
	#define  DIMM1_ENABLE_DEFAULT	UPD_DEFAULT
	#define  DIMM1_DISABLE		UPD_DISABLE
	#define  DIMM1_ENABLE		UPD_ENABLE

	/*
	 * PcdDIMMDWidth
	 * Selection 0x0 , "x8"
	 * Selection 0x1 , "x16"
	 * Selection 0x2 , "x32"
	 */
	uint8_t  DIMMDWidth;
	#define  DIMM_DWIDTH_DEFAULT	UPD_DEFAULT
	#define  DIMM_DWIDTH_X8		INCREMENT_FOR_DEFAULT(0)
	#define  DIMM_DWIDTH_X16	INCREMENT_FOR_DEFAULT(1)
	#define  DIMM_DWIDTH_X32	INCREMENT_FOR_DEFAULT(2)

	/*
	 * PcdDIMMDensity
	 * Selection 0x0 , "1 Gbit"
	 * Selection 0x1 , "2 Gbit"
	 * Selection 0x2 , "4 Gbit"
	 * Selection 0x3 , "8 Gbit"
	 */
	uint8_t  DIMMDensity;
	#define  DIMM_DENSITY_DEFAULT	UPD_DEFAULT
	#define  DIMM_DENSITY_1G_BIT	INCREMENT_FOR_DEFAULT(0)
	#define  DIMM_DENSITY_2G_BIT	INCREMENT_FOR_DEFAULT(1)
	#define  DIMM_DENSITY_4G_BIT	INCREMENT_FOR_DEFAULT(2)
	#define  DIMM_DENSITY_8G_BIT	INCREMENT_FOR_DEFAULT(3)

	/*
	 * PcdDIMMBusWidth
	 * Selection 0x0 , "8 bits"
	 * Selection 0x1 , "16 bits"
	 * Selection 0x2 , "32 bits"
	 * Selection 0x3 , "64 bits"
	 */
	uint8_t  DIMMBusWidth;
	#define  DIMM_BUS_WIDTH_DEFAULT	UPD_DEFAULT
	#define  DIMM_BUS_WIDTH_8BIT	INCREMENT_FOR_DEFAULT(0)
	#define  DIMM_BUS_WIDTH_16BIT	INCREMENT_FOR_DEFAULT(1)
	#define  DIMM_BUS_WIDTH_32BIT	INCREMENT_FOR_DEFAULT(2)
	#define  DIMM_BUS_WIDTH_64BIT	INCREMENT_FOR_DEFAULT(3)

	/*
	 * PcdDIMMSides
	 * Selection 0x0 , "1 Ranks"
	 * Selection 0x1 , "2 Ranks"
	 */
	uint8_t  DIMMSides;
	#define  DIMM_SIDES_DEFAULT	UPD_DEFAULT
	#define  DIMM_SIDES_1RANK	INCREMENT_FOR_DEFAULT(0)
	#define  DIMM_SIDES_2RANK	INCREMENT_FOR_DEFAULT(1)

	uint8_t  DIMMtCL;
	#define  DIMM_TCL_DEFAULT	UPD_DEFAULT

	uint8_t  DIMMtRPtRCD;
	#define  DIMM_TRP_TRCD_DEFAULT	UPD_DEFAULT

	uint8_t  DIMMtWR;
	#define  DIMM_TWR_DEFAULT	UPD_DEFAULT

	uint8_t  DIMMtWTR;
	#define  DIMM_TWTR_DEFAULT	UPD_DEFAULT

	uint8_t  DIMMtRRD;
	#define  DIMM_TRRD_DEFAULT	UPD_DEFAULT

	uint8_t  DIMMtRTP;
	#define  DIMM_TRTP_DEFAULT	UPD_DEFAULT

	uint8_t  DIMMtFAW;
	#define  DIMM_TFAW_DEFAULT	UPD_DEFAULT

/* ***** ACPI configuration ***** */
	/* Options for these are in src/arch/x86/include/arch/acpi.h */
	uint8_t  fadt_pm_profile;
	uint16_t fadt_boot_arch;

};

extern struct chip_operations soc_intel_fsp_baytrail_ops;
#endif /* _FSP_BAYTRAIL_CHIP_H_ */
