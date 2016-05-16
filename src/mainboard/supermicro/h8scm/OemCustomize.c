/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 - 2012 Advanced Micro Devices, Inc.
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

#include <northbridge/amd/agesa/agesawrapper.h>
#include <PlatformMemoryConfiguration.h>

/*----------------------------------------------------------------------------------------
 *                        CUSTOMER OVERIDES MEMORY TABLE
 *----------------------------------------------------------------------------------------
 */

//reference BKDG Table87: works
#define F15_WL_SEED 0x3B //family15 BKDG recommand 3B RDIMM, 1A UDIMM.
#define SEED_A 0x54
#define SEED_B 0x4D
#define SEED_C 0x45
#define SEED_D 0x40

#define F10_WL_SEED 0x3B //family10 BKDG recommand 3B RDIMM, 1A UDIMM.

/*
 *  Platform Specific Overriding Table allows IBV/OEM to pass in platform information to AGESA
 *  (e.g. MemClk routing, the number of DIMM slots per channel,...). If PlatformSpecificTable
 *  is populated, AGESA will base its settings on the data from the table. Otherwise, it will
 *  use its default conservative settings.
 *  I am not sure whether DefaultPlatformMemoryConfiguration is necessary.
 *  If I comment out these code, H8SCM will still pass mem training.
 */
CONST PSO_ENTRY ROMDATA DefaultPlatformMemoryConfiguration[] = {
	WRITE_LEVELING_SEED(
			ANY_SOCKET, ANY_CHANNEL, ALL_DIMMS,
			F15_WL_SEED, F15_WL_SEED, F15_WL_SEED, F15_WL_SEED,
			F15_WL_SEED, F15_WL_SEED, F15_WL_SEED, F15_WL_SEED,
			F15_WL_SEED),

	HW_RXEN_SEED(
		ANY_SOCKET, CHANNEL_A, ALL_DIMMS,
		SEED_A, SEED_A, SEED_A, SEED_A, SEED_A, SEED_A, SEED_A, SEED_A,
		SEED_A),
	HW_RXEN_SEED(
		ANY_SOCKET, CHANNEL_B, ALL_DIMMS,
		SEED_B, SEED_B, SEED_B, SEED_B, SEED_B, SEED_B, SEED_B, SEED_B,
		SEED_B),
	HW_RXEN_SEED(
		ANY_SOCKET, CHANNEL_C, ALL_DIMMS,
		SEED_C, SEED_C, SEED_C, SEED_C, SEED_C, SEED_C, SEED_C, SEED_C,
		SEED_C),
	HW_RXEN_SEED(
		ANY_SOCKET, CHANNEL_D, ALL_DIMMS,
		SEED_D, SEED_D, SEED_D, SEED_D, SEED_D, SEED_D, SEED_D, SEED_D,
		SEED_D),

	NUMBER_OF_DIMMS_SUPPORTED (ANY_SOCKET, ANY_CHANNEL, 2), //max 3
	PSO_END
};

const struct OEM_HOOK OemCustomize = {
};
