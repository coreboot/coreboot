/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
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

#ifndef SOUTHBRIDGE_NVIDIA_MCP55_CHIP_H
#define SOUTHBRIDGE_NVIDIA_MCP55_CHIP_H

#include <device/device.h>

struct southbridge_nvidia_mcp55_config
{
	unsigned int ide0_enable : 1;
	unsigned int ide1_enable : 1;
	unsigned int sata0_enable : 1;
	unsigned int sata1_enable : 1;
	unsigned int mac_eeprom_smbus;
	unsigned int mac_eeprom_addr;
};

#endif
