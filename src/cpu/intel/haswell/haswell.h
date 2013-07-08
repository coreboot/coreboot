/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#ifndef _CPU_INTEL_HASWELL_H
#define _CPU_INTEL_HASWELL_H

#include <arch/cpu.h>

/* Haswell bus clock is fixed at 100MHz */
#define HASWELL_BCLK		100

#define CORE_THREAD_COUNT_MSR		0x35
#define IA32_FEATURE_CONTROL		0x3a
#define  CPUID_VMX			(1 << 5)
#define  CPUID_SMX			(1 << 6)
#define MSR_FEATURE_CONFIG		0x13c
#define MSR_FLEX_RATIO			0x194
#define  FLEX_RATIO_LOCK		(1 << 20)
#define  FLEX_RATIO_EN			(1 << 16)
#define IA32_PLATFORM_DCA_CAP		0x1f8
#define IA32_MISC_ENABLE		0x1a0
#define MSR_TEMPERATURE_TARGET		0x1a2
#define IA32_PERF_CTL 			0x199
#define IA32_THERM_INTERRUPT		0x19b
#define IA32_ENERGY_PERFORMANCE_BIAS	0x1b0
#define  ENERGY_POLICY_PERFORMANCE	0
#define  ENERGY_POLICY_NORMAL		6
#define  ENERGY_POLICY_POWERSAVE	15
#define IA32_PACKAGE_THERM_INTERRUPT	0x1b2
#define MSR_LT_LOCK_MEMORY		0x2e7
#define IA32_MC0_STATUS 		0x401

#define MSR_PIC_MSG_CONTROL		0x2e
#define MSR_PLATFORM_INFO		0xce
#define  PLATFORM_INFO_SET_TDP		(1 << 29)
#define MSR_PMG_CST_CONFIG_CONTROL	0xe2
#define MSR_PMG_IO_CAPTURE_BASE		0xe4

#define MSR_MISC_PWR_MGMT		0x1aa
#define  MISC_PWR_MGMT_EIST_HW_DIS	(1 << 0)
#define MSR_TURBO_RATIO_LIMIT		0x1ad
#define MSR_POWER_CTL			0x1fc

#define MSR_PKGC3_IRTL			0x60a
#define MSR_PKGC6_IRTL			0x60b
#define MSR_PKGC7_IRTL			0x60c
#define  IRTL_VALID			(1 << 15)
#define  IRTL_1_NS			(0 << 10)
#define  IRTL_32_NS			(1 << 10)
#define  IRTL_1024_NS			(2 << 10)
#define  IRTL_32768_NS			(3 << 10)
#define  IRTL_1048576_NS		(4 << 10)
#define  IRTL_33554432_NS		(5 << 10)
#define  IRTL_RESPONSE_MASK		(0x3ff)

/* long duration in low dword, short duration in high dword */
#define MSR_PKG_POWER_LIMIT		0x610
#define  PKG_POWER_LIMIT_MASK		0x7fff
#define  PKG_POWER_LIMIT_EN		(1 << 15)
#define  PKG_POWER_LIMIT_CLAMP		(1 << 16)
#define  PKG_POWER_LIMIT_TIME_SHIFT	17
#define  PKG_POWER_LIMIT_TIME_MASK	0x7f

#define MSR_PP0_CURRENT_CONFIG		0x601
#define MSR_VR_CURRENT_CONFIG		0x601
#define  PP0_CURRENT_LIMIT		(112 << 3) /* 112 A */
#define MSR_PP1_CURRENT_CONFIG		0x602
#define  PP1_CURRENT_LIMIT_SNB		(35 << 3) /* 35 A */
#define  PP1_CURRENT_LIMIT_IVB		(50 << 3) /* 50 A */
#define MSR_PKG_POWER_SKU_UNIT		0x606
#define MSR_PKG_POWER_SKU		0x614
#define MSR_PP0_POWER_LIMIT		0x638
#define MSR_PP1_POWER_LIMIT		0x640

#define MSR_CONFIG_TDP_NOMINAL		0x648
#define MSR_CONFIG_TDP_LEVEL1		0x649
#define MSR_CONFIG_TDP_LEVEL2		0x64a
#define MSR_CONFIG_TDP_CONTROL		0x64b
#define MSR_TURBO_ACTIVATION_RATIO	0x64c

