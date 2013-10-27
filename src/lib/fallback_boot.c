#include <console/console.h>
#include <fallback.h>
#include <watchdog.h>
#include <arch/io.h>

void boot_successful(void)
{
#if CONFIG_FRAMEBUFFER_SET_VESA_MODE && !CONFIG_FRAMEBUFFER_KEEP_VESA_MODE
	void vbe_textmode_console(void);

	vbe_textmode_console();
#endif

/* We want to only do it at resume in the case of CONFIG_KEEP_BOOT_COUNT */
#if !CONFIG_KEEP_BOOT_COUNT
	/* Remember this was a successful boot */
	set_boot_successful();
#endif

	/* turn off the boot watchdog */
	watchdog_off();
}
