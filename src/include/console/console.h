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
 */

#ifndef CONSOLE_CONSOLE_H_
#define CONSOLE_CONSOLE_H_

#include <stdint.h>
#include <rules.h>
#include <arch/cpu.h>
#include <console/post_codes.h>
#include <commonlib/loglevel.h>

#define RAM_DEBUG (IS_ENABLED(CONFIG_DEBUG_RAM_SETUP) ? BIOS_DEBUG : BIOS_NEVER)
#define RAM_SPEW  (IS_ENABLED(CONFIG_DEBUG_RAM_SETUP) ? BIOS_SPEW  : BIOS_NEVER)

#ifndef __ROMCC__

void post_code(u8 value);
#if IS_ENABLED(CONFIG_CMOS_POST_EXTRA)
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
void __attribute__((noreturn)) die(const char *msg);

/*
 * This function is weak and can be overridden to provide additional
 * feedback to the user. Possible use case: Play a beep.
 */
void die_notify(void);

#define __CONSOLE_ENABLE__ \
	((ENV_BOOTBLOCK && IS_ENABLED(CONFIG_BOOTBLOCK_CONSOLE)) || \
	(ENV_POSTCAR && IS_ENABLED(CONFIG_POSTCAR_CONSOLE)) || \
	ENV_VERSTAGE || ENV_ROMSTAGE || ENV_RAMSTAGE || ENV_LIBAGESA || \
	(ENV_SMM && IS_ENABLED(CONFIG_DEBUG_SMI)))

#if __CONSOLE_ENABLE__
asmlinkage void console_init(void);
int console_log_level(int msg_level);
int do_printk(int msg_level, const char *fmt, ...)
	__attribute__((format(printf, 2, 3)));
void do_putchar(unsigned char byte);

#define printk(LEVEL, fmt, args...) \
	do { do_printk(LEVEL, fmt, ##args); } while (0)

#if IS_ENABLED(CONFIG_CONSOLE_OVERRIDE_LOGLEVEL)
/*
 * This function should be implemented at mainboard level.
 * The returned value will _replace_ the loglevel value;
 */
int get_console_loglevel(void);
#else
static inline int get_console_loglevel(void)
{
	return CONFIG_DEFAULT_CONSOLE_LOGLEVEL;
}
#endif
#else
static inline void console_init(void) {}
static inline int console_log_level(int msg_level) { return 0; }
static inline void printk(int LEVEL, const char *fmt, ...) {}
static inline void do_putchar(unsigned char byte) {}
#endif

#if IS_ENABLED(CONFIG_VBOOT)
/* FIXME: Collision of varargs with AMD headers without guard. */
#include <console/vtxprintf.h>
#if __CONSOLE_ENABLE__
void do_printk_va_list(int msg_level, const char *fmt, va_list args);
#else
static inline void do_printk_va_list(int l, const char *fmt, va_list args) {}
#endif
#endif

#endif /* !__ROMCC__ */

#endif /* CONSOLE_CONSOLE_H_ */
