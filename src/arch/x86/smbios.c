/* SPDX-License-Identifier: GPL-2.0-only */

#include <string.h>
#include <smbios.h>
#include <console/console.h>
#include <arch/cpu.h>
#include <cpu/x86/name.h>
#include <stdio.h>

static int smbios_cpu_vendor(u8 *start)
{
	if (cpu_have_cpuid()) {
		u32 tmp[4];
		const struct cpuid_result res = cpuid(0);
		tmp[0] = res.ebx;
		tmp[1] = res.edx;
		tmp[2] = res.ecx;
		tmp[3] = 0;
		return smbios_add_string(start, (const char *)tmp);
	} else {
		return smbios_add_string(start, "Unknown");
	}
}

static int smbios_processor_name(u8 *start)
{
	u32 tmp[13];
	const char *str = "Unknown Processor Name";
	if (cpu_have_cpuid()) {
		int i;
		struct cpuid_result res;
		if (cpu_cpuid_extended_level() >= 0x80000004) {
			int j = 0;
			for (i = 0; i < 3; i++) {
				res = cpuid(0x80000002 + i);
				tmp[j++] = res.eax;
				tmp[j++] = res.ebx;
				tmp[j++] = res.ecx;
				tmp[j++] = res.edx;
			}
			tmp[12] = 0;
			str = (const char *)tmp;
		}
	}
	return smbios_add_string(start, str);
}

static int get_socket_type(void)
{
	if (CONFIG(CPU_INTEL_SLOT_1))
		return PROCESSOR_UPGRADE_SLOT_1;
	if (CONFIG(CPU_INTEL_SOCKET_MPGA604))
		return PROCESSOR_UPGRADE_SOCKET_MPGA604;
	if (CONFIG(CPU_INTEL_SOCKET_LGA775))
		return PROCESSOR_UPGRADE_SOCKET_LGA775;
	if (CONFIG(SOC_INTEL_ALDERLAKE))
		return PROCESSOR_UPGRADE_SOCKET_LGA1700;
	if (CONFIG(SOC_INTEL_METEORLAKE))
		return PROCESSOR_UPGRADE_OTHER;
	if (CONFIG(SOC_INTEL_SKYLAKE_SP))
		return PROCESSOR_UPGRADE_SOCKET_LGA3647_1;
	if (CONFIG(SOC_INTEL_COOPERLAKE_SP))
		return PROCESSOR_UPGRADE_SOCKET_LGA4189;
	if (CONFIG(SOC_INTEL_SAPPHIRERAPIDS_SP))
		return PROCESSOR_UPGRADE_SOCKET_LGA4677;

	return PROCESSOR_UPGRADE_UNKNOWN;
}

unsigned int __weak smbios_processor_family(struct cpuid_result res)
{
	return (res.eax > 0) ? 0x0c : 0x6;
}

static size_t get_number_of_caches(size_t max_logical_cpus_sharing_cache)
{
	size_t number_of_cpus_per_package = 0;
	size_t max_logical_cpus_per_package = 0;
	struct cpuid_result res;

	if (!cpu_have_cpuid())
		return 1;

	res = cpuid(1);

	max_logical_cpus_per_package = (res.ebx >> 16) & 0xff;

	/* Check if it's last level cache */
	if (max_logical_cpus_sharing_cache == max_logical_cpus_per_package)
		return 1;

	if (cpuid_get_max_func() >= 0xb) {
		res = cpuid_ext(0xb, 1);
		number_of_cpus_per_package = res.ebx & 0xff;
	} else {
		number_of_cpus_per_package = max_logical_cpus_per_package;
	}

	return number_of_cpus_per_package / max_logical_cpus_sharing_cache;
}

#define MAX_CPUS_ENABLED (CONFIG_MAX_CPUS > 0xff ? 0xff : CONFIG_MAX_CPUS)

