/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#ifndef _AGESAWRAPPER_H_
#define _AGESAWRAPPER_H_

#include <stdint.h>
#include <vendorcode/amd/agesa/f15tn/AGESA.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
/* Define AMD Ontario APPU SSID/SVID */
#define AMD_APU_SVID    0x1022
#define AMD_APU_SSID    0x1234
#define PCIE_BASE_ADDRESS   CONFIG_MMCONF_BASE_ADDRESS

enum {
	PICK_DMI,       /* DMI Interface */
	PICK_PSTATE,    /* Acpi Pstate SSDT Table */
	PICK_SRAT,      /* SRAT Table */
	PICK_SLIT,      /* SLIT Table */
	PICK_WHEA_MCE,  /* WHEA MCE table */
	PICK_WHEA_CMC,  /* WHEA CMV table */
	PICK_ALIB,      /* SACPI SSDT table with ALIB implementation */
	PICK_IVRS,      /* IOMMU ACPI IVRS(I/O Virtualization Reporting Structure) table */
};

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

typedef struct {
	UINT32 CalloutName;
	AGESA_STATUS (*CalloutPtr) (UINT32 Func, UINT32 Data, VOID* ConfigPtr);
} BIOS_CALLOUT_STRUCT;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */

UINT32 agesawrapper_amdinitreset (void);
UINT32 agesawrapper_amdinitearly (void);
UINT32 agesawrapper_amdinitenv (void);
UINT32 agesawrapper_amdinitlate (void);
UINT32 agesawrapper_amdinitpost (void);
UINT32 agesawrapper_amdinitmid (void);
UINT32 agesawrapper_amdreadeventlog (UINT8 HeapStatus);
UINT32 agesawrapper_amdinitmmio (void);
uint32_t agesawrapper_amdinitcpuio (void);
void *agesawrapper_getlateinitptr (int pick);
UINT32 agesawrapper_amdlaterunaptask (UINT32 Func, UINT32 Data, void *ConfigPtr);
UINT32 agesawrapper_amdS3Save(VOID);
UINT32 agesawrapper_amdinitresume(VOID);
UINT32 agesawrapper_amds3laterestore (VOID);

UINT32 agesawrapper_fchs3earlyrestore (VOID);
UINT32 agesawrapper_fchs3laterestore (VOID);

#endif
