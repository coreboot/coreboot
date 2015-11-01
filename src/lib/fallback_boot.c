#include <console/console.h>
#include <fallback.h>
#include <watchdog.h>
#include <arch/io.h>

#if CONFIG_PC80_SYSTEM
#include <pc80/mc146818rtc.h>

static void set_boot_successful(void)
{
	uint8_t index, byte;

	index = inb(RTC_PORT(0)) & 0x80;
	index |= RTC_BOOT_BYTE;
	outb(index, RTC_PORT(0));

	byte = inb(RTC_PORT(1));

	if (IS_ENABLED(CONFIG_SKIP_MAX_REBOOT_CNT_CLEAR)) {
		/* Set the fallback boot bit to allow for recovery if
		 * the payload fails to boot.
		 * It is the responsibility of the payload to reset
		 * the normal boot bit to 1 if desired
		 */
		byte &= ~RTC_BOOT_NORMAL;
	} else {
		/* If we are in normal mode set the boot count to 0 */
		if (byte & RTC_BOOT_NORMAL)
			byte &= 0x0f;

	}

	outb(byte, RTC_PORT(1));
}
#else
static void set_boot_successful(void)
{
	/* To be implemented */
}
#endif

void boot_successful(void)
{
#if CONFIG_FRAMEBUFFER_SET_VESA_MODE && !CONFIG_FRAMEBUFFER_KEEP_VESA_MODE
	void vbe_textmode_console(void);

	vbe_textmode_console();
#endif
	/* Remember this was a successful boot */
	set_boot_successful();

	/* turn off the boot watchdog */
	watchdog_off();
}