int smbios_write_type4(unsigned long *current, int handle)
{
	unsigned int cpu_voltage;
	struct cpuid_result res;
	uint16_t characteristics = 0;
	static unsigned int cnt = 0;
	char buf[8];

	/* Provide sane defaults even for CPU without CPUID */
	res.eax = res.edx = 0;
	res.ebx = 0x10000;

	if (cpu_have_cpuid())
		res = cpuid(1);

	struct smbios_type4 *t = smbios_carve_table(*current, SMBIOS_PROCESSOR_INFORMATION,
						    sizeof(*t), handle);

	snprintf(buf, sizeof(buf), "CPU%d", cnt++);
	t->socket_designation = smbios_add_string(t->eos, buf);

	t->processor_id[0] = res.eax;
	t->processor_id[1] = res.edx;
	t->processor_manufacturer = smbios_cpu_vendor(t->eos);
	t->processor_version = smbios_processor_name(t->eos);
	t->processor_family = smbios_processor_family(res);
	t->processor_type = SMBIOS_PROCESSOR_TYPE_CENTRAL;
	/*
	 * If CPUID leaf 11 is available, calculate "core count" by dividing
	 * SMT_ID (logical processors in a core) by Core_ID (number of cores).
	 * This seems to be the way to arrive to a number of cores mentioned on
	 * ark.intel.com.
	 */
	if (cpu_have_cpuid() && cpuid_get_max_func() >= 0xb) {
		uint32_t leaf_b_cores = 0, leaf_b_threads = 0;
		res = cpuid_ext(0xb, 1);
		leaf_b_cores = res.ebx;
		res = cpuid_ext(0xb, 0);
		leaf_b_threads = res.ebx;
		/* if hyperthreading is not available, pretend this is 1 */
		if (leaf_b_threads == 0)
			leaf_b_threads = 1;

		t->core_count2 = leaf_b_cores / leaf_b_threads;
		t->core_count = t->core_count2 > 0xff ? 0xff : t->core_count2;
		t->thread_count2 = leaf_b_cores;
		t->thread_count = t->thread_count2 > 0xff ? 0xff : t->thread_count2;
	} else {
		t->core_count = (res.ebx >> 16) & 0xff;
		t->core_count2 = t->core_count;
		t->thread_count2 = t->core_count2;
		t->thread_count = t->thread_count2;
	}
	/* Assume we enable all the cores always, capped only by MAX_CPUS */
	t->core_enabled = MIN(t->core_count, MAX_CPUS_ENABLED);
	t->core_enabled2 = MIN(t->core_count2, CONFIG_MAX_CPUS);
	t->l1_cache_handle = 0xffff;
	t->l2_cache_handle = 0xffff;
	t->l3_cache_handle = 0xffff;
	t->serial_number = smbios_add_string(t->eos, smbios_processor_serial_number());
	t->status = SMBIOS_PROCESSOR_STATUS_CPU_ENABLED | SMBIOS_PROCESSOR_STATUS_POPULATED;
	t->processor_upgrade = get_socket_type();
	if (cpu_have_cpuid() && cpuid_get_max_func() >= 0x16) {
		t->current_speed = cpuid_eax(0x16); /* base frequency */
		t->external_clock = cpuid_ecx(0x16);
	} else {
		t->current_speed = smbios_cpu_get_current_speed_mhz();
		t->external_clock = smbios_processor_external_clock();
	}

	/* This field identifies a capability for the system, not the processor itself. */
	t->max_speed = smbios_cpu_get_max_speed_mhz();

	if (cpu_have_cpuid()) {
		res = cpuid(1);

		if ((res.ecx) & BIT(5))
			characteristics |= BIT(6); /* BIT6: Enhanced Virtualization */

		if ((res.edx) & BIT(28))
			characteristics |= BIT(4); /* BIT4: Hardware Thread */

		if (cpu_cpuid_extended_level() >= 0x80000001) {
			res = cpuid(0x80000001);

			if ((res.edx) & BIT(20))
				characteristics |= BIT(5); /* BIT5: Execute Protection */
		}
	}
	t->processor_characteristics = characteristics | smbios_processor_characteristics();
	cpu_voltage = smbios_cpu_get_voltage();
	if (cpu_voltage > 0)
		t->voltage = 0x80 | cpu_voltage;

	const int len = smbios_full_table_len(&t->header, t->eos);
	*current += len;
	return len;
}

/*
 * Parse the "Deterministic Cache Parameters" as provided by Intel in
 * leaf 4 or AMD in extended leaf 0x8000001d.
 *
 * @param current Pointer to memory address to write the tables to
 * @param handle Pointer to handle for the tables
 * @param max_struct_size Pointer to maximum struct size
 * @param type4 Pointer to SMBIOS type 4 structure
 */
int smbios_write_type7_cache_parameters(unsigned long *current,
					int *handle,
					int *max_struct_size,
					struct smbios_type4 *type4)
{
	unsigned int cnt = CACHE_L1D;
	int len = 0;

	if (!cpu_have_cpuid())
		return len;

	enum cpu_type dcache_cpuid = cpu_check_deterministic_cache_cpuid_supported();
	if (dcache_cpuid == CPUID_TYPE_INVALID || dcache_cpuid == CPUID_COMMAND_UNSUPPORTED) {
		printk(BIOS_DEBUG, "SMBIOS: Unknown CPU or CPU doesn't support Deterministic "
					"Cache CPUID leaf\n");
		return len;
	}

	while (1) {
		enum smbios_cache_associativity associativity;
		enum smbios_cache_type type;
		struct cpu_cache_info info;
		if (!fill_cpu_cache_info(cnt++, &info))
			continue;

		const u8 cache_type = info.type;
		const u8 level = info.level;
		const size_t assoc = info.num_ways;
		const size_t cache_share = info.num_cores_shared;
		const size_t cache_size = info.size * get_number_of_caches(cache_share);

		if (!cache_type)
			/* No more caches in the system */
			break;

		switch (cache_type) {
		case 1:
			type = SMBIOS_CACHE_TYPE_DATA;
			break;
		case 2:
			type = SMBIOS_CACHE_TYPE_INSTRUCTION;
			break;
		case 3:
			type = SMBIOS_CACHE_TYPE_UNIFIED;
			break;
		default:
			type = SMBIOS_CACHE_TYPE_UNKNOWN;
			break;
		}

		if (info.fully_associative)
			associativity = SMBIOS_CACHE_ASSOCIATIVITY_FULL;
		else
			associativity = smbios_cache_associativity(assoc);

		const int h = (*handle)++;

		update_max(len, *max_struct_size, smbios_write_type7(current, h,
			   level, smbios_cache_sram_type(), associativity,
			   type, cache_size, cache_size));

		if (type4) {
			switch (level) {
			case 1:
				type4->l1_cache_handle = h;
				break;
			case 2:
				type4->l2_cache_handle = h;
				break;
			case 3:
				type4->l3_cache_handle = h;
				break;
			}
		}
	};

	return len;
}
