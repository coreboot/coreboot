/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/barrier.h>
#include <cbfs.h>
#include <console/console.h>
#include <soc/mcu_common.h>
#include <soc/symbols.h>
#include <timer.h>

int mtk_init_mcu(struct mtk_mcu *mcu)
{
	struct stopwatch sw;

	if (!mcu)
		return CB_ERR_ARG;

	stopwatch_init(&sw);

	mcu->run_size = cbfs_load(mcu->firmware_name, mcu->load_buffer, mcu->buffer_size);
	if (mcu->run_size == 0) {
		printk(BIOS_ERR, "%s: Failed to load %s\n", __func__, mcu->firmware_name);
		return CB_ERR;
	}

	if (mcu->run_address) {
		memcpy(mcu->run_address, mcu->load_buffer, mcu->run_size);
		/* Memory barrier to ensure data is flushed before resetting MCU. */
		mb();
	}

	if (mcu->reset)
		mcu->reset(mcu);

	printk(BIOS_DEBUG, "%s: Loaded (and reset) %s in %lld msecs (%zd bytes)\n",
	       __func__, mcu->firmware_name, stopwatch_duration_msecs(&sw), mcu->run_size);

	return CB_SUCCESS;
}
