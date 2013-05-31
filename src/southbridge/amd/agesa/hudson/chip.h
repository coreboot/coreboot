/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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

#ifndef HUDSON_CHIP_H
#define HUDSON_CHIP_H

struct southbridge_amd_agesa_hudson_config
{
	#if 1
	u32 ide0_enable : 1;
	u32 sata0_enable : 1;
	u32 boot_switch_sata_ide : 1;
	u32 hda_viddid;
	u8  gpp_configuration;
	u8  sd_mode;
	#endif
};

#endif /* HUDSON_CHIP_H */
