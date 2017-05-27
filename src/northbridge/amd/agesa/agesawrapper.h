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

AGESA_STATUS agesawrapper_amdinitreset(void);
AGESA_STATUS agesawrapper_amdinitearly(void);
AGESA_STATUS agesawrapper_amdinitenv(void);
AGESA_STATUS agesawrapper_amdinitlate(void);
AGESA_STATUS agesawrapper_amdinitpost(void);
AGESA_STATUS agesawrapper_amdinitmid(void);

AGESA_STATUS agesawrapper_amdinitresume(void);
AGESA_STATUS agesawrapper_amdS3Save(void);
AGESA_STATUS agesawrapper_amds3laterestore(void);
AGESA_STATUS agesawrapper_amdlaterunaptask (UINT32 Func, UINTN Data, VOID *ConfigPtr);

AGESA_STATUS agesawrapper_fchs3earlyrestore(void);
AGESA_STATUS agesawrapper_fchs3laterestore(void);

#define AGESA_EVENTLOG(status, stdheader) \
	agesawrapper_trace(status, stdheader, __func__)

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
#if !IS_ENABLED(CONFIG_CPU_AMD_PI)
/* TODO: With binaryPI we need different interface. */
AGESA_STATUS OemInitResume(AMD_S3_PARAMS *dataBlock);
AGESA_STATUS OemS3LateRestore(AMD_S3_PARAMS *dataBlock);
AGESA_STATUS OemS3Save(AMD_S3_PARAMS *dataBlock);
#endif

#endif /* _AGESAWRAPPER_H_ */
