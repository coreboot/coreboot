/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 coresystems GmbH
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef SOUTHBRIDGE_INTEL_I82801GX_CHIP_H
#define SOUTHBRIDGE_INTEL_I82801GX_CHIP_H

struct southbridge_intel_i82801gx_config {
	uint32_t ide_legacy_combined;
	uint32_t ide_enable_primary;
	uint32_t ide_enable_secondary;
	uint32_t sata_ahci;
};

extern struct chip_operations southbridge_intel_i82801gx_ops;

#endif				/* SOUTHBRIDGE_INTEL_I82801GX_CHIP_H */
