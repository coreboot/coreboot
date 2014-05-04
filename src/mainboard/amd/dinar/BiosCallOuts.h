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

#ifndef _BIOS_CALLOUT_H_
#define _BIOS_CALLOUT_H_

#include <northbridge/amd/agesa/def_callouts.h>
#include <northbridge/amd/agesa/family15/fam15_callouts.h>

/* CALLOUT Initialization */
AGESA_STATUS GetBiosCallout(UINT32 Func, UINT32 Data, VOID *ConfigPtr);

#define SB_GPIO_REG02   2
#define SB_GPIO_REG09   9
#define SB_GPIO_REG10   10
#define SB_GPIO_REG15   15
#define SB_GPIO_REG17   17
#define SB_GPIO_REG21   21
#define SB_GPIO_REG25   25
#define SB_GPIO_REG28   28

#endif //_BIOS_CALLOUT_H_
