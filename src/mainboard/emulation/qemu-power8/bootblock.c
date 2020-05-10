/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <program_loading.h>

void qemu_power8_main(void);

/* The qemu part of all this is very, very non-hardware like.
 * So it gets its own bootblock.
 */
void qemu_power8_main(void)
{
	if (CONFIG(BOOTBLOCK_CONSOLE)) {
		console_init();
	}

	run_romstage();
}
