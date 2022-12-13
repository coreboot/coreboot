/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/cpulib.h>
#include <intelblocks/msr.h>
#include <smbios.h>

/* Provide the max turbo frequency of the CPU */
unsigned int smbios_cpu_get_max_speed_mhz(void)
{
	return cpu_get_max_turbo_ratio() * CONFIG_CPU_BCLK_MHZ;
}
