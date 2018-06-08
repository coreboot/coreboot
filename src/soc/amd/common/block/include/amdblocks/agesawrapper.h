/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 - 2017 Advanced Micro Devices, Inc.
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

#ifndef __AGESAWRAPPER_H__
#define __AGESAWRAPPER_H__

#include <stdint.h>
#include <agesa_headers.h>

enum {
	PICK_DMI,       /* DMI Interface */
	PICK_PSTATE,    /* Acpi Pstate SSDT Table */
	PICK_SRAT,      /* SRAT Table */
	PICK_SLIT,      /* SLIT Table */
	PICK_WHEA_MCE,  /* WHEA MCE table */
	PICK_WHEA_CMC,  /* WHEA CMV table */
	PICK_ALIB,      /* SACPI SSDT table with ALIB implementation */
	PICK_IVRS,      /* IOMMU ACPI IVRS (I/O Virt. Reporting Structure) */
	PICK_CRAT,
};

/* Return current dispatcher or NULL on error. */
MODULE_ENTRY agesa_get_dispatcher(void);

AGESA_STATUS agesawrapper_amdinitreset(void);
AGESA_STATUS agesawrapper_amdinitearly(void);
AGESA_STATUS agesawrapper_amdinitenv(void);
AGESA_STATUS agesawrapper_amdinitlate(void);
AGESA_STATUS agesawrapper_amdinitpost(void);
AGESA_STATUS agesawrapper_amdinitmid(void);
void *agesawrapper_getlateinitptr(int pick);
AGESA_STATUS agesawrapper_amdlaterunaptask(UINT32 Func, UINTN Data,
							void *ConfigPtr);
AGESA_STATUS agesawrapper_amdinitrtb(void);
AGESA_STATUS agesawrapper_amdinitresume(void);
AGESA_STATUS agesawrapper_amds3laterestore(void);
AGESA_STATUS agesawrapper_amds3finalrestore(void);

AGESA_STATUS agesawrapper_fchs3earlyrestore(void);
AGESA_STATUS agesawrapper_fchs3laterestore(void);

VOID OemCustomizeInitEarly(IN OUT AMD_EARLY_PARAMS *InitEarly);
VOID amd_initcpuio(void);
const void *agesawrapper_locate_module(const CHAR8 name[8]);

void SetFchResetParams(FCH_RESET_INTERFACE *params);
void OemPostParams(AMD_POST_PARAMS *PostParams);
void SetMemParams(AMD_POST_PARAMS *PostParams);
void SetFchEnvParams(FCH_INTERFACE *params);
void SetNbEnvParams(GNB_ENV_CONFIGURATION *params);
void SetFchMidParams(FCH_INTERFACE *params);
void SetNbMidParams(GNB_MID_CONFIGURATION *params);

#endif /* __AGESAWRAPPER_H__ */
