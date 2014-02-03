/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Eric Biederman
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef CONSOLE_CONSOLE_H_
#define CONSOLE_CONSOLE_H_

#include <stdint.h>
#include <console/post_codes.h>

#ifndef __ROMCC__
int console_show(int msg_level);
void console_init(void);
void post_code(u8 value);
#if CONFIG_CMOS_POST_EXTRA
void post_log_extra(u32 value);
struct device;
void post_log_path(struct device *dev);
void post_log_clear(void);
#else
#define post_log_extra(x) do {} while (0)
#define post_log_path(x) do {} while (0)
#define post_log_clear() do {} while (0)
#endif
/* this function is weak and can be overridden by a mainboard function. */
void mainboard_post(u8 value);
void __attribute__ ((noreturn)) die(const char *msg);

#if defined(__BOOT_BLOCK__) && !CONFIG_BOOTBLOCK_CONSOLE || \
    defined(__SMM__) && !CONFIG_DEBUG_SMI || \
    (defined(__PRE_RAM__) && !defined(__BOOT_BLOCK__)) && !CONFIG_EARLY_CONSOLE

/* Do nothing. */
static inline void printk(int LEVEL, const char *fmt, ...) {}
static inline void do_putchar(unsigned char byte) {}

#else

int do_printk(int msg_level, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
void do_putchar(unsigned char byte);

#define printk(LEVEL, fmt, args...)				\
	do {							\
		do_printk(LEVEL, fmt, ##args);		\
	} while(0)

#endif

#if CONFIG_CHROMEOS
/* FIXME: Collision of varargs with AMD headers without guard. */
#include <console/vtxprintf.h>
#if !defined(__PRE_RAM__) || CONFIG_EARLY_CONSOLE
void do_vtxprintf(const char *fmt, va_list args);
#else
static inline void do_vtxprintf(const char *fmt, va_list args) {};
#endif
#endif

/* A lot of code still uses print_debug() et al. while use of printk()
 * would be preferred.
 */
#include <console/early_print.h>

#else /* __ROMCC__ */

#include "arch/x86/lib/romcc_console.c"

#endif /* __ROMCC__ */

#endif /* CONSOLE_CONSOLE_H_ */
