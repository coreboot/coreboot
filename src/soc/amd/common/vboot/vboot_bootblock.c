/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <console/console.h>
#include <cpu/x86/tsc.h>
#include <soc/psp_transfer.h>
#include <symbols.h>
#include <timestamp.h>
#include <types.h>

/*
 * Verstage on PSP uses the weak timestamp_tick_freq_mhz implementation returning 1, so the
 * unit of the timestamps in the transfer buffer is microseconds. The x86 side uses the TSC
 * rate as reference for the timestamps, so the values from the PSP transfer buffer need to be
 * multiplied by the TSC frequency in MHz.
 */
static uint64_t timestamp_from_usec(uint64_t usec)
{
	return usec * tsc_freq_mhz();
}

void boot_with_psp_timestamp(uint64_t base_timestamp)
{
	const struct transfer_info_struct *info = (const struct transfer_info_struct *)
		(void *)(uintptr_t)_transfer_buffer;

	if (!transfer_buffer_valid(info) || info->timestamp == 0)
		return;

	replay_transfer_buffer_cbmemc();

	/*
	 * info->timestamp is PSP's timestamp (in microseconds)
	 * when x86 processor is released.
	 */
	uint64_t psp_last_ts_usec = info->timestamp;

	int i;
	struct timestamp_table *psp_ts_table =
		(struct timestamp_table *)(void *)
		((uintptr_t)_transfer_buffer + info->timestamp_offset);
	/* new base_timestamp will be offset for all PSP timestamps. */
	base_timestamp -= timestamp_from_usec(psp_last_ts_usec);

	for (i = 0; i < psp_ts_table->num_entries; i++) {
		struct timestamp_entry *tse = &psp_ts_table->entries[i];
		/*
		 * We ignore the time between x86 processor release and bootblock.
		 * Since timestamp_add subtracts base_time, we first add old base_time
		 * to make it absolute then add base_timestamp again since
		 * it'll be a new base_time.
		 *
		 * Verstage on PSP uses a 1 microsecond timestamp granularity while the x86
		 * part of coreboot uses the TSC tick time as granularity, so this needs to be
		 * converted.
		 */
		tse->entry_stamp += timestamp_from_usec(psp_ts_table->base_time) +
			base_timestamp;
	}

	bootblock_main_with_timestamp(base_timestamp, psp_ts_table->entries,
				      psp_ts_table->num_entries);
}
