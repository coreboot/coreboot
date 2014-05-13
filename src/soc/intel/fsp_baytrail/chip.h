/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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

#ifndef _FSP_BAYTRAIL_CHIP_H_
#define _FSP_BAYTRAIL_CHIP_H_

#include <arch/acpi.h>

/* The devicetree parser expects chip.h to reside directly in the path
 * specified by the devicetree. */

struct soc_intel_fsp_baytrail_config {

/* ***** UPD Configuration ***** */

	/* Spd addresses */
	uint8_t  MrcInitSPDAddr1;
	uint8_t  MrcInitSPDAddr2;
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
	uint8_t  SataMode;
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
	uint16_t MrcInitTsegSize;
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
	uint16_t MrcInitMmioSize;
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
	uint8_t  eMMCBootMode;
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
	uint8_t  IgdDvmt50PreAlloc;
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
	uint8_t  ApertureSize;
	#define APERTURE_SIZE_DEFAULT	0
	#define APERTURE_SIZE_128MB		1
	#define APERTURE_SIZE_256MB		2
	#define APERTURE_SIZE_512MB		3
	#define APERTURE_SIZE_BASE		64

	/*
	 * Selection 0x1 , "1 MB"
	 * Selection 0x2 , "2 MB"
	 */
	uint8_t  GttSize;
	#define GTT_SIZE_DEFAULT	0
	#define GTT_SIZE_1MB		1
	#define GTT_SIZE_2MB		2

	/*
	 *  Enable PCI Mode for LPSS SIO devices.
	 *  If disabled, LPSS SIO devices will run in ACPI mode.
	 */
	uint8_t  LpssSioEnablePciMode;
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

/* ***** ACPI configuration ***** */
	/* Options for these are in src/arch/x86/include/arch/acpi.h */
	uint8_t  fadt_pm_profile;
	uint16_t fadt_boot_arch;

};

extern struct chip_operations soc_intel_fsp_baytrail_ops;
#endif /* _FSP_BAYTRAIL_CHIP_H_ */
