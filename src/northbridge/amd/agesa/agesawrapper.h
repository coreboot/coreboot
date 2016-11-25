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

#if IS_ENABLED(CONFIG_AGESA_LEGACY_WRAPPER) || \
	IS_ENABLED(CONFIG_BINARYPI_LEGACY_WRAPPER)

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

AGESA_STATUS agesawrapper_fchs3earlyrestore(void);
AGESA_STATUS agesawrapper_fchs3laterestore(void);

#define AGESA_EVENTLOG(status, stdheader) \
	agesawrapper_trace(status, stdheader, __func__)

#else

/* Defined to make unused agesa_main() build. */
static inline int agesawrapper_amdinitreset(void) { return -1; }
static inline int agesawrapper_amdinitearly(void) { return -1; }
static inline int agesawrapper_amdinitenv(void) { return -1; }
static inline int agesawrapper_amdinitpost(void) { return -1; }
static inline int agesawrapper_amdinitresume(void) { return -1; }
static inline int agesawrapper_amds3laterestore(void) { return -1; }

#endif

#if IS_ENABLED(CONFIG_AGESA_LEGACY_WRAPPER)
struct OEM_HOOK
{
	/* romstage */
	AGESA_STATUS (*InitEarly)(AMD_EARLY_PARAMS *);
	AGESA_STATUS (*InitPost)(AMD_POST_PARAMS *);

	/* ramstage */
	AGESA_STATUS (*InitMid)(AMD_MID_PARAMS *);
};

extern const struct OEM_HOOK OemCustomize;
#endif

#if IS_ENABLED(CONFIG_BINARYPI_LEGACY_WRAPPER)
const void *agesawrapper_locate_module (const CHAR8 name[8]);

VOID OemCustomizeInitEarly (IN OUT AMD_EARLY_PARAMS *InitEarly);
void OemPostParams(AMD_POST_PARAMS *PostParams);
#endif

#endif /* _AGESAWRAPPER_H_ */
