#include <console/console.h>
#include <fallback.h>
#include <string.h>
#include <watchdog.h>
#include <arch/acpi.h>
#include <arch/io.h>

void boot_successful(void)
{
#if CONFIG_FRAMEBUFFER_SET_VESA_MODE && !CONFIG_FRAMEBUFFER_KEEP_VESA_MODE
	void vbe_textmode_console(void);

	vbe_textmode_console();
#endif

#if CONFIG_KEEP_BOOT_COUNT
	/* we don't want to resume with the wrong prefix next time */
	if (acpi_slp_type == 3 &&
	    strncmp(CONFIG_CBFS_PREFIX, "fallback", sizeof("fallback")))
		set_boot_successful();
#else
	/* Remember this was a successful boot */
	set_boot_successful();
#endif

	/* turn off the boot watchdog */
	watchdog_off();
}
