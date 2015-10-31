/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 DMP Electronics Inc.
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

/********************************************************************
 * Change the vendor / device IDs to match the XGI Z9S VBIOS header.
 ********************************************************************/
#include <device/pci.h>
u32 map_oprom_vendev(u32 vendev)
{
	u32 new_vendev = vendev;

	switch (vendev) {
	case 0x18ca0020:
		new_vendev = 0x18ca0021;
		break;
	}

	return new_vendev;
}
