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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

/**
 * This file defines the SPD addresses for the mainboard. Must be included in
 * romstage.c
 */

#define RC00 0
#define RC01 1
#define RC02 2
#define RC03 3
#define RC04 4
#define RC05 5
#define RC06 6
#define RC07 7
#define RC08 8
#define RC09 9
#define RC10 10
#define RC11 11
#define RC12 12
#define RC13 13
#define RC14 14
#define RC15 15
#define RC16 16
#define RC17 17
#define RC18 18
#define RC19 19
#define RC20 20
#define RC21 21
#define RC22 22
#define RC23 23
#define RC24 24
#define RC25 25
#define RC26 26
#define RC27 27
#define RC28 28
#define RC29 29
#define RC30 30
#define RC31 31

#define RC32 32
#define RC33 33
#define RC34 34
#define RC35 35
#define RC36 36
#define RC37 37
#define RC38 38
#define RC39 39
#define RC40 40
#define RC41 41
#define RC42 42
#define RC43 43
#define RC44 44
#define RC45 45
#define RC46 46
#define RC47 47
#define RC48 48
#define RC49 49
#define RC50 50
#define RC51 51
#define RC52 52
#define RC53 53
#define RC54 54
#define RC55 55
#define RC56 56
#define RC57 57
#define RC58 58
#define RC59 59
#define RC60 60
#define RC61 61
#define RC62 62
#define RC63 63


#define DIMM0 0x50
#define DIMM1 0x51
#define DIMM2 0x52
#define DIMM3 0x53
#define DIMM4 0x54
#define DIMM5 0x55
#define DIMM6 0x56
#define DIMM7 0x57


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

