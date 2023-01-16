/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_crat.h>
#include <acpi/acpi_ivrs.h>
#include <acpi/acpi.h>
#include <amdblocks/acpi.h>
#include <amdblocks/cpu.h>
#include <amdblocks/data_fabric.h>
#include <amdblocks/ioapic.h>
#include <cpu/amd/cpuid.h>
#include <cpu/cpu.h>
#include <device/device.h>
#include <device/mmio.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <FspGuids.h>
#include <soc/acpi.h>
#include <soc/data_fabric.h>
#include <soc/pci_devs.h>
#include <stdint.h>

static unsigned long gen_crat_hsa_entry(struct acpi_crat_header *crat, unsigned long current)
{
	struct crat_hsa_processing_unit *hsa_entry = (struct crat_hsa_processing_unit *)current;
	memset(hsa_entry, 0, sizeof(struct crat_hsa_processing_unit));

	hsa_entry->flags = CRAT_HSA_PR_FLAG_EN | CRAT_HSA_PR_FLAG_CPU_PRES;
	hsa_entry->wave_front_size = 4;
	hsa_entry->num_cpu_cores = get_cpu_count();
	hsa_entry->length = sizeof(struct crat_hsa_processing_unit);
	crat->total_entries++;

	current += hsa_entry->length;
	return current;
}

static unsigned long create_crat_memory_entry(uint32_t domain, uint64_t region_base,
					      uint64_t region_size, unsigned long current)
{
	struct crat_memory *mem_affinity = (struct crat_memory *)current;
	memset(mem_affinity, 0, sizeof(struct crat_memory));

	mem_affinity->type = CRAT_MEMORY_TYPE;
	mem_affinity->length = sizeof(struct crat_memory);
	mem_affinity->proximity_domain = 0;
	mem_affinity->base_address_low = region_base & 0xffffffff;
	mem_affinity->base_address_high = (region_base >> 32) & 0xffffffff;
	mem_affinity->length_low = region_size & 0xffffffff;
	mem_affinity->length_high = (region_size >> 32) & 0xffffffff;
	mem_affinity->flags = CRAT_MEM_FLAG_EN;
	mem_affinity->width = 64;

	current += mem_affinity->length;
	return current;
}

static unsigned long gen_crat_memory_entries(struct acpi_crat_header *crat,
					     unsigned long current)
{
	uint32_t dram_base_reg, dram_limit_reg, dram_hole_ctl;
	uint64_t memory_length, memory_base, hole_base, size_below_hole;
	size_t new_entries = 0;

	for (size_t dram_map_idx = 0; dram_map_idx < PICASSO_NUM_DRAM_REG;
	     dram_map_idx++) {
		dram_base_reg =
			data_fabric_read32(0, DF_DRAM_BASE(dram_map_idx), IOMS0_FABRIC_ID);

		if (dram_base_reg & DRAM_BASE_REG_VALID) {
			dram_limit_reg = data_fabric_read32(0, DF_DRAM_LIMIT(dram_map_idx),
								IOMS0_FABRIC_ID);
			memory_length =
				((dram_limit_reg & DRAM_LIMIT_ADDR) >> DRAM_LIMIT_ADDR_SHFT) + 1
				- ((dram_base_reg & DRAM_BASE_ADDR) >> DRAM_BASE_ADDR_SHFT);
			memory_length = memory_length << 28;
			memory_base = (uint64_t)(dram_base_reg & DRAM_BASE_ADDR)
				      << (28 - DRAM_BASE_ADDR_SHFT);

			if (memory_base == 0) {
				current =
					create_crat_memory_entry(0, 0ull, 0xa0000ull, current);
				memory_base = 1 * MiB;
				memory_length = memory_base;
				new_entries++;
			}

			if (dram_base_reg & DRAM_BASE_HOLE_EN) {
				dram_hole_ctl = data_fabric_read32(0, D18F0_DRAM_HOLE_CTL,
								       IOMS0_FABRIC_ID);
				hole_base = (dram_hole_ctl & DRAM_HOLE_CTL_BASE);
				size_below_hole = hole_base - memory_base;
				current = create_crat_memory_entry(0, memory_base,
								   size_below_hole, current);
				memory_length = (uint64_t)(((dram_limit_reg & DRAM_LIMIT_ADDR)
						  >> DRAM_LIMIT_ADDR_SHFT)
						 + 1 - 0x10)
						<< 28;
				memory_base = 0x100000000;
				new_entries++;
			}

			current = create_crat_memory_entry(0, memory_base, memory_length,
							   current);
			new_entries++;
		}
	}
	crat->total_entries += new_entries;
	return current;
}

