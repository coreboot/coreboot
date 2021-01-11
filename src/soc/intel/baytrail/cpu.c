/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/intel/common/common.h>
#include <cpu/intel/em64t100_save_state.h>
#include <cpu/intel/microcode.h>
#include <cpu/intel/smm_reloc.h>
#include <cpu/intel/turbo.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/mp.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <reg_script.h>
#include <soc/iosf.h>
#include <soc/msr.h>
#include <soc/pattrs.h>
#include <soc/ramstage.h>

/* Core level MSRs */
static const struct reg_script core_msr_script[] = {
	/* Dynamic L2 shrink enable and threshold, clear SINGLE_PCTL bit 11 */
	REG_MSR_RMW(MSR_PKG_CST_CONFIG_CONTROL, ~0x3f080f, 0xe0008),
	REG_MSR_RMW(MSR_POWER_MISC, ~(ENABLE_ULFM_AUTOCM_MASK | ENABLE_INDP_AUTOCM_MASK), 0),

	/* Disable C1E */
	REG_MSR_RMW(MSR_POWER_CTL, ~0x2, 0),
	REG_MSR_OR(MSR_POWER_MISC, 0x44),
	REG_SCRIPT_END
};

static void soc_core_init(struct device *cpu)
{
	printk(BIOS_DEBUG, "Init BayTrail core.\n");

	/*
	 * The turbo disable bit is actually scoped at building block level -- not package.
	 * For non-BSP cores that are within a building block, enable turbo. The cores within
	 * the BSP's building block will just see it already enabled and move on.
	 */
	if (lapicid())
		enable_turbo();

	/* Set virtualization based on Kconfig option */
	set_vmx_and_lock();

	/* Set core MSRs */
	reg_script_run(core_msr_script);

	/* Set this core to max frequency ratio */
	set_max_freq();
}

static struct device_operations cpu_dev_ops = {
	.init = soc_core_init,
};

static const struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x30673 },
	{ X86_VENDOR_INTEL, 0x30678 },
	{ X86_VENDOR_INTEL, 0x30679 },
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};

/*
 * MP and SMM loading initialization.
 */

/* Package level MSRs */
static const struct reg_script package_msr_script[] = {
	/* Set Package TDP to ~7W */
	REG_MSR_WRITE(MSR_PKG_POWER_LIMIT, 0x3880fa),
	REG_MSR_RMW(MSR_PP1_POWER_LIMIT, ~(0x7f << 17), 0),
	REG_MSR_WRITE(MSR_PKG_TURBO_CFG1, 0x702),
	REG_MSR_WRITE(MSR_CPU_TURBO_WKLD_CFG1, 0x200b),
	REG_MSR_WRITE(MSR_CPU_TURBO_WKLD_CFG2, 0),
	REG_MSR_WRITE(MSR_CPU_THERM_CFG1, 0x00000305),
	REG_MSR_WRITE(MSR_CPU_THERM_CFG2, 0x0405500d),
	REG_MSR_WRITE(MSR_CPU_THERM_SENS_CFG, 0x27),
	REG_SCRIPT_END
};

static void pre_mp_init(void)
{
	uint32_t bsmrwac;

	/* Set up MTRRs based on physical address size. */
	x86_setup_mtrrs_with_detect();
	x86_mtrr_check();

	/*
	 * Configure the BUNIT to allow dirty cache line evictions in non-SMM mode for lines
	 * that were dirtied while in SMM mode. Otherwise the writes would be silently dropped.
	 */
	bsmrwac = iosf_bunit_read(BUNIT_SMRWAC) | SAI_IA_UNTRUSTED;
	iosf_bunit_write(BUNIT_SMRWAC, bsmrwac);

	/* Set package MSRs */
	reg_script_run(package_msr_script);

	/* Enable Turbo Mode on BSP and siblings of the BSP's building block. */
	enable_turbo();
}

static int get_cpu_count(void)
{
	const struct pattrs *pattrs = pattrs_get();

	return pattrs->num_cpus;
}

static void fill_in_relocation_params(struct smm_relocation_params *params)
{
	uintptr_t tseg_base;
	size_t tseg_size;

	/* All range registers are aligned to 4KiB */
	const u32 rmask = ~((1 << 12) - 1);

	smm_region(&tseg_base, &tseg_size);

	/* SMRR has 32-bits of valid address aligned to 4KiB. */
	params->smrr_base.lo = (tseg_base & rmask) | MTRR_TYPE_WRBACK;
	params->smrr_base.hi = 0;
	params->smrr_mask.lo = (~(tseg_size - 1) & rmask) | MTRR_PHYS_MASK_VALID;
	params->smrr_mask.hi = 0;
}

static void get_smm_info(uintptr_t *perm_smbase, size_t *perm_smsize,
				size_t *smm_save_state_size)
{
	printk(BIOS_DEBUG, "Setting up SMI for CPU\n");

	fill_in_relocation_params(&smm_reloc_params);

	smm_subregion(SMM_SUBREGION_HANDLER, perm_smbase, perm_smsize);

	*smm_save_state_size = sizeof(em64t100_smm_state_save_area_t);
}

static void get_microcode_info(const void **microcode, int *parallel)
{
	const struct pattrs *pattrs = pattrs_get();

	*microcode = pattrs->microcode_patch;
	*parallel = !intel_ht_supported();
}

static void per_cpu_smm_trigger(void)
{
	const struct pattrs *pattrs = pattrs_get();

	/* Relocate SMM space. */
	smm_initiate_relocation();

	/* Load microcode after SMM relocation. */
	intel_microcode_load_unlocked(pattrs->microcode_patch);
}

static void relocation_handler(int cpu, uintptr_t curr_smbase, uintptr_t staggered_smbase)
{
	struct smm_relocation_params *relo_params = &smm_reloc_params;
	em64t100_smm_state_save_area_t *smm_state;

	/* Set up SMRR. */
	wrmsr(IA32_SMRR_PHYS_BASE, relo_params->smrr_base);
	wrmsr(IA32_SMRR_PHYS_MASK, relo_params->smrr_mask);

	smm_state = (void *)(SMM_EM64T100_SAVE_STATE_OFFSET + curr_smbase);
	smm_state->smbase = staggered_smbase;
}

static void post_mp_init(void)
{
	global_smi_enable();
}

static const struct mp_ops mp_ops = {
	.pre_mp_init         = pre_mp_init,
	.get_cpu_count       = get_cpu_count,
	.get_smm_info        = get_smm_info,
	.get_microcode_info  = get_microcode_info,
	.pre_mp_smm_init     = smm_southbridge_clear_state,
	.per_cpu_smm_trigger = per_cpu_smm_trigger,
	.relocation_handler  = relocation_handler,
	.post_mp_init        = post_mp_init,
};

void baytrail_init_cpus(struct device *dev)
{
	struct bus *cpu_bus = dev->link_list;

	if (mp_init_with_smm(cpu_bus, &mp_ops))
		printk(BIOS_ERR, "MP initialization failure.\n");
}
