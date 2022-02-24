/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <console/console.h>
#include <soc/psp_transfer.h>
#include <symbols.h>
#include <timestamp.h>

void boot_with_psp_timestamp(uint64_t base_timestamp)
{
	const struct transfer_info_struct *info = (const struct transfer_info_struct *)
		(void *)(uintptr_t)_transfer_buffer;

	if (!transfer_buffer_valid(info) || info->timestamp == 0)
		return;

	replay_transfer_buffer_cbmemc(info);

	/*
	 * info->timestamp is PSP's timestamp (in microseconds)
	 * when x86 processor is released.
	 */
	uint64_t psp_last_ts = info->timestamp;

	int i;
	struct timestamp_table *psp_ts_table =
		(struct timestamp_table *)(void *)
		((uintptr_t)_transfer_buffer + info->timestamp_offset);
	/* new base_timestamp will be offset for all PSP timestamps. */
	base_timestamp -= psp_last_ts;

	for (i = 0; i < psp_ts_table->num_entries; i++) {
		struct timestamp_entry *tse = &psp_ts_table->entries[i];
		/*
		 * We ignore the time between x86 processor release and bootblock.
		 * Since timestamp_add subtracts base_time, we first add old base_time
		 * to make it absolute then add base_timestamp again since
		 * it'll be a new base_time.
		 *
		 * We don't need to convert unit since both PSP and coreboot
		 * will use 1us granularity.
		 *
		 */
		tse->entry_stamp += psp_ts_table->base_time + base_timestamp;
	}

	bootblock_main_with_timestamp(base_timestamp, psp_ts_table->entries,
				      psp_ts_table->num_entries);
}
