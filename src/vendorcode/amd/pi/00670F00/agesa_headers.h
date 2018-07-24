/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Google, Inc.
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

#ifndef __AGESA_HEADERS_H__
#define __AGESA_HEADERS_H__

#define AGESA_HEADERS_ARE_WRAPPED
#pragma pack(push)
#include "Porting.h"
#include "AGESA.h"
#include "AMD.h"

#include "Include/PlatformMemoryConfiguration.h"
#include "Proc/Fch/FchPlatform.h"
#pragma pack(pop)
#undef AGESA_HEADERS_ARE_WRAPPED

#endif
