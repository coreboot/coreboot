/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
 * Copyright (C) 2014 Intel Corporation
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

#ifndef _FSP_BAYTRAIL_CHIP_H_
#define _FSP_BAYTRAIL_CHIP_H_

#include <arch/acpi.h>

/* The devicetree parser expects chip.h to reside directly in the path
 * specified by the devicetree. */

struct soc_intel_fsp_baytrail_config {

/* ***** UPD Configuration ***** */

	/* Spd addresses */
	uint8_t  PcdMrcInitSPDAddr1;
	uint8_t  PcdMrcInitSPDAddr2;
	#define SPD_ADDR_DEFAULT	0x00
	#define SPD_ADDR_DISABLED	0xFF

	/* SataMode
	 *  NOTE: These are offset by 1 to set 0 as "use default".  This is so that
	 *  if the register value is not set in the devicetree.cb file, the default
	 *  value gets used.  This is fixed up in the chipset_fsp_util.c code.
	 *
	 *  0x0 "IDE"
	 *  0x1 "AHCI"
	 */
	uint8_t  PcdSataMode;
	#define SATA_MODE_DEFAULT	0x00
	#define SATA_MODE_IDE		0x01
	#define SATA_MODE_AHCI		0x02

	/*
	 * MrcInitTsegSize
	 *  0x01, "1 MB"
	 *  0x02, "2 MB"
	 *  0x04, "4 MB"
	 *  0x08, "8 MB"
	 */
	uint16_t PcdMrcInitTsegSize;
	#define TSEG_SIZE_DEFAULT	0
	#define TSEG_SIZE_1_MB		1
	#define TSEG_SIZE_2_MB		2
	#define TSEG_SIZE_4_MB		4
	#define TSEG_SIZE_8_MB		8

	/*
	 * MrcInitMmioSize
	 *  0x400, "1.0 GB"s
	 *  0x600, "1.5 GB"
	 *  0x800, "2.0 GB"
	 */
	uint16_t PcdMrcInitMmioSize;
	#define MMIO_SIZE_DEFAULT	0x00
	#define MMIO_SIZE_1_0_GB	0x400
	#define MMIO_SIZE_1_5_GB	0x600
	#define MMIO_SIZE_2_0_GB	0x800

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
	#define EMMC_USE_DEFAULT		0
	#define EMMC_DISABLED			1
	#define EMMC_AUTO				2
	#define EMMC_4_1				3
	#define EMMC_4_5				4
	#define EMMC_FOLLOWS_DEVICETREE	5

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
	#define IGD_MEMSIZE_DEFAULT	0x00
	#define IGD_MEMSIZE_32MB	0x01
	#define IGD_MEMSIZE_64MB	0x02
	#define IGD_MEMSIZE_96MB	0x03
	#define IGD_MEMSIZE_128MB	0x04
	#define IGD_MEMSIZE_160MB	0x05
	#define IGD_MEMSIZE_192MB	0x06
	#define IGD_MEMSIZE_224MB	0x07
	#define IGD_MEMSIZE_256MB	0x08
	#define IGD_MEMSIZE_288MB	0x09
	#define IGD_MEMSIZE_320MB	0x0A
	#define IGD_MEMSIZE_352MB	0x0B
	#define IGD_MEMSIZE_384MB	0x0C
	#define IGD_MEMSIZE_416MB	0x0D
	#define IGD_MEMSIZE_448MB	0x0E
	#define IGD_MEMSIZE_480MB	0x0F
	#define IGD_MEMSIZE_512MB	0x10
	#define IGD_MEMSIZE_MULTIPLIER	32

	/*
	 * Selection 0x1 , "128 MB"
	 * Selection 0x2 , "256 MB"
	 * Selection 0x3 , "512 MB"
	 */
	uint8_t PcdApertureSize;
	#define APERTURE_SIZE_DEFAULT	0
	#define APERTURE_SIZE_128MB		1
	#define APERTURE_SIZE_256MB		2
	#define APERTURE_SIZE_512MB		3
	#define APERTURE_SIZE_BASE		64

	/*
	 * Selection 0x1 , "1 MB"
	 * Selection 0x2 , "2 MB"
	 */
	uint8_t PcdGttSize;
	#define GTT_SIZE_DEFAULT	0
	#define GTT_SIZE_1MB		1
	#define GTT_SIZE_2MB		2

	/*
	 *  Enable PCI Mode for LPSS SIO devices.
	 *  If disabled, LPSS SIO devices will run in ACPI mode.
	 */
	uint8_t PcdLpssSioEnablePciMode;
	#define LPSS_PCI_MODE_DEFAULT	0x00
	#define LPSS_PCI_MODE_DISABLE	0x01
	#define LPSS_PCI_MODE_ENABLE	0x02

	/* modifiers for various enables */
	uint8_t  AzaliaAutoEnable;
	#define AZALIA_FOLLOWS_DEVICETREE	0
	#define AZALIA_FSP_AUTO_ENABLE		1

