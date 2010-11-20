/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

/** This file defines the SPD addresses for the mainboard. */

#include <spd.h>

static const u8 spd_addr[] = {
	//first node
	RC00, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
#if CONFIG_MAX_PHYSICAL_CPUS > 1
	//second node
	RC01, DIMM0, DIMM2, DIMM4, DIMM6, DIMM1, DIMM3, DIMM5, DIMM7,
#endif
#if CONFIG_MAX_PHYSICAL_CPUS > 2
	// third node
	RC02, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	// forth node
	RC03, DIMM0, DIMM2, DIMM4, DIMM6, DIMM1, DIMM3, DIMM5, DIMM7,
#endif
#if CONFIG_MAX_PHYSICAL_CPUS > 4
	RC04, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC05, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
#endif
#if CONFIG_MAX_PHYSICAL_CPUS > 6
	RC06, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC07, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
#endif
#if CONFIG_MAX_PHYSICAL_CPUS > 8
	RC08, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC09, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC10, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC11, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
#endif
#if CONFIG_MAX_PHYSICAL_CPUS > 12
	RC12, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC13, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC14, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC15, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
#endif
#if CONFIG_MAX_PHYSICAL_CPUS > 16
	RC16, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC17, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC18, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC19, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
#endif
#if CONFIG_MAX_PHYSICAL_CPUS > 20
	RC20, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC21, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC22, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC23, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
#endif
#if CONFIG_MAX_PHYSICAL_CPUS > 24
	RC24, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC25, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC26, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC27, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC28, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC29, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC30, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC31, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
#endif
#if CONFIG_MAX_PHYSICAL_CPUS > 32
	RC32, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC33, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC34, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC35, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC36, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC37, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC38, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC39, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC40, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC41, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC42, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC43, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC44, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC45, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC46, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC47, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
#endif
#if CONFIG_MAX_PHYSICAL_CPUS > 48
	RC48, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC49, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC50, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC51, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC52, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC53, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC54, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC55, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC56, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC57, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC58, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC59, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC60, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC61, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC62, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
	RC63, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0,
#endif
};
