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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _PLATFORM_GNB_PCIE_COMPLEX_H
#define _PLATFORM_GNB_PCIE_COMPLEX_H

#include "Porting.h"
#include "AGESA.h"
#include "amdlib.h"
#include <cpu/amd/agesa/s3_resume.h>

//GNB GPP Port4
#define GNB_GPP_PORT4_PORT_PRESENT		1	//0:Disable 1:Enable
#define GNB_GPP_PORT4_SPEED_MODE		2	//0:Auto 1:GEN1 2:GEN2
#define GNB_GPP_PORT4_LINK_ASPM			3	//0:Disable 1:L0s 2:L1 3:L0s+L1
#define GNB_GPP_PORT4_CHANNEL_TYPE		4	//0:LowLoss(-3.5db) 1:HighLoss(-6db) 2:Half-swing(0db)
											 //3:Half-swing(-3.5db) 4:extended length (-6db) 5:extended length(-8db)
#define GNB_GPP_PORT4_HOTPLUG_SUPPORT	0	//0:Disable 1:Basic 3:Enhanced

//GNB GPP Port5
#define GNB_GPP_PORT5_PORT_PRESENT		1	//0:Disable 1:Enable
#define GNB_GPP_PORT5_SPEED_MODE		2	//0:Auto 1:GEN1 2:GEN2
#define GNB_GPP_PORT5_LINK_ASPM			3	//0:Disable 1:L0s 2:L1 3:L0s+L1
#define GNB_GPP_PORT5_CHANNEL_TYPE		4	//0:LowLoss(-3.5db) 1:HighLoss(-6db) 2:Half-swing(0db)
											 //3:Half-swing(-3.5db) 4:extended length (-6db) 5:extended length(-8db)
#define GNB_GPP_PORT5_HOTPLUG_SUPPORT	0	//0:Disable 1:Basic 3:Enhanced

//GNB GPP Port6
#define GNB_GPP_PORT6_PORT_PRESENT		1	//0:Disable 1:Enable
#define GNB_GPP_PORT6_SPEED_MODE		2	//0:Auto 1:GEN1 2:GEN2
#define GNB_GPP_PORT6_LINK_ASPM			3	//0:Disable 1:L0s 2:L1 3:L0s+L1
#define GNB_GPP_PORT6_CHANNEL_TYPE		4	//0:LowLoss(-3.5db) 1:HighLoss(-6db) 2:Half-swing(0db)
											 //3:Half-swing(-3.5db) 4:extended length (-6db) 5:extended length(-8db)
#define GNB_GPP_PORT6_HOTPLUG_SUPPORT	0	//0:Disable 1:Basic 3:Enhanced

//GNB GPP Port7
#define GNB_GPP_PORT7_PORT_PRESENT		1	//0:Disable 1:Enable
#define GNB_GPP_PORT7_SPEED_MODE		2	//0:Auto 1:GEN1 2:GEN2
#define GNB_GPP_PORT7_LINK_ASPM			3	//0:Disable 1:L0s 2:L1 3:L0s+L1
#define GNB_GPP_PORT7_CHANNEL_TYPE		4	//0:LowLoss(-3.5db) 1:HighLoss(-6db) 2:Half-swing(0db)
											 //3:Half-swing(-3.5db) 4:extended length (-6db) 5:extended length(-8db)
#define GNB_GPP_PORT7_HOTPLUG_SUPPORT	0	//0:Disable 1:Basic 3:Enhanced

//GNB GPP Port8
#define GNB_GPP_PORT8_PORT_PRESENT		1	//0:Disable 1:Enable
#define GNB_GPP_PORT8_SPEED_MODE		2	//0:Auto 1:GEN1 2:GEN2
#define GNB_GPP_PORT8_LINK_ASPM			3	//0:Disable 1:L0s 2:L1 3:L0s+L1
#define GNB_GPP_PORT8_CHANNEL_TYPE		4	//0:LowLoss(-3.5db) 1:HighLoss(-6db) 2:Half-swing(0db)
											 //3:Half-swing(-3.5db) 4:extended length (-6db) 5:extended length(-8db)
#define GNB_GPP_PORT8_HOTPLUG_SUPPORT	0	//0:Disable 1:Basic 3:Enhanced

VOID
OemCustomizeInitEarly (
	IN	OUT AMD_EARLY_PARAMS	*InitEarly
	);

#endif //_PLATFORM_GNB_PCIE_COMPLEX_H
