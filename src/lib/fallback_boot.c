#include <console/console.h>
#include <fallback.h>
#include <watchdog.h>
#include <pc80/mc146818rtc.h>
#include <arch/io.h>


void set_boot_successful(void)
{
	/* Remember I succesfully booted by setting
	 * the initial boot direction
	 * to the direction that I booted.
	 */
	unsigned char index, byte;
	index = inb(RTC_PORT(0)) & 0x80;
	index |= RTC_BOOT_BYTE;
	outb(index, RTC_PORT(0));

	byte = inb(RTC_PORT(1));
	byte &= 0xfe;
	byte |= (byte & (1 << 1)) >> 1;

	/* If we are in normal mode set the boot count to 0 */
	if(byte & 1)
		byte &= 0x0f;
	outb(byte, RTC_PORT(1));
}

void boot_successful(void)
{
#if CONFIG_BOOTSPLASH && !CONFIG_COREBOOT_KEEP_FRAMEBUFFER
	void vbe_textmode_console(void);

	vbe_textmode_console();
#endif
	/* Remember this was a successful boot */
	set_boot_successful();

	/* turn off the boot watchdog */
	watchdog_off();
}
