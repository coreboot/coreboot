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

static uint32_t get_lpddr_tCKE(const uint32_t mem_clock_mhz)
{
	const struct timing_lookup lut[] = {
		{  533,  4 },
		{  666,  5 },
		{ fmax,  6 },
	};
	return lookup_timing(mem_clock_mhz, lut, ARRAY_SIZE(lut));
}

static uint32_t get_ddr_tCKE(const uint32_t mem_clock_mhz)
{
	const struct timing_lookup lut[] = {
		{  533,  3 },
		{  800,  4 },
		{  933,  5 },
		{ 1200,  6 },
		{ fmax,  7 },
	};
	return lookup_timing(mem_clock_mhz, lut, ARRAY_SIZE(lut));
}

uint32_t get_tCKE(const uint32_t mem_clock_mhz, const bool lpddr)
{
	return lpddr ? get_lpddr_tCKE(mem_clock_mhz) : get_ddr_tCKE(mem_clock_mhz);
}

uint32_t get_tXPDLL(const uint32_t mem_clock_mhz)
{
	const struct timing_lookup lut[] = {
		{  400, 10 },
		{  533, 13 },
		{  666, 16 },
		{  800, 20 },
		{  933, 23 },
		{ 1066, 26 },
		{ 1200, 29 },
		{ fmax, 32 },
	};
	return lookup_timing(mem_clock_mhz, lut, ARRAY_SIZE(lut));
}

uint32_t get_tAONPD(const uint32_t mem_clock_mhz)
{
	const struct timing_lookup lut[] = {
		{  400,  4 },
		{  533,  5 },
		{  666,  6 },
		{  800,  7 }, /* SNB had 8 */
		{  933,  8 },
		{ 1066, 10 },
		{ 1200, 11 },
		{ fmax, 12 },
	};
	return lookup_timing(mem_clock_mhz, lut, ARRAY_SIZE(lut));
}

uint32_t get_tMOD(const uint32_t mem_clock_mhz)
{
	const struct timing_lookup lut[] = {
		{  800, 12 },
		{  933, 14 },
		{ 1066, 16 },
		{ 1200, 18 },
		{ fmax, 20 },
	};
	return lookup_timing(mem_clock_mhz, lut, ARRAY_SIZE(lut));
}

uint32_t get_tXS_offset(const uint32_t mem_clock_mhz)
{
	return DIV_ROUND_UP(mem_clock_mhz, 100);
}

static uint32_t get_lpddr_tZQOPER(const uint32_t mem_clock_mhz)
{
	return (mem_clock_mhz * 360) / 1000;
}

static uint32_t get_ddr_tZQOPER(const uint32_t mem_clock_mhz)
{
	const struct timing_lookup lut[] = {
		{  800, 256 },
		{  933, 299 },
		{ 1066, 342 },
		{ 1200, 384 },
		{ fmax, 427 },
	};
	return lookup_timing(mem_clock_mhz, lut, ARRAY_SIZE(lut));
}

/* tZQOPER defines the period required for ZQCL after SR exit */
uint32_t get_tZQOPER(const uint32_t mem_clock_mhz, const bool lpddr)
{
	return lpddr ? get_lpddr_tZQOPER(mem_clock_mhz) : get_ddr_tZQOPER(mem_clock_mhz);
}

uint32_t get_tZQCS(const uint32_t mem_clock_mhz, const bool lpddr)
{
	return DIV_ROUND_UP(get_tZQOPER(mem_clock_mhz, lpddr), 4);
}
