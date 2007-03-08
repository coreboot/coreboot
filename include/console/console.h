/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef CONSOLE_CONSOLE_H_
#define CONSOLE_CONSOLE_H_

#include <arch/types.h>
#include <console/loglevel.h>

void console_init(void);
void console_tx_byte(unsigned char byte);
void console_tx_flush(void);
unsigned char console_rx_byte(void);
int console_tst_byte(void);
void post_code(u8 value);
void die(const char *msg);

struct console_driver {
	void (*init)(void);
	void (*tx_byte)(unsigned char byte);
	void (*tx_flush)(void);
	unsigned char (*rx_byte)(void);
	int (*tst_byte)(void);
};

#define __console	__attribute__((used, __section__ (".rodata.console_drivers")))

/* Defined by the linker... */
extern struct console_driver console_drivers[];
extern struct console_driver econsole_drivers[];

extern int console_loglevel;

// 
int printk(int msg_level, const char *fmt, ...);


#endif /* CONSOLE_CONSOLE_H_ */
