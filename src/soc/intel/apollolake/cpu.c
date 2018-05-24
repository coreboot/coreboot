/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2017 Intel Corp.
 * Copyright (C) 2017 Siemens AG, Inc.
 * (Written by Andrey Petrov <andrey.petrov@intel.com> for Intel Corp.)
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <assert.h>
#include <console/console.h>
#include "chip.h"
#include <cpu/cpu.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/mp.h>
#include <cpu/intel/microcode.h>
#include <cpu/intel/turbo.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <device/device.h>
#include <device/pci.h>
#include <fsp/api.h>
#include <fsp/memmap.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/fast_spi.h>
#include <intelblocks/mp_init.h>
#include <intelblocks/msr.h>
#include <intelblocks/sgx.h>
#include <intelblocks/smm.h>
#include <reg_script.h>
#include <romstage_handoff.h>
#include <soc/cpu.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>

static const struct reg_script core_msr_script[] = {
#if !IS_ENABLED(CONFIG_SOC_INTEL_GLK)
	/* Enable C-state and IO/MWAIT redirect */
	REG_MSR_WRITE(MSR_PMG_CST_CONFIG_CONTROL,
		(PKG_C_STATE_LIMIT_C2_MASK | CORE_C_STATE_LIMIT_C10_MASK
		| IO_MWAIT_REDIRECT_MASK | CST_CFG_LOCK_MASK)),
	/* Power Management I/O base address for I/O trapping to C-states */
	REG_MSR_WRITE(MSR_PMG_IO_CAPTURE_BASE,
		(ACPI_PMIO_CST_REG | (PMG_IO_BASE_CST_RNG_BLK_SIZE << 16))),
	/* Disable C1E */
	REG_MSR_RMW(MSR_POWER_CTL, ~0x2, 0),
	/* Disable support for MONITOR and MWAIT instructions */
	REG_MSR_RMW(MSR_IA32_MISC_ENABLES, ~MONITOR_MWAIT_DIS_MASK, 0),
#endif
	/*
	 * Enable and Lock the Advanced Encryption Standard (AES-NI)
	 * feature register
	 */
	REG_MSR_RMW(MSR_FEATURE_CONFIG, ~FEATURE_CONFIG_RESERVED_MASK,
		FEATURE_CONFIG_LOCK),
	REG_SCRIPT_END
};

void soc_core_init(struct device *cpu)
{
	/* Clear out pending MCEs */
	/* TODO(adurbin): This should only be done on a cold boot. Also, some
	 * of these banks are core vs package scope. For now every CPU clears
	 * every bank. */
	mca_configure();

	/* Set core MSRs */
	reg_script_run(core_msr_script);
	/*
	 * Enable ACPI PM timer emulation, which also lets microcode know
	 * location of ACPI_BASE_ADDRESS. This also enables other features
	 * implemented in microcode.
	*/
	enable_pm_timer_emulation();

	/* Configure Core PRMRR for SGX. */
	if (IS_ENABLED(CONFIG_SOC_INTEL_COMMON_BLOCK_SGX))
		prmrr_core_configure();

	/* Set Max Non-Turbo ratio if RAPL is disabled. */
	if (IS_ENABLED(CONFIG_APL_SKIP_SET_POWER_LIMITS)) {
		cpu_set_p_state_to_max_non_turbo_ratio();
		cpu_disable_eist();
	}
}

#if !IS_ENABLED(CONFIG_SOC_INTEL_COMMON_BLOCK_CPU_MPINIT)
static void soc_init_core(struct device *cpu)
{
	soc_core_init(cpu);
}

static struct device_operations cpu_dev_ops = {
	.init = soc_init_core,
};

static const struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, CPUID_APOLLOLAKE_A0 },
	{ X86_VENDOR_INTEL, CPUID_APOLLOLAKE_B0 },
	{ X86_VENDOR_INTEL, CPUID_APOLLOLAKE_E0 },
	{ X86_VENDOR_INTEL, CPUID_GLK_A0 },
	{ X86_VENDOR_INTEL, CPUID_GLK_B0 },
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
#endif

/*
 * MP and SMM loading initialization.
 */
struct smm_relocation_attrs {
	uint32_t smbase;
	uint32_t smrr_base;
	uint32_t smrr_mask;
};

static struct smm_relocation_attrs relo_attrs;

/*
 * Do essential initialization tasks before APs can be fired up.
 *
 * IF (IS_ENABLED(CONFIG_SOC_INTEL_COMMON_BLOCK_CPU_MPINIT)) -
 * Skip Pre MP init MTRR programming, as MTRRs are mirrored from BSP,
 * that are set prior to ramstage.
 * Real MTRRs are programmed after resource allocation.
 *
 * Do FSP loading before MP Init to ensure that the FSP component stored in
 * external stage cache in TSEG does not flush off due to SMM relocation
 * during MP Init stage.
 *
 * ELSE -
 * Enable MTRRs on the BSP. This creates the MTRR solution that the
 * APs will use. Otherwise APs will try to apply the incomplete solution
 * as the BSP is calculating it.
 */
