#include <part/fallback_boot.h>
#include <pc80/mc146818rtc.h>
#include <arch/io.h>
#include <printk.h>

void boot_successful(void)
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
	byte |= (byte & 2) >> 1;
	outb(byte, RTC_PORT(1));
}