static unsigned long add_crat_cache_entry(struct crat_cache **cache_affinity,
					  unsigned long current)
{
	*cache_affinity = (struct crat_cache *)current;
	memset(*cache_affinity, 0, sizeof(struct crat_cache));

	(*cache_affinity)->type = CRAT_CACHE_TYPE;
	(*cache_affinity)->length = sizeof(struct crat_cache);
	(*cache_affinity)->flags = CRAT_CACHE_FLAG_EN | CRAT_CACHE_FLAG_CPU_CACHE;

	current += sizeof(struct crat_cache);
	return current;
}

static uint8_t get_associativity(uint32_t encoded_associativity)
{
	uint8_t associativity = 0;

	switch (encoded_associativity) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		return encoded_associativity;
	case 5:
		associativity = 6;
		break;
	case 6:
		associativity = 8;
		break;
	case 8:
		associativity = 16;
		break;
	case 0xA:
		associativity = 32;
		break;
	case 0xB:
		associativity = 48;
		break;
	case 0xC:
		associativity = 64;
		break;
	case 0xD:
		associativity = 96;
		break;
	case 0xE:
		associativity = 128;
		break;
	case 0xF:
		associativity = 0xFF;
		break;
	default:
		return 0;
	}

	return associativity;
}

static unsigned long gen_crat_cache_entry(struct acpi_crat_header *crat, unsigned long current)
{
	size_t total_num_threads, num_threads_sharing0, num_threads_sharing1,
		num_threads_sharing2, num_threads_sharing3, thread, new_entries;
	struct cpuid_result cache_props0, cache_props1, cache_props2, cache_props3;
	uint8_t sibling_mask = 0;
	uint32_t l1_data_cache_ids, l1_inst_cache_ids, l2_cache_ids, l3_cache_ids;
	struct crat_cache *cache_affinity = NULL;

	total_num_threads = get_cpu_count();

	cache_props0 = cpuid_ext(CPUID_CACHE_PROPS, CACHE_PROPS_0);
	cache_props1 = cpuid_ext(CPUID_CACHE_PROPS, CACHE_PROPS_1);
	cache_props2 = cpuid_ext(CPUID_CACHE_PROPS, CACHE_PROPS_2);
	cache_props3 = cpuid_ext(CPUID_CACHE_PROPS, CACHE_PROPS_3);

	l1_data_cache_ids = cpuid_ecx(CPUID_L1_TLB_CACHE_IDS);
	l1_inst_cache_ids = cpuid_edx(CPUID_L1_TLB_CACHE_IDS);
	l2_cache_ids = cpuid_ecx(CPUID_L2_L3_CACHE_L2_TLB_IDS);
	l3_cache_ids = cpuid_edx(CPUID_L2_L3_CACHE_L2_TLB_IDS);

	num_threads_sharing0 =
		((cache_props0.eax & NUM_SHARE_CACHE_MASK) >> NUM_SHARE_CACHE_SHFT) + 1;
	num_threads_sharing1 =
		((cache_props1.eax & NUM_SHARE_CACHE_MASK) >> NUM_SHARE_CACHE_SHFT) + 1;
	num_threads_sharing2 =
		((cache_props2.eax & NUM_SHARE_CACHE_MASK) >> NUM_SHARE_CACHE_SHFT) + 1;
	num_threads_sharing3 =
		((cache_props3.eax & NUM_SHARE_CACHE_MASK) >> NUM_SHARE_CACHE_SHFT) + 1;

	new_entries = 0;
	for (thread = 0; thread < total_num_threads; thread++) {
		/* L1 data cache */
		if (thread % num_threads_sharing0 == 0) {
			current = add_crat_cache_entry(&cache_affinity, current);
			new_entries++;

			cache_affinity->flags |= CRAT_CACHE_FLAG_DATA_CACHE;
			cache_affinity->proc_id_low = thread;
			sibling_mask = 1;
			for (size_t sibling = 1; sibling < num_threads_sharing0; sibling++)
				sibling_mask = (sibling_mask << 1) + 1;
			cache_affinity->sibling_map[thread / 8] = sibling_mask << (thread % 8);
			cache_affinity->cache_properties =
				(cache_props0.edx & CACHE_INCLUSIVE_MASK) ? 2 : 0;
			cache_affinity->cache_size =
				(l1_data_cache_ids & L1_DC_SIZE_MASK) >> L1_DC_SIZE_SHFT;
			cache_affinity->cache_level = CRAT_L1_CACHE;
			cache_affinity->lines_per_tag =
				(l1_data_cache_ids & L1_DC_LINE_TAG_MASK)
				>> L1_DC_LINE_TAG_SHFT;
			cache_affinity->cache_line_size =
				(l1_data_cache_ids & L1_DC_LINE_SIZE_MASK)
				>> L1_DC_LINE_SIZE_SHFT;
			cache_affinity->associativity =
				(l1_data_cache_ids & L1_DC_ASSOC_MASK) >> L1_DC_ASSOC_SHFT;
			cache_affinity->cache_latency = 1;
		}

		/* L1 instruction cache */
		if (thread % num_threads_sharing1 == 0) {
			current = add_crat_cache_entry(&cache_affinity, current);
			new_entries++;

			cache_affinity->flags |= CRAT_CACHE_FLAG_INSTR_CACHE;
			cache_affinity->proc_id_low = thread;
			sibling_mask = 1;
			for (size_t sibling = 1; sibling < num_threads_sharing1; sibling++)
				sibling_mask = (sibling_mask << 1) + 1;
			cache_affinity->sibling_map[thread / 8] = sibling_mask << (thread % 8);
			cache_affinity->cache_properties =
				(cache_props1.edx & CACHE_INCLUSIVE_MASK) ? 2 : 0;
			cache_affinity->cache_size =
				(l1_inst_cache_ids & L1_IC_SIZE_MASK) >> L1_IC_SIZE_SHFT;
			cache_affinity->cache_level = CRAT_L1_CACHE;
			cache_affinity->lines_per_tag =
				(l1_inst_cache_ids & L1_IC_LINE_TAG_MASK)
				>> L1_IC_LINE_TAG_SHFT;
			cache_affinity->cache_line_size =
				(l1_inst_cache_ids & L1_IC_LINE_SIZE_MASK)
				>> L1_IC_LINE_SIZE_SHFT;
			cache_affinity->associativity =
				(l1_inst_cache_ids & L1_IC_ASSOC_MASK) >> L1_IC_ASSOC_SHFT;
			cache_affinity->cache_latency = 1;
		}

		/* L2 cache */
		if (thread % num_threads_sharing2 == 0) {
			current = add_crat_cache_entry(&cache_affinity, current);
			new_entries++;

			cache_affinity->flags |=
				CRAT_CACHE_FLAG_DATA_CACHE | CRAT_CACHE_FLAG_INSTR_CACHE;
			cache_affinity->proc_id_low = thread;
			sibling_mask = 1;
			for (size_t sibling = 1; sibling < num_threads_sharing2; sibling++)
				sibling_mask = (sibling_mask << 1) + 1;
			cache_affinity->sibling_map[thread / 8] = sibling_mask << (thread % 8);
			cache_affinity->cache_properties =
				(cache_props2.edx & CACHE_INCLUSIVE_MASK) ? 2 : 0;
			cache_affinity->cache_size =
				(l2_cache_ids & L2_DC_SIZE_MASK) >> L2_DC_SIZE_SHFT;
			cache_affinity->cache_level = CRAT_L2_CACHE;
			cache_affinity->lines_per_tag =
				(l2_cache_ids & L2_DC_LINE_TAG_MASK) >> L2_DC_LINE_TAG_SHFT;
			cache_affinity->cache_line_size =
				(l2_cache_ids & L2_DC_LINE_SIZE_MASK) >> L2_DC_LINE_SIZE_SHFT;
			cache_affinity->associativity = get_associativity(
				(l2_cache_ids & L2_DC_ASSOC_MASK) >> L2_DC_ASSOC_SHFT);
			cache_affinity->cache_latency = 1;
		}

		/* L3 cache */
		if (thread % num_threads_sharing3 == 0) {
			current = add_crat_cache_entry(&cache_affinity, current);
			new_entries++;

			cache_affinity->flags |=
				CRAT_CACHE_FLAG_DATA_CACHE | CRAT_CACHE_FLAG_INSTR_CACHE;
			cache_affinity->proc_id_low = thread;
			sibling_mask = 1;
			for (size_t sibling = 1; sibling < num_threads_sharing3; sibling++)
				sibling_mask = (sibling_mask << 1) + 1;
			cache_affinity->sibling_map[thread / 8] = sibling_mask << (thread % 8);
			cache_affinity->cache_properties =
				(cache_props0.edx & CACHE_INCLUSIVE_MASK) ? 2 : 0;
			cache_affinity->cache_size =
				((l3_cache_ids & L3_DC_SIZE_MASK) >> L3_DC_SIZE_SHFT) * 512;
			cache_affinity->cache_level = CRAT_L3_CACHE;
			cache_affinity->lines_per_tag =
				(l3_cache_ids & L3_DC_LINE_TAG_MASK) >> L3_DC_LINE_TAG_SHFT;
			cache_affinity->cache_line_size =
				(l3_cache_ids & L3_DC_LINE_SIZE_MASK) >> L3_DC_LINE_SIZE_SHFT;
			cache_affinity->associativity = get_associativity(
				(l3_cache_ids & L3_DC_ASSOC_MASK) >> L3_DC_ASSOC_SHFT);
			cache_affinity->cache_latency = 1;
		}
	}
	crat->total_entries += new_entries;
	return current;
}

