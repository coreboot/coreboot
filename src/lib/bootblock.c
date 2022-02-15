/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <arch/exception.h>
#include <bootblock_common.h>
#include <console/console.h>
#include <delay.h>
#include <metadata_hash.h>
#include <option.h>
#include <post.h>
#include <program_loading.h>
#include <security/tpm/tspi.h>
#include <symbols.h>
#include <timestamp.h>

__weak void bootblock_mainboard_early_init(void) { /* no-op */ }
__weak void bootblock_soc_early_init(void) { /* do nothing */ }
__weak void bootblock_soc_init(void) { /* do nothing */ }
__weak void bootblock_mainboard_init(void) { /* do nothing */ }

/*
 * This is a the same as the bootblock main(), with the difference that it does
 * not collect a timestamp. Instead it accepts the initial timestamp and
 * possibly additional timestamp entries as arguments. This can be used in cases
 * where earlier stamps are available. Note that this function is designed to be
 * entered from C code. This function assumes that the timer has already been
 * initialized, so it does not call init_timer().
 */
void bootblock_main_with_timestamp(uint64_t base_timestamp,
	struct timestamp_entry *timestamps, size_t num_timestamps)
{
	/* Initialize timestamps if we have TIMESTAMP region in memlayout.ld. */
	if (CONFIG(COLLECT_TIMESTAMPS) &&
	    REGION_SIZE(timestamp) > 0) {
		int i;
		timestamp_init(base_timestamp);
		for (i = 0; i < num_timestamps; i++)
			timestamp_add(timestamps[i].entry_id,
				      timestamps[i].entry_stamp);
	}

	timestamp_add_now(TS_BOOTBLOCK_START);

	bootblock_soc_early_init();
	bootblock_mainboard_early_init();

	if (CONFIG(USE_OPTION_TABLE))
		sanitize_cmos();

	if (CONFIG(CMOS_POST))
		cmos_post_init();

	if (CONFIG(BOOTBLOCK_CONSOLE)) {
		console_init();
		exception_init();
	}

	bootblock_soc_init();
	bootblock_mainboard_init();

	if (CONFIG(TPM_MEASURED_BOOT_INIT_BOOTBLOCK)) {
		int s3resume = acpi_is_wakeup_s3();
		tpm_setup(s3resume);
	}

	timestamp_add_now(TS_BOOTBLOCK_END);

	run_romstage();
}

void bootblock_main_with_basetime(uint64_t base_timestamp)
{
	bootblock_main_with_timestamp(base_timestamp, NULL, 0);
}

void main(void)
{
	uint64_t base_timestamp = 0;

	init_timer();

	if (CONFIG(COLLECT_TIMESTAMPS))
		base_timestamp = timestamp_get();

	bootblock_main_with_timestamp(base_timestamp, NULL, 0);
}

#if CONFIG(COMPRESS_BOOTBLOCK)
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
	if (CONFIG(CBFS_VERIFICATION))
		metadata_hash_import_anchor(arg->metadata_hash_anchor);
	bootblock_main_with_timestamp(arg->base_timestamp, arg->timestamps,
				      arg->num_timestamps);
}

#endif
