/* tag: legacybios environment, executable code
 *
 * Copyright (C) 2003 Stefan Reinauer
 *
 * See the file "COPYING" for further information about
 * the copyright and warranty status of this work.
 */

#include "config.h"
#include "types.h"

void printk(const char *fmt, ...);
void cls(void);
#ifdef DEBUG_CONSOLE
int uart_init(int port, unsigned long speed);
#endif


void legacybios(ucell romstart, ucell romend)
{
#ifdef DEBUG_CONSOLE
	uart_init(SERIAL_PORT, SERIAL_SPEED);
	/* Clear the screen.  */
	cls();
#endif

#ifdef DEBUG_BOOT
	printk("LegacyBIOS started.\n");
#endif
	
	return;
}
