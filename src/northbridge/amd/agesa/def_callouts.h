/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011,2012 Advanced Micro Devices, Inc.
 * Copyright (C) 2013 Sage Electronic Engineering, LLC
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

#ifndef CALLOUTS_AMD_AGESA_H
#define CALLOUTS_AMD_AGESA_H

#include "Porting.h"
#include "AGESA.h"


AGESA_STATUS agesa_NoopUnsupported (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
AGESA_STATUS agesa_NoopSuccess (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
AGESA_STATUS agesa_EmptyIdsInitData (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
AGESA_STATUS agesa_Reset (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
AGESA_STATUS agesa_RunFuncOnAp (UINT32 Func, UINT32 Data, VOID *ConfigPtr);

#endif /* CALLOUTS_AMD_AGESA_H */
