/* SPDX-License-Identifier: GPL-2.0-only */

#include <fallback.h>
#include <watchdog.h>

/* Implement platform specific override. */
void __weak set_boot_successful(void) { }

void boot_successful(void)
{
	if (CONFIG(FRAMEBUFFER_SET_VESA_MODE) &&
	    CONFIG(VGA_TEXT_FRAMEBUFFER)) {
		void vbe_textmode_console(void);

		vbe_textmode_console();
	}

	/* Remember this was a successful boot */
	set_boot_successful();

	/* turn off the boot watchdog */
	watchdog_off();
}
