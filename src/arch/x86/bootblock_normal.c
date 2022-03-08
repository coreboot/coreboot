/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <fallback.h>
#include <program_loading.h>
#include <stddef.h>

static const char *get_fallback(const char *stagelist)
{
	while (*stagelist)
		stagelist++;
	return ++stagelist;
}

enum cb_err legacy_romstage_select_and_load(struct prog *romstage)
{
	static const char *default_filenames = "normal/romstage\0fallback/romstage";
	const char *boot_candidate;
	size_t stages_len;

	boot_candidate = cbfs_map("coreboot-stages", &stages_len);
	if (!boot_candidate)
		boot_candidate = default_filenames;

	if (do_normal_boot()) {
		romstage->name = boot_candidate;
		if (cbfs_prog_stage_load(romstage) == CB_SUCCESS)
			return CB_SUCCESS;
	}

	romstage->name = get_fallback(boot_candidate);
	return cbfs_prog_stage_load(romstage);
}