static void pre_mp_init(void)
{
	if (IS_ENABLED(CONFIG_SOC_INTEL_COMMON_BLOCK_CPU_MPINIT)) {
		fsps_load(romstage_handoff_is_resume());
		return;
	}
	x86_setup_mtrrs_with_detect();
	x86_mtrr_check();
}

#if !IS_ENABLED(CONFIG_SOC_INTEL_COMMON_BLOCK_CPU_MPINIT)
static void read_cpu_topology(unsigned int *num_phys, unsigned int *num_virt)
{
	msr_t msr;
	msr = rdmsr(MSR_CORE_THREAD_COUNT);
	*num_virt = (msr.lo >> 0) & 0xffff;
	*num_phys = (msr.lo >> 16) & 0xffff;
}

/* Find CPU topology */
int get_cpu_count(void)
{
	unsigned int num_virt_cores, num_phys_cores;

	read_cpu_topology(&num_phys_cores, &num_virt_cores);

	printk(BIOS_DEBUG, "Detected %u core, %u thread CPU.\n",
	       num_phys_cores, num_virt_cores);

	return num_virt_cores;
}

void get_microcode_info(const void **microcode, int *parallel)
{
	*microcode = intel_microcode_find();
	*parallel = 1;

	/* Make sure BSP is using the microcode from cbfs */
	intel_microcode_load_unlocked(*microcode);
}
#endif

static void get_smm_info(uintptr_t *perm_smbase, size_t *perm_smsize,
				size_t *smm_save_state_size)
{
	void *smm_base;
	size_t smm_size;
	void *handler_base;
	size_t handler_size;

	/* All range registers are aligned to 4KiB */
	const uint32_t rmask = ~((1 << 12) - 1);

	/* Initialize global tracking state. */
	smm_region_info(&smm_base, &smm_size);
	smm_subregion(SMM_SUBREGION_HANDLER, &handler_base, &handler_size);

	relo_attrs.smbase = (uint32_t)smm_base;
	relo_attrs.smrr_base = relo_attrs.smbase | MTRR_TYPE_WRBACK;
	relo_attrs.smrr_mask = ~(smm_size - 1) & rmask;
	relo_attrs.smrr_mask |= MTRR_PHYS_MASK_VALID;

	*perm_smbase = (uintptr_t)handler_base;
	*perm_smsize = handler_size;
	*smm_save_state_size = sizeof(em64t100_smm_state_save_area_t);
}

static void relocation_handler(int cpu, uintptr_t curr_smbase,
				uintptr_t staggered_smbase)
{
	msr_t smrr;
	em64t100_smm_state_save_area_t *smm_state;
	/* Set up SMRR. */
	smrr.lo = relo_attrs.smrr_base;
	smrr.hi = 0;
	wrmsr(SMRR_PHYS_BASE, smrr);
	smrr.lo = relo_attrs.smrr_mask;
	smrr.hi = 0;
	wrmsr(SMRR_PHYS_MASK, smrr);
	smm_state = (void *)(SMM_EM64T100_SAVE_STATE_OFFSET + curr_smbase);
	smm_state->smbase = staggered_smbase;
}
/*
 * CPU initialization recipe
 *
 * Note that no microcode update is passed to the init function. CSE updates
 * the microcode on all cores before releasing them from reset. That means that
 * the BSP and all APs will come up with the same microcode revision.
 */

static void post_mp_init(void)
{
	smm_southbridge_enable(PWRBTN_EN | GBL_EN);

	if (IS_ENABLED(CONFIG_SOC_INTEL_COMMON_BLOCK_SGX))
		mp_run_on_all_cpus(sgx_configure, NULL, 2000);
}

static const struct mp_ops mp_ops = {
	.pre_mp_init = pre_mp_init,
	.get_cpu_count = get_cpu_count,
	.get_smm_info = get_smm_info,
	.get_microcode_info = get_microcode_info,
	.pre_mp_smm_init = smm_southbridge_clear_state,
	.relocation_handler = relocation_handler,
	.post_mp_init = post_mp_init,
};

void soc_init_cpus(struct bus *cpu_bus)
{
	/* Clear for take-off */
	if (mp_init_with_smm(cpu_bus, &mp_ops))
		printk(BIOS_ERR, "MP initialization failure.\n");
}

void apollolake_init_cpus(struct device *dev)
{
	if (IS_ENABLED(CONFIG_SOC_INTEL_COMMON_BLOCK_CPU_MPINIT))
		return;
	soc_init_cpus(dev->link_list);

	/* Temporarily cache the memory-mapped boot media. */
	if (IS_ENABLED(CONFIG_BOOT_DEVICE_MEMORY_MAPPED) &&
		IS_ENABLED(CONFIG_BOOT_DEVICE_SPI_FLASH))
		fast_spi_cache_bios_region();
}

void cpu_lock_sgx_memory(void)
{
	/* Do nothing because MCHECK while loading microcode and enabling
	 * IA untrusted mode takes care of necessary locking */
}

int soc_fill_sgx_param(struct sgx_param *sgx_param)
{
	struct device *dev = SA_DEV_ROOT;
	assert(dev != NULL);
	config_t *conf = dev->chip_info;

	if (!conf) {
		printk(BIOS_ERR, "Failed to get chip_info for SGX param\n");
		return -1;
	}

	sgx_param->enable = conf->sgx_enable;
	return 0;
}
