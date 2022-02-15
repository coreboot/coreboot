/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <commonlib/bsd/compression.h>
#include <delay.h>
#include <metadata_hash.h>
#include <program_loading.h>
#include <symbols.h>
#include <timestamp.h>

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
		{ .entry_id = TS_ULZ4F_START },
		{ .entry_id = TS_ULZ4F_END },
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

	if (CONFIG(COLLECT_TIMESTAMPS))
		arg.base_timestamp = timestamp_get();

	if (CONFIG(CBFS_VERIFICATION))
		arg.metadata_hash_anchor = metadata_hash_export_anchor();

	decompressor_soc_init();

	if (CONFIG(COLLECT_TIMESTAMPS))
		arg.timestamps[0].entry_stamp = timestamp_get();

	size_t out_size = ulz4f(compressed_bootblock, _bootblock);
	prog_segment_loaded((uintptr_t)_bootblock, out_size, SEG_FINAL);

	if (CONFIG(COLLECT_TIMESTAMPS))
		arg.timestamps[1].entry_stamp = timestamp_get();

	prog_run(&prog_bootblock);
}
