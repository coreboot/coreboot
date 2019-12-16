/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <cbfs.h>
#include <pc80/mc146818rtc.h>
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
