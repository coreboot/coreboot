/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/amd/amd64_save_state.h>
#include <cpu/intel/smm_reloc.h>
#include <cpu/x86/legacy_save_state.h>
#include <cpu/x86/mp.h>
#include <cpu/x86/smm.h>
#include <mainboard/emulation/qemu-i440fx/fw_cfg.h>
#include <stddef.h>
#include <stdint.h>

static void get_smm_info(uintptr_t *perm_smbase, size_t *perm_smsize,
	      size_t *smm_save_state_size)
{
	printk(BIOS_DEBUG, "Setting up SMI for CPU\n");

	if (CONFIG(SMM_TSEG))
		smm_subregion(SMM_SUBREGION_HANDLER, perm_smbase, perm_smsize);

	if (CONFIG(SMM_ASEG)) {
		smm_open_aseg();
		*perm_smbase = 0xa0000;
		*perm_smsize = 0x10000;
	}

	/* FIXME: on X86_64 the save state size is smaller than the size of the SMM stub */
	*smm_save_state_size = sizeof(amd64_smm_state_save_area_t);
	printk(BIOS_DEBUG, "Save state size: 0x%zx bytes\n", *smm_save_state_size);
}

/*
 * The relocation work is actually performed in SMM context, but the code
 * resides in the ramstage module. This occurs by trampolining from the default
 * SMRAM entry point to here.
 */

union __packed save_state {
	amd64_smm_state_save_area_t amd64;
	struct {
		char _reserved[sizeof(amd64_smm_state_save_area_t)
			       - sizeof(legacy_smm_state_save_area_t)];
		legacy_smm_state_save_area_t legacy;
	};
};

_Static_assert(sizeof(union save_state) == sizeof(amd64_smm_state_save_area_t),
	       "Incorrect save state union size");

_Static_assert(offsetof(union save_state, amd64.smm_revision)
	       == offsetof(union save_state, legacy.smm_revision),
	       "Incompatible SMM save state revision offset");

static void relocation_handler(int cpu, uintptr_t curr_smbase,
			       uintptr_t staggered_smbase)
{
	union save_state *save_state =
		(void *)(curr_smbase + SMM_DEFAULT_SIZE - sizeof(*save_state));

	u32 smbase = staggered_smbase;

	/* The SMM save state revision is always at a compatible offset */
	const u32 revision = save_state->legacy.smm_revision;
	switch (revision) {
	case 0x00020000:
		save_state->legacy.smbase = smbase;
		break;
	case 0x00020064:
		save_state->amd64.smbase = smbase;
		break;
	default:
		printk(BIOS_ERR, "Unknown SMM revision 0x%x, not relocating SMM\n", revision);
		return;
	};

	printk(BIOS_DEBUG, "In relocation handler: cpu %d\n", cpu);
	printk(BIOS_DEBUG, "SMM revision: 0x%08x\n", revision);
	printk(BIOS_DEBUG, "New SMBASE=0x%08x\n", smbase);
}

static void post_mp_init(void)
{
	/* Now that all APs have been relocated as well as the BSP let SMIs start flowing. */
	global_smi_enable();

	/* Lock down the SMRAM space. */
	smm_lock();
}

const struct mp_ops mp_ops_with_smm = {
	.get_cpu_count       = fw_cfg_max_cpus,
	.get_smm_info        = get_smm_info,
	.pre_mp_smm_init     = smm_southbridge_clear_state,
	.relocation_handler  = relocation_handler,
	.post_mp_init        = post_mp_init,
};
