/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cache.h>
#include <arch/lib_helpers.h>
#include <arch/smc.h>
#include <console/console.h>
#include <smbios.h>
#include <stdio.h>
#include <string.h>

static void smbios_processor_id(u32 *processor_id)
{
	uint32_t jep106code, soc_revision;
	uint64_t midr_el1;

	if (smccc_supports_arch_soc_id()) {
		smccc_arch_soc_id(&jep106code, &soc_revision);
		processor_id[0] = jep106code;
		processor_id[1] = soc_revision;
	} else {
		midr_el1 = raw_read_midr_el1();
		processor_id[0] = midr_el1;
		processor_id[1] = 0;
	}
}

static int smbios_processor_manufacturer(u8 *start)
{
	char midr_el1_implementer;
	char buf[32];

	// [31:24] - Implementer code
	midr_el1_implementer = (raw_read_midr_el1() & 0xff000000) >> 24;

	snprintf(buf, sizeof(buf), "CPU implementer %x", midr_el1_implementer);
	return smbios_add_string(start, buf);
}

static int smbios_processor_name(u8 *start)
{
	uint16_t midr_el1_partnumber;
	char buf[32];

	// [15:4] - PartNum
	midr_el1_partnumber = (raw_read_midr_el1() & 0xfff0) >> 4;

	snprintf(buf, sizeof(buf), "ARMv8 Processor rev %d", midr_el1_partnumber);
	return smbios_add_string(start, buf);
}

#define MAX_CPUS_ENABLED(cpus) (cpus > 0xff ? 0xff : cpus)

/* NOTE: Not handling big.LITTLE clusters. Consider using MP services (not yet) or the DSU. */
int smbios_write_type4(unsigned long *current, int handle)
{
	static unsigned int cnt = 0;
	char buf[8];
	uint16_t characteristics = 0;
	unsigned int cpu_voltage;

	struct smbios_type4 *t = smbios_carve_table(*current, SMBIOS_PROCESSOR_INFORMATION,
						    sizeof(*t), handle);

	snprintf(buf, sizeof(buf), "CPU%d", cnt++);
	t->socket_designation = smbios_add_string(t->eos, buf);

	smbios_processor_id(t->processor_id);
	t->processor_manufacturer = smbios_processor_manufacturer(t->eos);
	t->processor_version = smbios_processor_name(t->eos);
	t->processor_family = 0xfe; /* Use processor_family2 field */
	t->processor_family2 = 0x101; /* ARMv8 */
	t->processor_type = SMBIOS_PROCESSOR_TYPE_CENTRAL;

	smbios_cpu_get_core_counts(&t->core_count2, &t->thread_count2);
	t->core_count = MAX_CPUS_ENABLED(t->core_count2);
	t->thread_count = MAX_CPUS_ENABLED(t->thread_count2);
	/* Assume we always enable all cores */
	t->core_enabled = t->core_count;
	t->core_enabled2 = t->core_count2;
	t->l1_cache_handle = 0xffff;
	t->l2_cache_handle = 0xffff;
	t->l3_cache_handle = 0xffff;
	t->serial_number = smbios_add_string(t->eos, smbios_processor_serial_number());
	t->status = SMBIOS_PROCESSOR_STATUS_CPU_ENABLED | SMBIOS_PROCESSOR_STATUS_POPULATED;
	t->processor_upgrade = PROCESSOR_UPGRADE_UNKNOWN;

	t->external_clock = smbios_processor_external_clock();
	if (t->external_clock == 0)
		t->external_clock = (raw_read_cntfrq_el0() / 1000 / 1000);

	t->current_speed = smbios_cpu_get_current_speed_mhz();

	/* This field identifies a capability for the system, not the processor itself. */
	t->max_speed = smbios_cpu_get_max_speed_mhz();

	/* TODO: Are "Enhanced Virtualization" (by EL2) and "Power/Performance Control" supported? */
	characteristics |= PROCESSOR_64BIT_CAPABLE;
	characteristics |= BIT(5); /* Execute Protection */

	if (t->core_count > 1)
		characteristics |= PROCESSOR_MULTI_CORE;
	if (t->thread_count > 1)
		characteristics |= BIT(4); /* BIT4: Hardware Thread */
	if (smccc_supports_arch_soc_id())
		characteristics |= BIT(9); /* Arm64 SoC ID */

	t->processor_characteristics = characteristics | smbios_processor_characteristics();

	cpu_voltage = smbios_cpu_get_voltage();
	if (cpu_voltage > 0)
		t->voltage = 0x80 | cpu_voltage;

	const int len = smbios_full_table_len(&t->header, t->eos);
	*current += len;
	return len;
}

int smbios_write_type7_cache_parameters(unsigned long *current,
					int *handle,
					int *max_struct_size,
					struct smbios_type4 *type4)
{
	enum cache_level level = CACHE_L1;
	int h;
	int len = 0;

	while (1) {
		enum smbios_cache_type type;
		struct cache_info info;

		const u8 cache_type = cpu_get_cache_type(level);
		/* No more caches in the system */
		if (!cache_type)
			break;

		switch (cache_type) {
		case CACHE_INSTRUCTION:
			type = SMBIOS_CACHE_TYPE_INSTRUCTION;
			cpu_get_cache_info(level, cache_type, &info);
			break;
		case CACHE_DATA:
			type = SMBIOS_CACHE_TYPE_DATA;
			cpu_get_cache_info(level, cache_type, &info);
			break;
		case CACHE_SEPARATE:
			type = SMBIOS_CACHE_TYPE_DATA;
			cpu_get_cache_info(level, CACHE_DATA, &info);
			h = (*handle)++;
			update_max(len, *max_struct_size, smbios_write_type7(current, h,
				   level, smbios_cache_sram_type(), smbios_cache_associativity(info.associativity),
				   type, info.size, info.size));

			type = SMBIOS_CACHE_TYPE_INSTRUCTION;
			cpu_get_cache_info(level, CACHE_INSTRUCTION, &info);
			break;
		case CACHE_UNIFIED:
			type = SMBIOS_CACHE_TYPE_UNIFIED;
			cpu_get_cache_info(level, cache_type, &info);
			break;
		default:
			type = SMBIOS_CACHE_TYPE_UNKNOWN;
			info.size = info.associativity = 0;
			break;
		}

		h = (*handle)++;
		update_max(len, *max_struct_size, smbios_write_type7(current, h,
			   level, smbios_cache_sram_type(), smbios_cache_associativity(info.associativity),
			   type, info.size, info.size));

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
			default:
				break;
			}
		}

		level++;
	}

	return len;
}
