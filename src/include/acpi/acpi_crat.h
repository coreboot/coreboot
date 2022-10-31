/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ACPI_CRAT_H__
#define __ACPI_CRAT_H__

#include <stdint.h>

enum crat_entry_type {
	CRAT_HSA_PROC_UNIT_TYPE,
	CRAT_MEMORY_TYPE,
	CRAT_CACHE_TYPE,
	CRAT_TLB_TYPE,
	CRAT_FPU_TYPE,
	CRAT_IO_TYPE,
	CRAT_MAX_TYPE,
};

#define CRAT_HSA_PR_FLAG_EN_SHFT		0
#define CRAT_HSA_PR_FLAG_EN			(0x1 << CRAT_HSA_PR_FLAG_EN_SHFT)
#define CRAT_HSA_PR_FLAG_HOT_PLUG_SHFT		1
#define CRAT_HSA_PR_FLAG_HOT_PLUG		(0x1 << CRAT_HSA_PR_FLAG_HOT_PLUG_SHFT)
#define CRAT_HSA_PR_FLAG_CPU_PRES_SHFT		2
#define CRAT_HSA_PR_FLAG_CPU_PRES		(0x1 << CRAT_HSA_PR_FLAG_CPU_PRES_SHFT)
#define CRAT_HSA_PR_FLAG_GPU_PRES_SHFT		3
#define CRAT_HSA_PR_FLAG_GPU_PRES		(0x1 << CRAT_HSA_PR_FLAG_GPU_PRES_SHFT)
#define CRAT_HSA_PR_FLAG_CRAT_HSAMMU_SHFT	4
#define CRAT_HSA_PR_FLAG_CRAT_HSAMMU		(0x1 << CRAT_HSA_PR_FLAG_CRAT_HSAMMU_SHFT)
#define CRAT_HSA_PR_FLAG_VAL_LIM_SHFT		5
#define CRAT_HSA_PR_FLAG_VAL_LIM		(0x7 << CRAT_HSA_PR_FLAG_VAL_LIM_SHFT)
#define CRAT_HSA_PR_FLAG_ATOM_OPS_SHFT		8
#define CRAT_HSA_PR_FLAG_ATOM_OPS		(0x3 << CRAT_HSA_PR_FLAG_ATOM_OPS_SHFT)
#define CRAT_HSA_PR_FLAG_SMT_CAPS_SHFT		10
#define CRAT_HSA_PR_FLAG_SMT_CAPS		(0x3 << CRAT_HSA_PR_FLAG_SMT_CAPS_SHFT)

/* CRAT HSA Processing Unit Affinity Structure */
struct crat_hsa_processing_unit {
	uint8_t type;
	uint8_t length;
	uint16_t reserved;
	uint32_t flags;
	uint32_t proximity_node;
	uint32_t processor_id_low;
	uint16_t num_cpu_cores;
	uint16_t num_simd_cores;
	uint16_t max_waves_simd;
	uint16_t io_count;
	uint16_t hsa_capability;
	uint16_t lds_size_in_kb;
	uint8_t wave_front_size;
	uint8_t num_shader_banks;
	uint16_t uengine_identifier;
	uint8_t num_arrays;
	uint8_t num_cu_per_array;
	uint8_t num_simd_per_cu;
	uint8_t max_slots_scratch_cu;
	uint8_t reserved1[4];
} __packed;

#define CRAT_L1_CACHE			1
#define CRAT_L2_CACHE			2
#define CRAT_L3_CACHE			3

#define CRAT_MEM_FLAG_EN_SHFT		0
#define CRAT_MEM_FLAG_EN		(0x1 << CRAT_MEM_FLAG_EN_SHFT)
#define CRAT_MEM_FLAG_HOT_PLUG_SHFT	1
#define CRAT_MEM_FLAG_HOT_PLUG		(0x1 << CRAT_MEM_FLAG_HOT_PLUG_SHFT)
#define CRAT_MEM_FLAG_NV_SHFT		2
#define CRAT_MEM_FLAG_NV		(0x1 << CRAT_MEM_FLAG_NV_SHFT)
#define CRAT_MEM_FLAG_ATOM_OPS_SHFT	3
#define CRAT_MEM_FLAG_ATOM_OPS		(0x3 << CRAT_MEM_FLAG_ATOM_OPS_SHFT)

/* CRAT Memory Affinity Structure */
struct crat_memory {
	uint8_t type;
	uint8_t length;
	uint16_t reserved;
	uint32_t flags;
	uint32_t proximity_domain;
	uint32_t base_address_low;
	uint32_t base_address_high;
	uint32_t length_low;
	uint32_t length_high;
	uint32_t width;
	uint8_t reserved1[8];
} __packed;

#define CRAT_CACHE_FLAG_EN_SHFT			0
#define CRAT_CACHE_FLAG_EN			(0x1 << CRAT_CACHE_FLAG_EN_SHFT)
#define CRAT_CACHE_FLAG_DATA_CACHE_SHFT		1
#define CRAT_CACHE_FLAG_DATA_CACHE		(0x1 << CRAT_CACHE_FLAG_DATA_CACHE_SHFT)
#define CRAT_CACHE_FLAG_INSTR_CACHE_SHFT	2
#define CRAT_CACHE_FLAG_INSTR_CACHE		(0x1 << CRAT_CACHE_FLAG_INSTR_CACHE_SHFT)
#define CRAT_CACHE_FLAG_CPU_CACHE_SHFT		3
#define CRAT_CACHE_FLAG_CPU_CACHE		(0x1 << CRAT_CACHE_FLAG_CPU_CACHE_SHFT)
#define CRAT_CACHE_FLAG_SIMD_CACHE_SHFT		4
#define CRAT_CACHE_FLAG_SIMD_CACHE		(0x1 << CRAT_CACHE_FLAG_SIMD_CACHE_SHFT)
#define CRAT_CACHE_FLAG_GDS_SHFT		5
#define CRAT_CACHE_FLAG_GDS			(0x1 << CRAT_CACHE_FLAG_GDS_SHFT)
#define CRAT_CACHE_FLAG_ATOMIC_OPS_SHFT		6
#define CRAT_CACHE_FLAG_ATOMIC_OPS		(0x1 << CRAT_CACHE_FLAG_ATOMIC_OPS_SHFT)

