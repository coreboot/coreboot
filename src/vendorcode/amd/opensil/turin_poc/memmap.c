/* SPDX-License-Identifier: GPL-2.0-only */

#include <SilCommon.h>
#include <Sil-api.h> // needed above ApobCmn.h
#include <ApobCmn.h>
#include <xPRF-api.h>

#include "../opensil.h"

_Static_assert(sizeof(uint32_t) == sizeof(((MEMORY_HOLE_DESCRIPTOR){0}).Type),
	"Unexpected size of MEMORY_HOLE_TYPES in the MEMORY_HOLE_DESCRIPTOR "
	"struct which doesn't match the code in drivers/amd/opensil/memmap.c");

const char *opensil_get_hole_info_type(uint32_t type)
{
	const struct hole_type {
		MEMORY_HOLE_TYPES type;
		const char *string;
	} types[] = {
		{UMA, "UMA"},
		{MMIO, "MMIO"},
		{PrivilegedDRAM, "PrivilegedDRAM"},
		{Reserved1TbRemap, "Reserved1TbRemap"},
		{ReservedSLink, "ReservedSLink"},
		{ReservedSLinkAlignment, "ReservedSLinkAlignment"},
		{ReservedDrtm, "ReservedDrtm"},
		{ReservedCvip, "ReservedCvip"},
		{ReservedSmuFeatures, "ReservedSmuFeatures"},
		{ReservedFwtpm, "ReservedFwtpm"},
		{ReservedMpioC20, "ReservedMpioC20"},
		{ReservedNbif, "ReservedNbif"},
		{ReservedCxl, "ReservedCxl"},
		{ReservedCxlAlignment, "ReservedCxlAlignment"},
		{ReservedCpuTmr, "ReservedCpuTmr"},
		{ReservedRasEinj, "ReservedRasEinj"},
		{MaxMemoryHoleTypes, "MaxMemoryHoleTypes"},
	};

	int i;
	MEMORY_HOLE_TYPES enum_type = (MEMORY_HOLE_TYPES)type; // Cast int to enum
	for (i = 0; i < ARRAY_SIZE(types); i++)
		if (enum_type == types[i].type)
			return types[i].string;
	return "Unknown type";
}

void opensil_get_hole_info(uint32_t *n_holes, uint64_t *top_of_mem, void **hole_info)
{
        SIL_STATUS status = xPrfGetSystemMemoryMap(n_holes, top_of_mem, hole_info);
	SIL_STATUS_report("xPrfGetSystemMemoryMap", status);
	// Make sure hole_info does not get initialized to something odd by xPRF on failure
	if (status != SilPass) {
		*hole_info = NULL;
		*n_holes = 0;
		*top_of_mem = 0;
	}
}
