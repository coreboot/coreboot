/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/cpulib.h>
#include <intelblocks/msr.h>
#include <smbios.h>

/* AlderLake bus clock is fixed at 100MHz */
#define ADL_BCLK		100

/* Provide the max turbo frequency of the CPU */
unsigned int smbios_cpu_get_max_speed_mhz(void)
{
	return cpu_get_max_turbo_ratio() * ADL_BCLK;
}
