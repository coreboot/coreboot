/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/cpu.h>
#include <cpu/cpu.h>
#include <cpu/x86/mp.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/smm.h>
#include <cpu/amd/msr.h>
#include <cpu/amd/amd64_save_state.h>
#include <cpu/x86/lapic.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include <soc/pci_devs.h>
#include <soc/cpu.h>
#include <soc/reset.h>
#include <soc/smi.h>
#include <soc/iomap.h>
#include <console/console.h>
#include <cpu/amd/microcode.h>

/*
 * MP and SMM loading initialization.
 */
struct smm_relocation_params {
	msr_t tseg_base;
	msr_t tseg_mask;
};

static struct smm_relocation_params smm_reloc_params;

/*
 * Do essential initialization tasks before APs can be fired up -
 *
 *  1. Prevent race condition in MTRR solution. Enable MTRRs on the BSP. This
 *  creates the MTRR solution that the APs will use. Otherwise APs will try to
 *  apply the incomplete solution as the BSP is calculating it.
 */
static void pre_mp_init(void)
{
	x86_setup_mtrrs_with_detect_no_above_4gb();
	x86_mtrr_check();
}

static void fill_in_relocation_params(struct smm_relocation_params *params)
{
	uintptr_t tseg_base;
	size_t tseg_size;

	smm_region(&tseg_base, &tseg_size);

	params->tseg_base.lo = ALIGN_DOWN(tseg_base, 128 * KiB);
	params->tseg_base.hi = 0;
	params->tseg_mask.lo = ALIGN_DOWN(~(tseg_size - 1), 128 * KiB);
	params->tseg_mask.hi = ((1 << (cpu_phys_address_size() - 32)) - 1);

	params->tseg_mask.lo |= SMM_TSEG_WB;
}

static void get_smm_info(uintptr_t *perm_smbase, size_t *perm_smsize,
				size_t *smm_save_state_size)
{
	printk(BIOS_DEBUG, "Setting up SMI for CPU\n");

	fill_in_relocation_params(&smm_reloc_params);

	smm_subregion(SMM_SUBREGION_HANDLER, perm_smbase, perm_smsize);
	*smm_save_state_size = sizeof(amd64_smm_state_save_area_t);
}

static void relocation_handler(int cpu, uintptr_t curr_smbase,
				uintptr_t staggered_smbase)
{
	struct smm_relocation_params *relo_params = &smm_reloc_params;
	amd64_smm_state_save_area_t *smm_state;

	wrmsr(SMM_ADDR_MSR, relo_params->tseg_base);
	wrmsr(SMM_MASK_MSR, relo_params->tseg_mask);

	smm_state = (void *)(SMM_AMD64_SAVE_STATE_OFFSET + curr_smbase);
	smm_state->smbase = staggered_smbase;
}

static const struct mp_ops mp_ops = {
	.pre_mp_init = pre_mp_init,
	.get_cpu_count = get_cpu_count,
	.get_smm_info = get_smm_info,
	.relocation_handler = relocation_handler,
	.post_mp_init = global_smi_enable,
};

void mp_init_cpus(struct bus *cpu_bus)
{
	/* Clear for take-off */
	if (mp_init_with_smm(cpu_bus, &mp_ops) < 0)
		printk(BIOS_ERR, "MP initialization failure.\n");

	/* pre_mp_init made the flash not cacheable. Reset to WP for performance. */
	mtrr_use_temp_range(FLASH_BASE_ADDR, CONFIG_ROM_SIZE, MTRR_TYPE_WRPROT);

	set_warm_reset_flag();
}

static void model_17_init(struct device *dev)
{
	check_mca();
	setup_lapic();
	set_cstate_io_addr();

	amd_update_microcode_from_cbfs();
}

static struct device_operations cpu_dev_ops = {
	.init = model_17_init,
};

static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_AMD, RAVEN1_B0_CPUID},
	{ X86_VENDOR_AMD, PICASSO_B0_CPUID },
	{ X86_VENDOR_AMD, PICASSO_B1_CPUID },
	{ X86_VENDOR_AMD, RAVEN2_A0_CPUID },
	{ X86_VENDOR_AMD, RAVEN2_A1_CPUID },
	{ 0, 0 },
};

static const struct cpu_driver model_17 __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
