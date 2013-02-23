/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

/* Datasheet:
 *   - Name: Geode CS5530A I/O Companion Multi-Function South Bridge
 *   - URL: http://www.national.com/pf/CS/CS5530A.html
 *   - PDF: http://www.national.com/ds.cgi/CS/CS5530A.pdf
 *   - Revision: 1.1 (May 2001)
 */

/* Datasheet:
 *   - Name: AMD Geode CS5530A Companion Device Data Book
 *   - URL: http://www.amd.com/us-en/ConnectivitySolutions/ProductInformation/0,,50_2330_9863_9919,00.html
 *   - PDF: http://www.amd.com/files/connectivitysolutions/geode/5530a_db_v11.pdf
 *   - Date: October 2003
 *   - Publication ID: May 2001, Revision 1.1
 */

/* Datasheet:
 *   - Name: AMD Geode Solutions
 *           Integrated Processors, Companion Devices, and System Platforms
 *           (Geode CS5530 I/O Companion Multi-Function South Bridge)
 *   - URL: http://www.amd.com/us-en/ConnectivitySolutions/ProductInformation/0,,50_2330_9863_9919,00.html
 *   - PDF: http://www.amd.com/files/connectivitysolutions/geode/5530_db_v41.pdf
 *   - Date: September 2004
 */

/* Note: The CS3350 / CS3350A are mostly identical. This code handles both. */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include "cs5530.h"

void cs5530_enable(device_t dev)
{
}

struct chip_operations southbridge_amd_cs5530_ops = {
	CHIP_NAME("AMD Geode CS5530/CS5530A I/O Companion Southbridge")
	.enable_dev = cs5530_enable,
};
