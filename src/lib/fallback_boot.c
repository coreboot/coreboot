#include <fallback.h>
#include <compiler.h>
#include <watchdog.h>

/* Implement platform specific override. */
void __weak set_boot_successful(void) { }

void boot_successful(void)
{
	if (IS_ENABLED(CONFIG_FRAMEBUFFER_SET_VESA_MODE) &&
	    IS_ENABLED(CONFIG_VGA_TEXT_FRAMEBUFFER)) {
		void vbe_textmode_console(void);

		vbe_textmode_console();
	}

	/* Remember this was a successful boot */
	set_boot_successful();

	/* turn off the boot watchdog */
	watchdog_off();
}