/* P-state configuration */
#define PSS_MAX_ENTRIES			8
#define PSS_RATIO_STEP			2
#define PSS_LATENCY_TRANSITION		10
#define PSS_LATENCY_BUSMASTER		10

/* Region of SMM space is reserved for multipurpose use. It falls below
 * the IED region and above the SMM handler. */
#define RESERVED_SMM_SIZE CONFIG_SMM_RESERVED_SIZE
#define RESERVED_SMM_OFFSET \
	(CONFIG_SMM_TSEG_SIZE - CONFIG_IED_REGION_SIZE - RESERVED_SMM_SIZE)

/* Sanity check config options. */
#if (CONFIG_SMM_TSEG_SIZE <= (CONFIG_IED_REGION_SIZE + RESERVED_SMM_SIZE))
# error "CONFIG_SMM_TSEG_SIZE <= (CONFIG_IED_REGION_SIZE + RESERVED_SMM_SIZE)"
#endif
#if (CONFIG_SMM_TSEG_SIZE < 0x800000)
# error "CONFIG_SMM_TSEG_SIZE must at least be 8MiB"
#endif
#if ((CONFIG_SMM_TSEG_SIZE & (CONFIG_SMM_TSEG_SIZE - 1)) != 0)
# error "CONFIG_SMM_TSEG_SIZE is not a power of 2"
#endif
#if ((CONFIG_IED_REGION_SIZE & (CONFIG_IED_REGION_SIZE - 1)) != 0)
# error "CONFIG_IED_REGION_SIZE is not a power of 2"
#endif

#if !defined(__ROMCC__) // FIXME romcc should handle below constructs

#if defined(__PRE_RAM__)
struct pei_data;
struct rcba_config_instruction;
struct romstage_params {
	struct pei_data *pei_data;
	const void *gpio_map;
	const struct rcba_config_instruction *rcba_config;
	unsigned long bist;
};
void mainboard_romstage_entry(unsigned long bist);
void romstage_common(const struct romstage_params *params);
/* romstage_main is called from the cache-as-ram assembly file. The return
 * value is the stack value to be used for romstage once cache-as-ram is
 * torn down. The following values are pushed onto the stack to setup the
 * MTRRs:
 *   +0: Number of MTRRs
 *   +4: MTTR base 0 31:0
 *   +8: MTTR base 0 63:32
 *  +12: MTTR mask 0 31:0
 *  +16: MTTR mask 0 63:32
 *  +20: MTTR base 1 31:0
 *  +24: MTTR base 1 63:32
 *  +28: MTTR mask 1 31:0
 *  +32: MTTR mask 1 63:32
 *  ...
 */
void * asmlinkage romstage_main(unsigned long bist);
/* romstage_after_car() is the C function called after cache-as-ram has
 * been torn down. It is responsible for loading the ramstage. */
void romstage_after_car(void);
#endif

#ifdef __SMM__
/* Lock MSRs */
void intel_cpu_haswell_finalize_smm(void);
#else
/* Configure power limits for turbo mode */
void set_power_limits(u8 power_limit_1_time);
int cpu_config_tdp_levels(void);
/* Returns 0 on success, < 0 on failure. */
int smm_initialize(void);
void smm_initiate_relocation(void);
void smm_initiate_relocation_parallel(void);
struct bus;
void bsp_init_and_start_aps(struct bus *cpu_bus);
/* Returns 0 on success. < 0 on failure. */
int setup_ap_init(struct bus *cpu_bus, int *max_cpus,
                  const void *microcode_patch);
/* Returns 0 on success, < 0 on failure. */
int start_aps(struct bus *cpu_bus, int max_cpus);
void release_aps_for_smm_relocation(int do_parallel_relocation);
/* Determine if HyperThreading is disabled. The variable is not valid until
 * setup_ap_init() has been called. */
extern int ht_disabled;
#endif

/* This structure is saved along with the relocated ramstage program in SMM
 * space. It is used to protect the integrity of the ramstage program on S3
 * resume by saving a copy of the relocated ramstage in SMM space with the
 * assumption that the SMM region cannot be altered from the OS. The magic
 * value just serves as a quick sanity check. */

#define RAMSTAGE_CACHE_MAGIC 0xf3c3a02a

struct ramstage_cache {
	uint32_t magic;
	uint32_t entry_point;
	uint32_t load_address;
	uint32_t size;
	char program[0];
} __attribute__((packed));

#endif

#endif