	uint8_t  LpeAcpiModeEnable;
	#define LPE_ACPI_MODE_DISABLED	1
	#define LPE_ACPI_MODE_ENABLED	2

	uint32_t SerialDebugPortAddress;
	#define  SerialDebugPortAddress_DEFAULT 0

	uint8_t  SerialDebugPortType;
	#define  SERIAL_DEBUG_PORT_DEFAULT      0
	#define  SERIAL_DEBUG_PORT_TYPE_NONE    1
	#define  SERIAL_DEBUG_PORT_TYPE_IO      2
	#define  SERIAL_DEBUG_PORT_TYPE_MMIO    3

	uint8_t  PcdMrcDebugMsg;
	#define  MRC_DEBUG_MSG_DEFAULT          0
	#define  MRC_DEBUG_MSG_DISABLE          1
	#define  MRC_DEBUG_MSG_ENABLE           2

	uint8_t  PcdSccEnablePciMode;
	#define  SCC_PCI_MODE_DEFAULT           0
	#define  SCC_PCI_MODE_DISABLE           1
	#define  SCC_PCI_MODE_ENABLE            2

	uint8_t  IgdRenderStandby;
	#define  IGD_RENDER_STANDBY_DEFAULT     0
	#define  IGD_RENDER_STANDBY_DISABLE     1
	#define  IGD_RENDER_STANDBY_ENABLE      2

	uint8_t  TxeUmaEnable;
	#define  TXE_UMA_DEFAULT                0
	#define  TXE_UMA_DISABLE                1
	#define  TXE_UMA_ENABLE                 2

	/* Memory down data */
	uint8_t  EnableMemoryDown;
	#define  MEMORY_DOWN_DEFAULT            0
	#define  MEMORY_DOWN_DISABLE            1
	#define  MEMORY_DOWN_ENABLE             2

	uint8_t  DRAMSpeed;
	#define  DRAM_SPEED_DEFAULT             0
	#define  DRAM_SPEED_800MHZ              1
	#define  DRAM_SPEED_1066MHZ             2
	#define  DRAM_SPEED_1333MHZ             3
	#define  DRAM_SPEED_1600MHZ             4

	uint8_t  DRAMType;
	#define  DRAM_TYPE_DEFAULT              0
	#define  DRAM_TYPE_DDR3                 1
	#define  DRAM_TYPE_DDR3L                2

	uint8_t  DIMM0Enable;
	#define  DIMM0_ENABLE_DEFAULT           0
	#define  DIMM0_DISABLE                  1
	#define  DIMM0_ENABLE                   2

	uint8_t  DIMM1Enable;
	#define  DIMM1_ENABLE_DEFAULT           0
	#define  DIMM1_DISABLE                  1
	#define  DIMM1_ENABLE                   2

	uint8_t  DIMMDWidth;
	#define  DIMM_DWIDTH_DEFAULT            0
	#define  DIMM_DWIDTH_X8                 1
	#define  DIMM_DWIDTH_X16                2
	#define  DIMM_DWIDTH_X32                3

	uint8_t  DIMMDensity;
	#define  DIMM_DENSITY_DEFAULT           0
	#define  DIMM_DENSITY_1G_BIT            1
	#define  DIMM_DENSITY_2G_BIT            2
	#define  DIMM_DENSITY_4G_BIT            3
	#define  DIMM_DENSITY_8G_BIT            4

	uint8_t  DIMMBusWidth;
	#define  DIMM_BUS_WIDTH_DEFAULT         0
	#define  DIMM_BUS_WIDTH_8BIT            1
	#define  DIMM_BUS_WIDTH_16BIT           2
	#define  DIMM_BUS_WIDTH_32BIT           3
	#define  DIMM_BUS_WIDTH_64BIT           4

	uint8_t  DIMMSides;
	#define  DIMM_SIDES_DEFAULT             0
	#define  DIMM_SIDES_1RANK               1
	#define  DIMM_SIDES_2RANK               2

	uint8_t  DIMMtCL;
	#define  DIMM_TCL_DEFAULT               0

	uint8_t  DIMMtRPtRCD;
	#define  DIMM_TRP_TRCD_DEFAULT          0

	uint8_t  DIMMtWR;
	#define  DIMM_TWR_DEFAULT               0

	uint8_t  DIMMtWTR;
	#define  DIMM_TWTR_DEFAULT              0

	uint8_t  DIMMtRRD;
	#define  DIMM_TRRD_DEFAULT              0

	uint8_t  DIMMtRTP;
	#define  DIMM_TRTP_DEFAULT              0

	uint8_t  DIMMtFAW;
	#define  DIMM_TFAW_DEFAULT              0

/* ***** ACPI configuration ***** */
	/* Options for these are in src/arch/x86/include/arch/acpi.h */
	uint8_t  fadt_pm_profile;
	uint16_t fadt_boot_arch;

};

extern struct chip_operations soc_intel_fsp_baytrail_ops;
#endif /* _FSP_BAYTRAIL_CHIP_H_ */
