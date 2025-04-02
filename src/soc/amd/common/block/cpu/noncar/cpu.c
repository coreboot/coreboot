/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/cpu.h>
#include <amdblocks/mca.h>
#include <arch/cpuid.h>
#include <cpu/cpu.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/cpuid.h>
#include <cpu/amd/microcode.h>
#include <cpu/amd/msr.h>
#include <cpu/amd/mtrr.h>
#include <lib.h>
#include <smbios.h>
#include <soc/iomap.h>
#include <soc/msr.h>
#include <types.h>

#define UNSUPPORTED 0

#define UNINITIALIZED ((uint64_t)-1)

struct core_info {
	/* Core max boost frequency */
	uint32_t max_frequency;

	/* L3 Cache block unique ID & size (in bytes) */
	uint16_t l3_cache_uid;
	size_t l3_cache_size;
};

static struct core_info core_info_list[CONFIG_MAX_CPUS];

static union pstate_msr get_pstate0_msr(void)
{
	static union pstate_msr pstate_reg = { .raw = UNINITIALIZED };

	struct cpuid_result res;

	/* Check if we have already determined P-state support */
	if (pstate_reg.raw == UNINITIALIZED) {
		if (cpu_cpuid_extended_level() < CPUID_EXT_PM) {
			pstate_reg.pstate_en = UNSUPPORTED;
			return pstate_reg;
		}

		res = cpuid(CPUID_EXT_PM);

		if (!(res.edx & BIT(7))) { /* Hardware P-state control */
			pstate_reg.pstate_en = UNSUPPORTED;
			return pstate_reg;
		}

		pstate_reg.raw = rdmsr(PSTATE_MSR(get_pstate_0_reg())).raw;
	}

	return pstate_reg;
}

unsigned int smbios_cpu_get_current_speed_mhz(void)
{
	union pstate_msr pstate_reg = get_pstate0_msr();

	if (!pstate_reg.pstate_en)
		return 0;

	return get_pstate_core_freq(pstate_reg);
}

unsigned int smbios_cpu_get_voltage(void)
{
	union pstate_msr pstate_reg = get_pstate0_msr();

	if (!pstate_reg.pstate_en)
		return 0;

	return get_pstate_core_uvolts(pstate_reg) / 100000; /* uV to (10 * V) */
}

uint32_t get_pstate_0_reg(void)
{
	return 0;
}

uint32_t get_pstate_latency(void)
{
	return 0;
}

unsigned int smbios_processor_family(struct cpuid_result res)
{
	return 0x6b; /* Zen */
}

unsigned int smbios_processor_external_clock(void)
{
	return 100; /* 100 MHz */
}

void set_cstate_io_addr(void)
{
	msr_t cst_addr;

	cst_addr.hi = 0;
	cst_addr.lo = ACPI_CSTATE_CONTROL;
	wrmsr(MSR_CSTATE_ADDRESS, cst_addr);
}

/* Number of most significant physical address bits reserved for secure memory encryption */
unsigned int get_reserved_phys_addr_bits(void)
{
	if (!(rdmsr(SYSCFG_MSR).raw & SYSCFG_MSR_SMEE))
		return 0;

	return (cpuid_ebx(CPUID_EBX_MEM_ENCRYPT) & CPUID_EBX_MEM_ENCRYPT_ADDR_BITS_MASK) >>
			CPUID_EBX_MEM_ENCRYPT_ADDR_BITS_SHIFT;
}

unsigned int smbios_cpu_get_max_speed_mhz(void)
{
	uint32_t max_freq = 0;

	for (int i = 0; i < get_cpu_count(); i++) {
		struct core_info *core_info = &core_info_list[i];

		if (max_freq < core_info->max_frequency)
			max_freq = core_info->max_frequency;
	}
	return max_freq;
}

/*
 * On some SoCs like AMD Glinda, multiple unique L3 cache blocks exist,
 * each with a distinct UID and possibly different sizes. The default
 * method assumes a single shared L3 cache across all cores, which is
 * inaccurate. This function aggregates sizes of all uniquely identified
 * L3 cache blocks to compute the correct total L3 size.
 */
bool soc_fill_cpu_cache_info(uint8_t level, struct cpu_cache_info *info)
{
	if (level != CACHE_L3)
		return x86_get_cpu_cache_info(level, info);

	if (!info)
		return false;

	uint32_t total_cache = 0;
	bool seen_cache_ids[CONFIG_MAX_CPUS] = {false};

	x86_get_cpu_cache_info(level, info);

	/*
	 * To calculate the total L3 cache size, iterate over core_info_list and add up the sizes
	 * of the L3 cache blocks with unique cache ID.
	 */
	for (int i = 0; i < get_cpu_count(); i++) {
		struct core_info *cache = &core_info_list[i];

		printk(BIOS_SPEW, "CPU %d: Cache Level: %d, Cache Size: %zu bytes\n",
		       i, cache->l3_cache_uid, cache->l3_cache_size);

		if (!seen_cache_ids[cache->l3_cache_uid]) {
			total_cache += cache->l3_cache_size;
			seen_cache_ids[cache->l3_cache_uid] = true;
		}
	}

	info->num_cores_shared = get_cpu_count();
	info->size = total_cache;

	printk(BIOS_SPEW, "Total cache at level: %d is:%zu\n", level, info->size);
	return true;
}

static uint32_t get_max_boost_frequency(void)
{
	msr_t msr = rdmsr(MSR_CPPC_CAPABILITY_1);
	uint16_t nominal_perf = (msr.lo >> SHIFT_CPPC_CAPABILITY_1_NOMINAL_PERF) & 0xff;
	uint16_t max_perf = (msr.lo >> SHIFT_CPPC_CAPABILITY_1_HIGHEST_PERF) & 0xff;

	return (smbios_cpu_get_current_speed_mhz() * max_perf)/nominal_perf;
}

static void ap_stash_core_info(void)
{
	unsigned int cpuid_cpu_id = cpuid_ebx(CPUID_EBX_CORE_ID) & 0xff;

	const uint8_t level = CACHE_L3;
	struct cpu_cache_info info;
	x86_get_cpu_cache_info(level, &info);

	struct core_info *core_info = &core_info_list[cpu_index()];
	core_info->l3_cache_uid = cpuid_cpu_id >> log2(info.num_cores_shared);
	core_info->l3_cache_size = info.size;

	core_info->max_frequency = get_max_boost_frequency();
}

void amd_cpu_init(struct device *dev)
{
	if (CONFIG(SOC_AMD_COMMON_BLOCK_MCA_COMMON))
		check_mca();

	set_cstate_io_addr();

	if (CONFIG(SOC_AMD_COMMON_BLOCK_UCODE))
		amd_apply_microcode_patch();

	if (CONFIG(SOC_FILL_CPU_CACHE_INFO))
		ap_stash_core_info();
}
