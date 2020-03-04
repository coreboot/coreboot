/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <cbfs.h>
#include <fallback.h>
#include <program_loading.h>
#include <stddef.h>
#include <string.h>

static const char *get_fallback(const char *stagelist)
{
	while (*stagelist)
		stagelist++;
	return ++stagelist;
}

int legacy_romstage_selector(struct prog *romstage)
{
	static const char *default_filenames = "normal/romstage\0fallback/romstage";
	const char *boot_candidate;
	size_t stages_len;

	boot_candidate = cbfs_boot_map_with_leak("coreboot-stages", CBFS_TYPE_RAW, &stages_len);
	if (!boot_candidate)
		boot_candidate = default_filenames;

	if (do_normal_boot()) {
		romstage->name = boot_candidate;
		if (!prog_locate(romstage))
			return 0;
	}

	romstage->name = get_fallback(boot_candidate);
	return prog_locate(romstage);
}
