/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/cpu.h>
#include <soc/cpu.h>
#include <soc/soc_util.h>
#include <types.h>

enum soc_type get_soc_type(void)
{
	uint32_t cpuid = cpuid_eax(1);

	if (cpuid_match(cpuid, PHOENIX_A0_CPUID, CPUID_ALL_STEPPINGS_MASK))
		return SOC_PHOENIX;


	if (cpuid_match(cpuid, PHOENIX2_A0_CPUID, CPUID_ALL_STEPPINGS_MASK))
		return SOC_PHOENIX2;

	return SOC_UNKNOWN;
}
