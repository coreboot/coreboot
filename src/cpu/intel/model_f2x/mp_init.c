/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/intel/microcode.h>
#include <cpu/intel/smm_reloc.h>
#include <cpu/intel/common/common.h>
#include <cpu/x86/legacy_save_state.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/mp.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include <types.h>

/* Parallel MP initialization support. */
static void pre_mp_init(void)
{
	const void *patch = intel_microcode_find();
	intel_microcode_load_unlocked(patch);

	/* Setup MTRRs based on physical address size. */
	x86_setup_mtrrs_with_detect();
	x86_mtrr_check();
}

static int get_cpu_count(void)
{
	return CONFIG_MAX_CPUS;
}

static void get_microcode_info(const void **microcode, int *parallel)
{
	*microcode = intel_microcode_find();
	*parallel = !intel_ht_supported();
}

static void pre_mp_smm_init(void)
{
	/* Clear the SMM state in the southbridge. */
	smm_southbridge_clear_state();

	/*
	 * Run the relocation handler for on the BSP to check and set up
	 * parallel SMM relocation.
	 */
	smm_initiate_relocation();
}

static void get_smm_info(uintptr_t *perm_smbase, size_t *perm_smsize,
	      size_t *smm_save_state_size)
{
	printk(BIOS_DEBUG, "Setting up SMI for CPU\n");

	smm_open();

	smm_subregion(SMM_SUBREGION_HANDLER, perm_smbase, perm_smsize);

	*smm_save_state_size = sizeof(legacy_smm_state_save_area_t);
	printk(BIOS_DEBUG, "Save state size: 0x%zx bytes\n", *smm_save_state_size);
}

/*
 * The relocation work is actually performed in SMM context, but the code
 * resides in the ramstage module. This occurs by trampolining from the default
 * SMRAM entry point to here.
 */
static void relocation_handler(int cpu, uintptr_t curr_smbase, uintptr_t staggered_smbase)
{
	legacy_smm_state_save_area_t *save_state;
	u32 smbase = staggered_smbase;

	save_state = (void *)(curr_smbase + SMM_DEFAULT_SIZE - sizeof(*save_state));
	save_state->smbase = smbase;

	printk(BIOS_DEBUG, "In relocation handler: cpu %d\n", cpu);
	printk(BIOS_DEBUG, "SMM revision: 0x%08x\n", save_state->smm_revision);
	printk(BIOS_DEBUG, "New SMBASE=0x%08x\n", smbase);
}

static void post_mp_init(void)
{
	smm_close();

	/* Now that all APs have been relocated as well as the BSP let SMIs start flowing. */
	global_smi_enable();

	/* Lock down the SMRAM space. */
	smm_lock();
}

static const struct mp_ops mp_ops = {
	.pre_mp_init = pre_mp_init,
	.get_cpu_count = get_cpu_count,
	.get_smm_info = get_smm_info,
	.get_microcode_info = get_microcode_info,
	.pre_mp_smm_init = pre_mp_smm_init,
	/* .per_cpu_smm_trigger = smm_initiate_relocation, using default */
	.relocation_handler = relocation_handler,
	.post_mp_init = post_mp_init,
};

void mp_init_cpus(struct bus *cpu_bus)
{
	/* TODO: Handle mp_init_with_smm failure? */
	mp_init_with_smm(cpu_bus, &mp_ops);
}