static uint8_t get_tlb_size(enum tlb_type type, struct crat_tlb *crat_tlb_entry,
			    uint16_t raw_assoc_size)
{
	uint8_t tlbsize;

	if (raw_assoc_size >= 256) {
		tlbsize = (uint8_t)(raw_assoc_size / 256);

		if (type == tlb_2m)
			crat_tlb_entry->flags |= CRAT_TLB_FLAG_2MB_BASE_256;
		else if (type == tlb_4k)
			crat_tlb_entry->flags |= CRAT_TLB_FLAG_4K_BASE_256;
		else if (type == tlb_1g)
			crat_tlb_entry->flags |= CRAT_TLB_FLAG_1GB_BASE_256;
	} else {
		tlbsize = (uint8_t)(raw_assoc_size);
	}
	return tlbsize;
}

static unsigned long add_crat_tlb_entry(struct crat_tlb **tlb_affinity, unsigned long current)
{
	*tlb_affinity = (struct crat_tlb *)current;
	memset(*tlb_affinity, 0, sizeof(struct crat_tlb));

	(*tlb_affinity)->type = CRAT_TLB_TYPE;
	(*tlb_affinity)->length = sizeof(struct crat_tlb);
	(*tlb_affinity)->flags = CRAT_TLB_FLAG_EN | CRAT_TLB_FLAG_CPU_TLB;

	current += sizeof(struct crat_tlb);
	return current;
}

