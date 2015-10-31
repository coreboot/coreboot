/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 * Copyright (C) 2014 Edward O'Callaghan <eocallaghan@alterapraxis.com>.
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

#ifndef _PLATFORM_GNB_PCIE_COMPLEX_H
#define _PLATFORM_GNB_PCIE_COMPLEX_H

#include <vendorcode/amd/agesa/f14/AGESA.h>
#include <vendorcode/amd/agesa/f14/Lib/amdlib.h>

/**
 * @brief Graphic NorthBridge (GNB) General Purpose Port (GPP)
 *
 * GNB_GPP_PORT?_PORT_PRESENT
 *  0:Disable 1:Enable
 *
 * GNB_GPP_PORT?_SPEED_MODE
 *  0:Auto 1:GEN1 2:GEN2
 *
 * GNB_GPP_PORT?_LINK_ASPM
 *  0:Disable 1:L0s 2:L1 3:L0s+L1
 *
 * GNB_GPP_PORT?_CHANNEL_TYPE -
 *  0:LowLoss(-3.5db) 1:HighLoss(-6db) 2:Half-swing(0db)
 *  3:Half-swing(-3.5db) 4:extended length (-6db) 5:extended length(-8db)
 *
 * GNB_GPP_PORT?_HOTPLUG_SUPPORT
 *  0:Disable 1:Basic 3:Enhanced
 */

/* GNB GPP 4 */
#define GNB_GPP_PORT4_PORT_PRESENT		1
#define GNB_GPP_PORT4_SPEED_MODE		2
#define GNB_GPP_PORT4_LINK_ASPM			3
#define GNB_GPP_PORT4_CHANNEL_TYPE		4
#define GNB_GPP_PORT4_HOTPLUG_SUPPORT	0

/* GNB GPP 5 */
#define GNB_GPP_PORT5_PORT_PRESENT		1
#define GNB_GPP_PORT5_SPEED_MODE		2
#define GNB_GPP_PORT5_LINK_ASPM			3
#define GNB_GPP_PORT5_CHANNEL_TYPE		4
#define GNB_GPP_PORT5_HOTPLUG_SUPPORT	0

/* GNB GPP 6 */
#define GNB_GPP_PORT6_PORT_PRESENT		1
#define GNB_GPP_PORT6_SPEED_MODE		2
#define GNB_GPP_PORT6_LINK_ASPM			3
#define GNB_GPP_PORT6_CHANNEL_TYPE		4
#define GNB_GPP_PORT6_HOTPLUG_SUPPORT	0

/* GNB GPP 7 */
#define GNB_GPP_PORT7_PORT_PRESENT		0
#define GNB_GPP_PORT7_SPEED_MODE		2
#define GNB_GPP_PORT7_LINK_ASPM			3
#define GNB_GPP_PORT7_CHANNEL_TYPE		4
#define GNB_GPP_PORT7_HOTPLUG_SUPPORT	0

/* GNB GPP 8 */
#define GNB_GPP_PORT8_PORT_PRESENT		1
#define GNB_GPP_PORT8_SPEED_MODE		2
#define GNB_GPP_PORT8_LINK_ASPM			3
#define GNB_GPP_PORT8_CHANNEL_TYPE		4
#define GNB_GPP_PORT8_HOTPLUG_SUPPORT	0

#endif /* _PLATFORM_GNB_PCIE_COMPLEX_H */
