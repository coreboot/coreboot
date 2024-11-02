/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/tracker.h>
#include <soc/tracker_common.h>

__weak void tracker_setup(void)
{
	/* do nothing. */
}

static void tracker_dump_data(void)
{
	int i, j, k;
	int size;
	uintptr_t reg;
	struct tracker *tra;

	for (j = 0; j < TRACKER_NUM; j++) {
		tra = &tracker_data[j];

		if (!(read32((void *)(tra->base_addr)) & tra->timeout))
			continue;

		printk(BIOS_INFO, "**Dump %s debug register start**\n", tra->str);
		for (k = 0; k < tra->offsets_size; k++) {
			size = 2 * tra->entry;
			for (i = 0; i < size; i++) {
				reg = tra->base_addr + tra->offsets[k] + i * 4;
				printk(BIOS_INFO, "%#lx:%#x,", reg, read32((void *)reg));

				if (i % 4 == 3 || i == size - 1)
					printk(BIOS_INFO, "\n");
			}
		}

		printk(BIOS_INFO, "**Dump %s debug register end**\n", tra->str);
	}
}

void bustracker_init(void)
{
	tracker_dump_data();
	tracker_setup();
}
