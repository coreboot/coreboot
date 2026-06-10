/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/cpu.h>
#include <cpu/cpu.h>
#include <device/device.h>
#include <soc/cpu.h>

_Static_assert(CONFIG_MAX_CPUS == 24, "Do not override MAX_CPUS. To reduce the number of "
	"available cores, use the downcore_mode and disable_smt devicetree settings instead.");

static struct device_operations cpu_dev_ops = {
	.init = amd_cpu_init,
};

unsigned int smbios_cache_error_correction_type(u8 level)
{
	return SMBIOS_CACHE_ERROR_CORRECTION_MULTI_BIT;
}

unsigned int smbios_cache_sram_type(void)
{
	return SMBIOS_CACHE_SRAM_TYPE_PIPELINE_BURST;
}

unsigned int smbios_cache_conf_operation_mode(u8 level)
{
	return SMBIOS_CACHE_OP_MODE_WRITE_BACK;
}

u8 smbios_cache_speed(u8 level)
{
	return 1;
}

static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_AMD, GLINDA_A0_CPUID, CPUID_ALL_STEPPINGS_MASK },
	{ X86_VENDOR_AMD, GLINDA_B0_CPUID, CPUID_ALL_STEPPINGS_MASK },
	{ X86_VENDOR_AMD, FAEGAN_A0_CPUID, CPUID_ALL_STEPPINGS_MASK },
	CPU_TABLE_END
};

static const struct cpu_driver zen_5 __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
