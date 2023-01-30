/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <assert.h>
#include <console/console.h>
#include "chip.h"
#include <cpu/cpu.h>
#include <cpu/x86/mp.h>
#include <cpu/intel/microcode.h>
#include <cpu/intel/turbo.h>
#include <cpu/intel/common/common.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <cpu/intel/em64t100_save_state.h>
#include <cpu/intel/smm_reloc.h>
#include <device/device.h>
#include <device/pci.h>
#include <fsp/api.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/fast_spi.h>
#include <intelblocks/mp_init.h>
#include <intelblocks/msr.h>
#include <intelblocks/sgx.h>
#include <reg_script.h>
#include <soc/cpu.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <types.h>

static const struct reg_script core_msr_script[] = {
#if !CONFIG(SOC_INTEL_GEMINILAKE)
	/* Enable C-state and IO/MWAIT redirect */
	REG_MSR_WRITE(MSR_PKG_CST_CONFIG_CONTROL,
		(PKG_C_STATE_LIMIT_C2_MASK | CORE_C_STATE_LIMIT_C10_MASK
		| IO_MWAIT_REDIRECT_MASK | CST_CFG_LOCK_MASK)),
	/* Power Management I/O base address for I/O trapping to C-states */
	REG_MSR_WRITE(MSR_PMG_IO_CAPTURE_BASE,
		(ACPI_PMIO_CST_REG | (PMG_IO_BASE_CST_RNG_BLK_SIZE << 16))),
	/* Disable support for MONITOR and MWAIT instructions */
	REG_MSR_RMW(IA32_MISC_ENABLE, ~MONITOR_MWAIT_DIS_MASK, 0),
#endif
	/* Disable C1E */
	REG_MSR_RMW(MSR_POWER_CTL, ~POWER_CTL_C1E_MASK, 0),
	REG_SCRIPT_END
};

bool cpu_soc_is_in_untrusted_mode(void)
{
	msr_t msr;

	msr = rdmsr(MSR_POWER_MISC);
	return !!(msr.lo & ENABLE_IA_UNTRUSTED);
}

void cpu_soc_bios_done(void)
{
	msr_t msr;

	msr = rdmsr(MSR_POWER_MISC);
	msr.lo |= ENABLE_IA_UNTRUSTED;
	wrmsr(MSR_POWER_MISC, msr);
}

void soc_core_init(struct device *cpu)
{
	/* Configure Core PRMRR for SGX. */
	if (CONFIG(SOC_INTEL_COMMON_BLOCK_SGX_ENABLE))
		prmrr_core_configure();

	/* Clear out pending MCEs */
	/* TODO(adurbin): Some of these banks are core vs package
			  scope. For now every CPU clears every bank. */
	if (CONFIG(SOC_INTEL_COMMON_BLOCK_SGX_ENABLE) || acpi_get_sleep_type() == ACPI_S5)
		mca_configure();

	/* Set core MSRs */
	reg_script_run(core_msr_script);

	set_aesni_lock();

	/*
	 * Enable ACPI PM timer emulation, which also lets microcode know
	 * location of ACPI_BASE_ADDRESS. This also enables other features
	 * implemented in microcode.
	*/
	enable_pm_timer_emulation();

	/* Set Max Non-Turbo ratio if RAPL is disabled. */
	if (CONFIG(SOC_INTEL_DISABLE_POWER_LIMITS)) {
		cpu_set_p_state_to_max_non_turbo_ratio();
		/* Disable speed step */
		cpu_set_eist(false);
	} else if (CONFIG(SOC_INTEL_SET_MIN_CLOCK_RATIO)) {
		cpu_set_p_state_to_min_clock_ratio();
		/* Disable speed step */
		cpu_set_eist(false);
	}
}

#if !CONFIG(SOC_INTEL_COMMON_BLOCK_CPU_MPINIT)
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
	{ X86_VENDOR_INTEL, CPUID_GLK_R0 },
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
 * IF (CONFIG(SOC_INTEL_COMMON_BLOCK_CPU_MPINIT)) -
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
	if (CONFIG(SOC_INTEL_COMMON_BLOCK_CPU_MPINIT)) {
		fsps_load();
		return;
	}
	x86_setup_mtrrs_with_detect();
	x86_mtrr_check();

}

#if !CONFIG(SOC_INTEL_COMMON_BLOCK_CPU_MPINIT)
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
	uintptr_t smm_base;
	size_t smm_size;
	uintptr_t handler_base;
	size_t handler_size;

	/* All range registers are aligned to 4KiB */
	const uint32_t rmask = ~((1 << 12) - 1);

	/* Initialize global tracking state. */
	smm_region(&smm_base, &smm_size);
	smm_subregion(SMM_SUBREGION_HANDLER, &handler_base, &handler_size);

	relo_attrs.smbase = smm_base;
	relo_attrs.smrr_base = relo_attrs.smbase | MTRR_TYPE_WRBACK;
	relo_attrs.smrr_mask = ~(smm_size - 1) & rmask;
	relo_attrs.smrr_mask |= MTRR_PHYS_MASK_VALID;

	*perm_smbase = handler_base;
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
	wrmsr(IA32_SMRR_PHYS_BASE, smrr);
	smrr.lo = relo_attrs.smrr_mask;
	smrr.hi = 0;
	wrmsr(IA32_SMRR_PHYS_MASK, smrr);
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
	global_smi_enable();

	if (CONFIG(SOC_INTEL_COMMON_BLOCK_SGX_ENABLE))
		mp_run_on_all_cpus(sgx_configure, NULL);
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
	/* TODO: Handle mp_init_with_smm failure? */
	mp_init_with_smm(cpu_bus, &mp_ops);
}

void apollolake_init_cpus(struct device *dev)
{
	if (CONFIG(SOC_INTEL_COMMON_BLOCK_CPU_MPINIT))
		return;
	if (!dev->link_list)
		add_more_links(dev, 1);
	soc_init_cpus(dev->link_list);

	/* Temporarily cache the memory-mapped boot media. */
	if (CONFIG(BOOT_DEVICE_MEMORY_MAPPED) &&
		CONFIG(BOOT_DEVICE_SPI_FLASH))
		fast_spi_cache_bios_region();
}
