/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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

#ifndef _AGESAWRAPPER_H_
#define _AGESAWRAPPER_H_

#include <stdint.h>
#include "Porting.h"
#include "AGESA.h"

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

AGESA_STATUS agesawrapper_amdinitreset(void);
AGESA_STATUS agesawrapper_amdinitearly(void);
AGESA_STATUS agesawrapper_amdinitenv(void);
AGESA_STATUS agesawrapper_amdinitlate(void);
AGESA_STATUS agesawrapper_amdinitpost(void);
AGESA_STATUS agesawrapper_amdinitmid(void);

void agesawrapper_trace(AGESA_STATUS ret, AMD_CONFIG_PARAMS *StdHeader, const char *func);
#define AGESA_EVENTLOG(status, stdheader) \
	agesawrapper_trace(status, stdheader, __func__)

void amd_initcpuio(void);
void amd_initmmio(void);
void amd_initenv(void);

AGESA_STATUS agesawrapper_amdinitresume(void);
AGESA_STATUS agesawrapper_amdS3Save(void);
AGESA_STATUS agesawrapper_amds3laterestore(void);
AGESA_STATUS agesawrapper_amdlaterunaptask (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
void *agesawrapper_getlateinitptr (int pick);

AGESA_STATUS agesawrapper_fchs3earlyrestore(void);
AGESA_STATUS agesawrapper_fchs3laterestore(void);

struct OEM_HOOK
{
	/* romstage */
	AGESA_STATUS (*InitEarly)(AMD_EARLY_PARAMS *);
	AGESA_STATUS (*InitPost)(AMD_POST_PARAMS *);

	/* ramstage */
	AGESA_STATUS (*InitMid)(AMD_MID_PARAMS *);
};

extern const struct OEM_HOOK OemCustomize;

/* For suspend-to-ram support. */
AGESA_STATUS OemInitResume(AMD_RESUME_PARAMS *ResumeParams);
AGESA_STATUS OemS3LateRestore(AMD_S3LATE_PARAMS *S3LateParams);
AGESA_STATUS OemS3Save(AMD_S3SAVE_PARAMS *S3SaveParams);

#endif /* _AGESAWRAPPER_H_ */
