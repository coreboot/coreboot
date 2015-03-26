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
 * Foundation, Inc.
 */

#ifndef _AGESAWRAPPER_H_
#define _AGESAWRAPPER_H_

#include <stdint.h>
#include "Porting.h"
#include "AGESA.h"

/* TODO: Add a kconfig option to name the AGESA ROM file in CBFS */
#ifndef CONFIG_CBFS_AGESA_NAME
#define CONFIG_CBFS_AGESA_NAME "AGESA"
#endif

enum {
	PICK_DMI,       /* DMI Interface */
	PICK_PSTATE,    /* Acpi Pstate SSDT Table */
	PICK_SRAT,      /* SRAT Table */
	PICK_SLIT,      /* SLIT Table */
	PICK_WHEA_MCE,  /* WHEA MCE table */
	PICK_WHEA_CMC,  /* WHEA CMV table */
	PICK_ALIB,      /* SACPI SSDT table with ALIB implementation */
	PICK_IVRS,      /* IOMMU ACPI IVRS(I/O Virtualization Reporting Structure) table */
	PICK_CRAT,
};

AGESA_STATUS agesawrapper_amdinitreset(void);
AGESA_STATUS agesawrapper_amdinitearly(void);
AGESA_STATUS agesawrapper_amdinitenv(void);
AGESA_STATUS agesawrapper_amdinitlate(void);
AGESA_STATUS agesawrapper_amdinitpost(void);
AGESA_STATUS agesawrapper_amdinitmid(void);
AGESA_STATUS agesawrapper_amdreadeventlog(UINT8 HeapStatus);
void *agesawrapper_getlateinitptr(int pick);
AGESA_STATUS agesawrapper_amdlaterunaptask(UINT32 Func, UINT32 Data, void *ConfigPtr);
AGESA_STATUS agesawrapper_amdS3Save(void);
AGESA_STATUS agesawrapper_amdinitresume(void);
AGESA_STATUS agesawrapper_amds3laterestore(void);

AGESA_STATUS agesawrapper_fchs3earlyrestore(void);
AGESA_STATUS agesawrapper_fchs3laterestore(void);

VOID OemCustomizeInitEarly (IN	OUT AMD_EARLY_PARAMS *InitEarly);
VOID amd_initcpuio(void);
VOID amd_initmmio(void);
const void *agesawrapper_locate_module (const CHAR8 name[8]);

#endif /* _AGESAWRAPPER_H_ */
