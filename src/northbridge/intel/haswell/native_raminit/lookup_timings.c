/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <commonlib/bsd/clamp.h>
#include <types.h>

#include "raminit_native.h"

struct timing_lookup {
	uint32_t clock;
	uint32_t value;
};

static uint32_t lookup_timing(
	const uint32_t mem_clock_mhz,
	const struct timing_lookup *const lookup,
	const size_t length)
{
	/* Fall back to the last index */
	size_t i;
	for (i = 0; i < length - 1; i++) {
		/* Account for imprecise frequency values */
		if ((mem_clock_mhz - 5) <= lookup[i].clock)
			break;
	}
	return lookup[i].value;
}

static const uint32_t fmax = UINT32_MAX;

uint8_t get_tCWL(const uint32_t mem_clock_mhz)
{
	const struct timing_lookup lut[] = {
		{  400,  5 },
		{  533,  6 },
		{  666,  7 },
		{  800,  8 },
		{  933,  9 },
		{ 1066, 10 },
		{ 1200, 11 },
		{ fmax, 12 },
	};
	return lookup_timing(mem_clock_mhz, lut, ARRAY_SIZE(lut));
}

/* tREFI = 7800 ns * DDR MHz */
uint32_t get_tREFI(const uint32_t mem_clock_mhz)
{
	return (mem_clock_mhz * 7800) / 1000;
}

uint32_t get_tXP(const uint32_t mem_clock_mhz)
{
	const struct timing_lookup lut[] = {
		{  400,  3 },
		{  666,  4 },
		{  800,  5 },
		{  933,  6 },
		{ 1066,  7 },
		{ fmax,  8 },
	};
	return lookup_timing(mem_clock_mhz, lut, ARRAY_SIZE(lut));
}