static unsigned long gen_crat_tlb_entry(struct acpi_crat_header *crat, unsigned long current)
{
	size_t total_num_threads, num_threads_sharing0, num_threads_sharing1,
		num_threads_sharing2, thread, new_entries;
	struct cpuid_result cache_props0, cache_props1, cache_props2;
	uint8_t sibling_mask = 0;
	uint32_t l1_tlb_2M4M_ids, l1_tlb_4K_ids, l2_tlb_2M4M_ids, l2_tlb_4K_ids, l1_tlb_1G_ids,
		l2_tlb_1G_ids;
	struct crat_tlb *tlb_affinity = NULL;

	total_num_threads = get_cpu_count();
	cache_props0 = cpuid_ext(CPUID_CACHE_PROPS, CACHE_PROPS_0);
	cache_props1 = cpuid_ext(CPUID_CACHE_PROPS, CACHE_PROPS_1);
	cache_props2 = cpuid_ext(CPUID_CACHE_PROPS, CACHE_PROPS_2);

	l1_tlb_2M4M_ids = cpuid_eax(CPUID_L1_TLB_CACHE_IDS);
	l2_tlb_2M4M_ids = cpuid_eax(CPUID_L2_L3_CACHE_L2_TLB_IDS);
	l1_tlb_4K_ids = cpuid_ebx(CPUID_L1_TLB_CACHE_IDS);
	l2_tlb_4K_ids = cpuid_ebx(CPUID_L2_L3_CACHE_L2_TLB_IDS);
	l1_tlb_1G_ids = cpuid_eax(CPUID_TLB_L1L2_1G_IDS);
	l2_tlb_1G_ids = cpuid_ebx(CPUID_TLB_L1L2_1G_IDS);

	num_threads_sharing0 =
		((cache_props0.eax & NUM_SHARE_CACHE_MASK) >> NUM_SHARE_CACHE_SHFT) + 1;
	num_threads_sharing1 =
		((cache_props1.eax & NUM_SHARE_CACHE_MASK) >> NUM_SHARE_CACHE_SHFT) + 1;
	num_threads_sharing2 =
		((cache_props2.eax & NUM_SHARE_CACHE_MASK) >> NUM_SHARE_CACHE_SHFT) + 1;

	new_entries = 0;
	for (thread = 0; thread < total_num_threads; thread++) {

		/* L1 data TLB */
		if (thread % num_threads_sharing0 == 0) {
			current = add_crat_tlb_entry(&tlb_affinity, current);
			new_entries++;

			tlb_affinity->flags |= CRAT_TLB_FLAG_DATA_TLB;
			tlb_affinity->proc_id_low = thread;
			sibling_mask = 1;
			for (size_t sibling = 1; sibling < num_threads_sharing0; sibling++)
				sibling_mask = (sibling_mask << 1) + 1;
			tlb_affinity->sibling_map[thread / 8] = sibling_mask << (thread % 8);
			tlb_affinity->tlb_level = CRAT_L1_CACHE;

			tlb_affinity->data_tlb_2mb_assoc =
				(l1_tlb_2M4M_ids & L1_DAT_TLB_2M4M_ASSOC_MASK)
				>> L1_DAT_TLB_2M4M_ASSOC_SHFT;
			tlb_affinity->data_tlb_2mb_size =
				get_tlb_size(tlb_2m, tlb_affinity,
					     (l1_tlb_2M4M_ids & L1_DAT_TLB_2M4M_SIZE_MASK)
						     >> L1_DAT_TLB_2M4M_SIZE_SHFT);

			tlb_affinity->data_tlb_4k_assoc =
				(l1_tlb_4K_ids & L1_DAT_TLB_4K_ASSOC_MASK)
				>> L1_DAT_TLB_4K_ASSOC_SHFT;
			tlb_affinity->data_tlb_4k_size =
				get_tlb_size(tlb_4k, tlb_affinity,
					     (l1_tlb_4K_ids & L1_DAT_TLB_4K_SIZE_MASK)
						     >> L1_DAT_TLB_4K_SIZE_SHFT);

			tlb_affinity->data_tlb_1g_assoc =
				(l1_tlb_1G_ids & L1_DAT_TLB_1G_ASSOC_MASK)
				>> L1_DAT_TLB_1G_ASSOC_SHFT;
			tlb_affinity->data_tlb_1g_size =
				get_tlb_size(tlb_1g, tlb_affinity,
					     (l1_tlb_1G_ids & L1_DAT_TLB_1G_SIZE_MASK)
						     >> L1_DAT_TLB_1G_SIZE_SHFT);
		}

		/* L1 instruction TLB */
		if (thread % num_threads_sharing1 == 0) {
			current = add_crat_tlb_entry(&tlb_affinity, current);
			new_entries++;

			tlb_affinity->flags |= CRAT_TLB_FLAG_INSTR_TLB;
			tlb_affinity->proc_id_low = thread;
			sibling_mask = 1;
			for (size_t sibling = 1; sibling < num_threads_sharing1; sibling++)
				sibling_mask = (sibling_mask << 1) + 1;
			tlb_affinity->sibling_map[thread / 8] = sibling_mask << (thread % 8);
			tlb_affinity->tlb_level = CRAT_L1_CACHE;
			tlb_affinity->instr_tlb_2mb_assoc =
				(l1_tlb_2M4M_ids & L1_INST_TLB_2M4M_ASSOC_MASK)
				>> L1_INST_TLB_2M4M_ASSOC_SHFT;
			tlb_affinity->instr_tlb_2mb_size =
				get_tlb_size(tlb_2m, tlb_affinity,
					     (l1_tlb_2M4M_ids & L1_INST_TLB_2M4M_SIZE_MASK)
						     >> L1_INST_TLB_2M4M_SIZE_SHFT);

			tlb_affinity->instr_tlb_4k_assoc =
				(l1_tlb_4K_ids & L1_INST_TLB_4K_ASSOC_MASK)
				>> L1_INST_TLB_4K_ASSOC_SHFT;
			tlb_affinity->instr_tlb_4k_size =
				get_tlb_size(tlb_4k, tlb_affinity,
					     (l1_tlb_4K_ids & L1_INST_TLB_4K_SIZE_MASK)
						     >> L1_INST_TLB_4K_SIZE_SHFT);

			tlb_affinity->instr_tlb_1g_assoc =
				(l1_tlb_1G_ids & L1_INST_TLB_1G_ASSOC_MASK)
				>> L1_INST_TLB_1G_ASSOC_SHFT;
			tlb_affinity->instr_tlb_1g_size =
				get_tlb_size(tlb_1g, tlb_affinity,
					     (l1_tlb_1G_ids & L1_INST_TLB_1G_SIZE_MASK)
						     >> L1_INST_TLB_1G_SIZE_SHFT);
		}

		/* L2 Data TLB */
		if (thread % num_threads_sharing2 == 0) {
			current = add_crat_tlb_entry(&tlb_affinity, current);
			new_entries++;

			tlb_affinity->flags |= CRAT_TLB_FLAG_DATA_TLB;
			tlb_affinity->proc_id_low = thread;
			sibling_mask = 1;
			for (size_t sibling = 1; sibling < num_threads_sharing2; sibling++)
				sibling_mask = (sibling_mask << 1) + 1;
			tlb_affinity->sibling_map[thread / 8] = sibling_mask << (thread % 8);
			tlb_affinity->tlb_level = CRAT_L2_CACHE;
			tlb_affinity->data_tlb_2mb_assoc =
				(l2_tlb_2M4M_ids & L2_DAT_TLB_2M4M_ASSOC_MASK)
				>> L2_DAT_TLB_2M4M_ASSOC_SHFT;
			tlb_affinity->data_tlb_2mb_size =
				get_tlb_size(tlb_2m, tlb_affinity,
					     (l2_tlb_2M4M_ids & L2_DAT_TLB_2M4M_SIZE_MASK)
						     >> L2_DAT_TLB_2M4M_SIZE_SHFT);

			tlb_affinity->data_tlb_4k_assoc =
				get_associativity((l2_tlb_4K_ids & L2_DAT_TLB_2M4M_ASSOC_MASK)
						  >> L2_DAT_TLB_4K_ASSOC_SHFT);
			tlb_affinity->data_tlb_4k_size =
				get_tlb_size(tlb_4k, tlb_affinity,
					     (l2_tlb_2M4M_ids & L2_DAT_TLB_4K_SIZE_MASK)
						     >> L2_DAT_TLB_4K_SIZE_SHFT);

			tlb_affinity->data_tlb_1g_assoc =
				get_associativity((l2_tlb_1G_ids & L2_DAT_TLB_1G_ASSOC_MASK)
						  >> L2_DAT_TLB_1G_ASSOC_SHFT);
			tlb_affinity->data_tlb_1g_size =
				get_tlb_size(tlb_1g, tlb_affinity,
					     (l2_tlb_1G_ids & L2_DAT_TLB_1G_SIZE_MASK)
						     >> L2_DAT_TLB_1G_SIZE_SHFT);
		}

		/* L2 Instruction TLB */
		if (thread % num_threads_sharing2 == 0) {
			current = add_crat_tlb_entry(&tlb_affinity, current);
			new_entries++;

			tlb_affinity->flags |= CRAT_TLB_FLAG_INSTR_TLB;
			tlb_affinity->proc_id_low = thread;
			sibling_mask = 1;
			for (size_t sibling = 1; sibling < num_threads_sharing2; sibling++)
				sibling_mask = (sibling_mask << 1) + 1;
			tlb_affinity->sibling_map[thread / 8] = sibling_mask << (thread % 8);
			tlb_affinity->tlb_level = CRAT_L2_CACHE;
			tlb_affinity->instr_tlb_2mb_assoc = get_associativity(
				(l2_tlb_2M4M_ids & L2_INST_TLB_2M4M_ASSOC_MASK)
				>> L2_INST_TLB_2M4M_ASSOC_SHFT);
			tlb_affinity->instr_tlb_2mb_size =
				get_tlb_size(tlb_2m, tlb_affinity,
					     (l2_tlb_2M4M_ids & L2_INST_TLB_2M4M_SIZE_MASK)
						     >> L2_INST_TLB_2M4M_SIZE_SHFT);

			tlb_affinity->instr_tlb_4k_assoc =
				get_associativity((l2_tlb_4K_ids & L2_INST_TLB_4K_ASSOC_MASK)
						  >> L2_INST_TLB_4K_ASSOC_SHFT);
			tlb_affinity->instr_tlb_4k_size =
				get_tlb_size(tlb_4k, tlb_affinity,
					     (l2_tlb_4K_ids & L2_INST_TLB_4K_SIZE_MASK)
						     >> L2_INST_TLB_4K_SIZE_SHFT);

			tlb_affinity->instr_tlb_1g_assoc =
				get_associativity((l2_tlb_1G_ids & L2_INST_TLB_1G_ASSOC_MASK)
						  >> L2_INST_TLB_1G_ASSOC_SHFT);
			tlb_affinity->instr_tlb_1g_size =
				get_tlb_size(tlb_1g, tlb_affinity,
					     (l2_tlb_1G_ids & L2_INST_TLB_1G_SIZE_MASK)
						     >> L2_INST_TLB_1G_SIZE_SHFT);
		}
	}

	crat->total_entries += new_entries;
	return current;
}

