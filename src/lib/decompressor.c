/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google Inc.
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

#include <bootblock_common.h>
#include <commonlib/compression.h>
#include <delay.h>
#include <program_loading.h>
#include <symbols.h>

extern u8 compressed_bootblock[];
asm (
	".pushsection .data.compressed_bootblock,\"a\",@progbits\n\t"
	".type compressed_bootblock, %object\n\t"
	".balign 8\n"
	"compressed_bootblock:\n\t"
	".incbin \"" __BUILD_DIR__ "/cbfs/" CONFIG_CBFS_PREFIX "/bootblock.lz4\"\n\t"
	".size compressed_bootblock, . - compressed_bootblock\n\t"
	".popsection\n\t"
);

struct bootblock_arg arg = {
	.base_timestamp = 0,
	.num_timestamps = 2,
	.timestamps = {
		{ .entry_id = TS_START_ULZ4F },
		{ .entry_id = TS_END_ULZ4F },
	},
};

struct prog prog_bootblock = {
	.type = PROG_BOOTBLOCK,
	.entry = (void *)_bootblock,
	.arg = &arg,
};

__weak void decompressor_soc_init(void) { /* no-op */ }

void main(void)
{
	init_timer();

	if (IS_ENABLED(CONFIG_COLLECT_TIMESTAMPS))
		arg.base_timestamp = timestamp_get();

	decompressor_soc_init();

	if (IS_ENABLED(CONFIG_COLLECT_TIMESTAMPS))
		arg.timestamps[0].entry_stamp = timestamp_get();

	size_t out_size = ulz4f(compressed_bootblock, _bootblock);
	prog_segment_loaded((uintptr_t)_bootblock, out_size, SEG_FINAL);

	if (IS_ENABLED(CONFIG_COLLECT_TIMESTAMPS))
		arg.timestamps[1].entry_stamp = timestamp_get();

	prog_run(&prog_bootblock);
}