/* CRAT Cache Affinity Structure */
struct crat_cache {
	uint8_t type;
	uint8_t length;
	uint16_t reserved;
	uint32_t flags;
	uint32_t proc_id_low;
	uint8_t sibling_map[32];
	uint32_t cache_size;
	uint8_t cache_level;
	uint8_t lines_per_tag;
	uint16_t cache_line_size;
	uint8_t associativity;
	uint8_t cache_properties;
	uint16_t cache_latency;
	uint8_t reserved1[8];
} __packed;

enum tlb_type {
	tlb_2m,
	tlb_4k,
	tlb_1g,
	tlb_type_max,
};

#define CRAT_TLB_FLAG_EN_SHFT			0
#define CRAT_TLB_FLAG_EN			(0x1 << CRAT_TLB_FLAG_EN_SHFT)
#define CRAT_TLB_FLAG_DATA_TLB_SHFT		1
#define CRAT_TLB_FLAG_DATA_TLB			(0x1 << CRAT_TLB_FLAG_DATA_TLB_SHFT)
#define CRAT_TLB_FLAG_INSTR_TLB_SHFT		2
#define CRAT_TLB_FLAG_INSTR_TLB			(0x1 << CRAT_TLB_FLAG_INSTR_TLB_SHFT)
#define CRAT_TLB_FLAG_CPU_TLB_SHFT		3
#define CRAT_TLB_FLAG_CPU_TLB			(0x1 << CRAT_TLB_FLAG_CPU_TLB_SHFT)
#define CRAT_TLB_FLAG_SIMD_TLB_SHFT		4
#define CRAT_TLB_FLAG_SIMD_TLB			(0x1 << CRAT_TLB_FLAG_SIMD_TLB_SHFT)
#define CRAT_TLB_FLAG_4K_BASE_256_SHFT		5
#define CRAT_TLB_FLAG_4K_BASE_256		(0x1 << CRAT_TLB_FLAG_4K_BASE_256_SHFT)
#define CRAT_TLB_FLAG_2MB_BASE_256_SHFT		7
#define CRAT_TLB_FLAG_2MB_BASE_256		(0x1 << CRAT_TLB_FLAG_2MB_BASE_256_SHFT)
#define CRAT_TLB_FLAG_1GB_BASE_256_SHFT		9
#define CRAT_TLB_FLAG_1GB_BASE_256		(0x1 << CRAT_TLB_FLAG_1GB_BASE_256_SHFT)

/* CRAT TLB Affinity Structure */
struct crat_tlb {
	uint8_t type;
	uint8_t length;
	uint16_t reserved;
	uint32_t flags;
	uint32_t proc_id_low;
	uint8_t sibling_map[32];
	uint32_t tlb_level;
	uint8_t data_tlb_2mb_assoc;
	uint8_t data_tlb_2mb_size;
	uint8_t instr_tlb_2mb_assoc;
	uint8_t instr_tlb_2mb_size;
	uint8_t data_tlb_4k_assoc;
	uint8_t data_tlb_4k_size;
	uint8_t instr_tlb_4k_assoc;
	uint8_t instr_tlb_4k_size;
	uint8_t data_tlb_1g_assoc;
	uint8_t data_tlb_1g_size;
	uint8_t instr_tlb_1g_assoc;
	uint8_t instr_tlb_1g_size;
	uint8_t reserved1[4];
} __packed;

#define CRAT_FPU_FLAG_EN_SHFT		0
#define CRAT_FPU_FLAG_EN		(0x1 << CRAT_TLB_FLAG_EN_SHFT)

/* CRAT FPU Affinity Structure */
struct crat_fpu {
	uint8_t type;
	uint8_t length;
	uint16_t reserved;
	uint32_t flags;
	uint32_t proc_id_low;
	uint8_t sibling_map[32];
	uint32_t fpu_size;
	uint8_t reserved1[16];
} __packed;

#define CRAT_IO_FLAG_EN_SHFT		0
#define CRAT_IO_FLAG_EN			(0x1 << CRAT_IO_FLAG_EN_SHFT)
#define CRAT_IO_FLAG_COHER_SHFT		1
#define CRAT_IO_FLAG_COHER		(0x1 << CRAT_IO_FLAG_COHER_SHFT)

/* CRAT IO Affinity Structure */
struct crat_io {
	uint8_t type;
	uint8_t length;
	uint16_t reserved;
	uint32_t flags;
	uint32_t proximity_domain_from;
	uint32_t proximity_domain_to;
	uint8_t io_type;
	uint8_t version_major;
	uint16_t version_minor;
	uint32_t minimum_latency;
	uint32_t maximum_latency;
	uint32_t minimum_bandwidth;
	uint32_t maximum_bandwidth;
	uint32_t recommended_transfer_size;
	uint8_t reserved1[24];
} __packed;

#endif /* __ACPI_CRAT_H__ */
