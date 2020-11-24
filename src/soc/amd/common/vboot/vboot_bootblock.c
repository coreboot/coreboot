/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/reset.h>
#include <bl_uapp/bl_syscall_public.h>
#include <bootblock_common.h>
#include <console/console.h>
#include <pc80/mc146818rtc.h>
#include <security/vboot/vbnv.h>
#include <security/vboot/symbols.h>
#include <soc/psp_transfer.h>
#include <timestamp.h>
#include <2struct.h>

static int transfer_buffer_valid(const struct transfer_info_struct *ptr)
{
	if (ptr->magic_val == TRANSFER_MAGIC_VAL)
		return 1;
	else
		return 0;
}

void verify_psp_transfer_buf(void)
{
	if (*(uint32_t *)_vboot2_work == VB2_SHARED_DATA_MAGIC) {
		cmos_write(0x00, CMOS_RECOVERY_BYTE);
		return;
	}

	/*
	 * If CMOS is valid and the system has already been rebooted once, but
	 * still returns here, instead of rebooting to verstage again, assume
	 * that the system is in a reboot loop and halt.
	 */
	if ((!vbnv_cmos_failed()) && cmos_read(CMOS_RECOVERY_BYTE) ==
			CMOS_RECOVERY_MAGIC_VAL)
		die("Error: Reboot into recovery was unsuccessful.  Halting.");

	printk(BIOS_ERR, "ERROR: VBOOT workbuf not valid.\n");
	printk(BIOS_DEBUG, "Signature: %#08x\n", *(uint32_t *)_vboot2_work);
	cmos_init(0);
	cmos_write(CMOS_RECOVERY_MAGIC_VAL, CMOS_RECOVERY_BYTE);
	warm_reset();
}

void show_psp_transfer_info(void)
{
	struct transfer_info_struct *info = (struct transfer_info_struct *)
			(void *)(uintptr_t)_transfer_buffer;

	if (transfer_buffer_valid(info)) {
		if ((info->psp_info & PSP_INFO_VALID) == 0) {
			printk(BIOS_INFO, "No PSP info found in transfer buffer.\n");
			return;
		}

		printk(BIOS_INFO, "PSP boot mode: %s\n",
				info->psp_info & PSP_INFO_PRODUCTION_MODE ?
				"Production" : "Development");
		printk(BIOS_INFO, "Silicon level: %s\n",
				info->psp_info & PSP_INFO_PRODUCTION_SILICON ?
				"Production" : "Pre-Production");
	}
}

void boot_with_psp_timestamp(uint64_t base_timestamp)
{
	const struct transfer_info_struct *info = (const struct transfer_info_struct *)
		(void *)(uintptr_t)_transfer_buffer;

	if (!transfer_buffer_valid(info) || info->timestamp == 0)
		return;

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