static unsigned long acpi_fill_crat(struct acpi_crat_header *crat, unsigned long current)
{
	current = gen_crat_hsa_entry(crat, current);
	current = gen_crat_memory_entries(crat, current);
	current = gen_crat_cache_entry(crat, current);
	current = gen_crat_tlb_entry(crat, current);
	crat->num_nodes++;

	return current;
}

uintptr_t agesa_write_acpi_tables(const struct device *device, uintptr_t current,
				  acpi_rsdp_t *rsdp)
{
	acpi_ivrs_t *ivrs;
	struct acpi_crat_header *crat;

	/* CRAT */
	current = acpi_align_current(current);
	crat = (struct acpi_crat_header *)current;
	acpi_create_crat(crat, acpi_fill_crat);
	current += crat->header.length;
	acpi_add_table(rsdp, crat);

	/* add ALIB SSDT from HOB */
	current = add_agesa_fsp_acpi_table(AMD_FSP_ACPI_ALIB_HOB_GUID, "ALIB", rsdp, current);

	/* IVRS */
	current = acpi_align_current(current);
	ivrs = (acpi_ivrs_t *)current;
	acpi_create_ivrs(ivrs, acpi_fill_ivrs);
	current += ivrs->header.length;
	acpi_add_table(rsdp, ivrs);

	/* Add SRAT, MSCT, SLIT if needed in the future */

	return current;
}
