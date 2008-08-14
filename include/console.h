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

#ifndef CONSOLE_H
#define CONSOLE_H

#include <types.h>
#include <shared.h> /* We share symbols from stage 0 */
#include <post_code.h>

#define BIOS_ALWAYS	0	/* log no matter what; not necessarily an error */
#define BIOS_EMERG      0   /* system is unusable                   */
#define BIOS_ALERT      1   /* action must be taken immediately     */
#define BIOS_CRIT       2   /* critical conditions                  */
#define BIOS_ERR        3   /* error conditions                     */
#define BIOS_WARNING    4   /* warning conditions                   */
#define BIOS_NOTICE     5   /* normal but significant condition     */
#define BIOS_INFO       6   /* informational                        */
#define BIOS_DEBUG      7   /* debug-level messages                 */
#define BIOS_SPEW       8   /* Way too many details                 */

void console_init(void);
void console_tx_byte(unsigned char byte, void *arg);
void console_tx_flush(void);
unsigned char console_rx_byte(void);
int console_tst_byte(void);
#ifdef CONFIG_CONSOLE_BUFFER
void printk_buffer_init(void);
void printk_buffer_move(void *newaddr, int newsize);
#endif

struct console_driver {
	void (*init)(void);
	void (*tx_byte)(unsigned char byte, void *arg);
	void (*tx_flush)(void);
	unsigned char (*rx_byte)(void);
	int (*tst_byte)(void);
};

#ifdef CONFIG_CONSOLE_BUFFER
struct printk_buffer {
	int len;
	int readoffset;
	int writeoffset;
	char buffer[];
};
#endif

/*
 * If you change struct global_vars in any way, you have to fix all stage0 asm
 * code. The stage0 asm code modification is nontrivial (size of the struct,
 * alignment, initialization, order of struct members, initialization).
 * Depending on your compiler, real breakage may happen.
 */
struct global_vars {
#ifdef CONFIG_CONSOLE_BUFFER
	struct printk_buffer *printk_buffer;
#endif
};

SHARED_WITH_ATTRIBUTES(printk, int, __attribute__((format (printf, 2, 3))),
					int msg_level, const char *fmt, ...);
SHARED(banner, void, int msg_level, const char *msg);
SHARED(dump_mem_range, void, int msg_level, unsigned char *buf, int size);
SHARED(die, void, const char *msg);

#endif /* CONSOLE_H */
