#include <console/loglevel.h>

#if CONFIG_USE_PRINTK_IN_CAR == 0
static void __console_tx_byte(unsigned char byte)
{
	uart_tx_byte(byte);
}

#include "console_print.c"

#else  
/* CONFIG_USE_INIT == 1 */

#include "console_printk.c"

#endif /* CONFIG_USE_INIT */

#ifndef LINUXBIOS_EXTRA_VERSION
#define LINUXBIOS_EXTRA_VERSION ""
#endif


static void console_init(void)
{
	static const char console_test[] = 
		"\r\n\r\nLinuxBIOS-"
		LINUXBIOS_VERSION
		LINUXBIOS_EXTRA_VERSION
		" "
		LINUXBIOS_BUILD
		" starting...\r\n";
	print_info(console_test);
}


static void die(const char *str)
{
	print_emerg(str);
	do {
		hlt();
	} while(1);
}
