#include <build.h>
#include <console/loglevel.h>

#if CONFIG_USE_PRINTK_IN_CAR == 0
#include "console_print.c"
#else  /* CONFIG_USE_PRINTK_IN_CAR == 1 */
#include <console/console.h>
#endif /* CONFIG_USE_PRINTK_IN_CAR */

void console_init(void)
{
	static const char console_test[] = 
		"\r\n\r\ncoreboot-"
		COREBOOT_VERSION
		COREBOOT_EXTRA_VERSION
		" "
		COREBOOT_BUILD
		" starting...\r\n";
	print_info(console_test);
}


void post_code(u8 value)
{
#if !defined(CONFIG_NO_POST) || CONFIG_NO_POST==0
#if CONFIG_SERIAL_POST==1
	print_emerg("POST: 0x");
	print_emerg_hex8(value);
	print_emerg("\r\n");
#endif
	outb(value, 0x80);
#endif
}

void die(const char *str)
{
	print_emerg(str);
	do {
		hlt();
	} while(1);
}
