/*
 * This file is part of the coreboot project.
 *
 * Copyright 2010 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/exception.h>
#include <bootblock_common.h>
#include <compiler.h>
#include <console/console.h>
#include <delay.h>
#include <pc80/mc146818rtc.h>
#include <program_loading.h>
#include <symbols.h>

DECLARE_OPTIONAL_REGION(timestamp);

__weak void bootblock_mainboard_early_init(void) { /* no-op */ }
__weak void bootblock_soc_early_init(void) { /* do nothing */ }
__weak void bootblock_soc_init(void) { /* do nothing */ }
__weak void bootblock_mainboard_init(void) { /* do nothing */ }

asmlinkage void bootblock_main_with_timestamp(uint64_t base_timestamp,
	struct timestamp_entry *timestamps, size_t num_timestamps)
{
	/* Initialize timestamps if we have TIMESTAMP region in memlayout.ld. */
	if (IS_ENABLED(CONFIG_COLLECT_TIMESTAMPS) && _timestamp_size > 0) {
		int i;
		timestamp_init(base_timestamp);
		for (i = 0; i < num_timestamps; i++)
			timestamp_add(timestamps[i].entry_id,
				      timestamps[i].entry_stamp);
	}

	sanitize_cmos();
	cmos_post_init();

	bootblock_soc_early_init();
	bootblock_mainboard_early_init();

	if (IS_ENABLED(CONFIG_BOOTBLOCK_CONSOLE)) {
		console_init();
		exception_init();
	}

	bootblock_soc_init();
	bootblock_mainboard_init();

	run_romstage();
}

void main(void)
{
	uint64_t base_timestamp = 0;

	init_timer();

	if (IS_ENABLED(CONFIG_COLLECT_TIMESTAMPS))
		base_timestamp = timestamp_get();

	bootblock_main_with_timestamp(base_timestamp, NULL, 0);
}

#if IS_ENABLED(CONFIG_COMPRESS_BOOTBLOCK)
/*
 * This is the bootblock entry point when it is run after a decompressor stage.
 * For non-decompressor builds, _start is generally defined in architecture-
 * specific assembly code. In decompressor builds that architecture
 * initialization code already ran in the decompressor, so the bootblock can
 * start straight into common code with a C environment.
 */
void _start(struct bootblock_arg *arg);
void _start(struct bootblock_arg *arg)
{
	bootblock_main_with_timestamp(arg->base_timestamp, arg->timestamps,
				      arg->num_timestamps);
}

#endif
