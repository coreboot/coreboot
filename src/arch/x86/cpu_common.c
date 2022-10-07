/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/cpu.h>
#include <types.h>

#if ENV_X86_32
/* Standard macro to see if a specific flag is changeable */
static inline int flag_is_changeable_p(uint32_t flag)
{
	uint32_t f1, f2;

	asm(
		"pushfl\n\t"
		"pushfl\n\t"
		"popl %0\n\t"
		"movl %0,%1\n\t"
		"xorl %2,%0\n\t"
		"pushl %0\n\t"
		"popfl\n\t"
		"pushfl\n\t"
		"popl %0\n\t"
		"popfl\n\t"
		: "=&r" (f1), "=&r" (f2)
		: "ir" (flag));
	return ((f1^f2) & flag) != 0;
}

/* Probe for the CPUID instruction */
int cpu_have_cpuid(void)
{
	return flag_is_changeable_p(X86_EFLAGS_ID);
}

#else

int cpu_have_cpuid(void)
{
	return 1;
}
#endif

unsigned int cpu_cpuid_extended_level(void)
{
	return cpuid_eax(0x80000000);
}

int cpu_phys_address_size(void)
{
	if (!(cpu_have_cpuid()))
		return 32;

	if (cpu_cpuid_extended_level() >= 0x80000008)
		return cpuid_eax(0x80000008) & 0xff;

	if (cpuid_edx(1) & (CPUID_FEATURE_PAE | CPUID_FEATURE_PSE36))
		return 36;
	return 32;
}

/*
 * Get processor id using cpuid eax=1
 * return value in EAX register
 */
uint32_t cpu_get_cpuid(void)
{
	return cpuid_eax(1);
}

/*
 * Get processor feature flag using cpuid eax=1
 * return value in ECX register
 */
uint32_t cpu_get_feature_flags_ecx(void)
{
	return cpuid_ecx(1);
}

/*
 * Get processor feature flag using cpuid eax=1
 * return value in EDX register
 */
uint32_t cpu_get_feature_flags_edx(void)
{
	return cpuid_edx(1);
}

enum cpu_type cpu_check_deterministic_cache_cpuid_supported(void)
{
	struct cpuid_result res;

	if (cpu_is_intel()) {
		res = cpuid(0);
		if (res.eax < 4)
			return CPUID_COMMAND_UNSUPPORTED;
		return CPUID_TYPE_INTEL;
	} else if (cpu_is_amd()) {
		res = cpuid(0x80000000);
		if (res.eax < 0x80000001)
			return CPUID_COMMAND_UNSUPPORTED;

		res = cpuid(0x80000001);
		if (!(res.ecx & (1 << 22)))
			return CPUID_COMMAND_UNSUPPORTED;

		return CPUID_TYPE_AMD;
	} else {
		return CPUID_TYPE_INVALID;
	}
}

static uint32_t cpu_get_cache_info_leaf(void)
{
	uint32_t leaf = (cpu_check_deterministic_cache_cpuid_supported() == CPUID_TYPE_AMD) ?
				DETERMINISTIC_CACHE_PARAMETERS_CPUID_AMD :
				DETERMINISTIC_CACHE_PARAMETERS_CPUID_IA;

	return leaf;
}

size_t cpu_get_cache_ways_assoc_info(const struct cpu_cache_info *info)
{
	if (!info)
		return 0;

	return info->num_ways;
}

uint8_t cpu_get_cache_type(const struct cpu_cache_info *info)
{
	if (!info)
		return 0;

	return info->type;
}

uint8_t cpu_get_cache_level(const struct cpu_cache_info *info)
{
	if (!info)
		return 0;

	return info->level;
}

size_t cpu_get_cache_phy_partition_info(const struct cpu_cache_info *info)
{
	if (!info)
		return 0;

	return info->physical_partitions;
}

size_t cpu_get_cache_line_size(const struct cpu_cache_info *info)
{
	if (!info)
		return 0;

	return info->line_size;
}

size_t cpu_get_cache_sets(const struct cpu_cache_info *info)
{
	if (!info)
		return 0;

	return info->num_sets;
}

bool cpu_is_cache_full_assoc(const struct cpu_cache_info *info)
{
	if (!info)
		return false;

	return info->fully_associative;
}

size_t cpu_get_max_cache_share(const struct cpu_cache_info *info)
{
	if (!info)
		return 0;

	return info->num_cores_shared;
}

size_t get_cache_size(const struct cpu_cache_info *info)
{
	if (!info)
		return 0;

	return info->num_ways * info->physical_partitions * info->line_size * info->num_sets;
}

bool fill_cpu_cache_info(uint8_t level, struct cpu_cache_info *info)
{
	if (!info)
		return false;

	uint32_t leaf = cpu_get_cache_info_leaf();
	if (!leaf)
		return false;

	struct cpuid_result cache_info_res = cpuid_ext(leaf, level);

	info->type = CPUID_CACHE_TYPE(cache_info_res);
	info->level = CPUID_CACHE_LEVEL(cache_info_res);
	info->num_ways = CPUID_CACHE_WAYS_OF_ASSOC(cache_info_res) + 1;
	info->num_sets = CPUID_CACHE_NO_OF_SETS(cache_info_res) + 1;
	info->line_size = CPUID_CACHE_COHER_LINE(cache_info_res) + 1;
	info->physical_partitions = CPUID_CACHE_PHYS_LINE(cache_info_res) + 1;
	info->num_cores_shared = CPUID_CACHE_SHARING_CACHE(cache_info_res) + 1;
	info->fully_associative = CPUID_CACHE_FULL_ASSOC(cache_info_res);
	info->size = get_cache_size(info);

	return true;
}
