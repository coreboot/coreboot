/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/cpu.h>
#include <smbios.h>

unsigned int smbios_cache_error_correction_type(u8 level)
{
	return SMBIOS_CACHE_ERROR_CORRECTION_SINGLE_BIT;
}

unsigned int smbios_cache_sram_type(void)
{
	return SMBIOS_CACHE_SRAM_TYPE_SYNCHRONOUS;
}

unsigned int smbios_cache_conf_operation_mode(u8 level)
{
	switch (level) {
	case 1:
		return SMBIOS_CACHE_OP_MODE_WRITE_BACK;
	case 2:
	case 3:
		return SMBIOS_CACHE_OP_MODE_VARIES_WITH_MEMORY_ADDRESS;
	default:
		return SMBIOS_CACHE_OP_MODE_UNKNOWN;
	}
}

unsigned int smbios_processor_family(struct cpuid_result res)
{
	return SMBIOS_PROCESSOR_FAMILY_XEON;
}

unsigned int smbios_processor_characteristics(void)
{
	return PROCESSOR_64BIT_CAPABLE|PROCESSOR_MULTI_CORE|PROCESSOR_POWER_PERFORMANCE_CONTROL;
}
