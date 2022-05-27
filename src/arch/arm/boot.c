/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <arch/cache.h>
#include <program_loading.h>

void boot_linux(void *kernel_ptr, void *fdt_ptr);

void arch_prog_run(struct prog *prog)
{
	void (*doit)(void *);

	cache_sync_instructions();

	switch (prog_cbfs_type(prog)) {
	case CBFS_TYPE_FIT_PAYLOAD:
		/*
		 * We only load Linux payloads from the ramstage, so provide a hint to
		 * the linker that the below functions do not need to be included in
		 * earlier stages.
		 */
		if (!ENV_RAMSTAGE)
			break;

		dcache_mmu_disable();
		boot_linux(prog_entry(prog), prog_entry_arg(prog));
		break;
	default:
		doit = prog_entry(prog);
		doit(prog_entry_arg(prog));
	}
}
