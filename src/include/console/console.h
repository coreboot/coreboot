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
#include <arch/cpu.h>
#include <console/post_codes.h>

/* console.h is supposed to provide the log levels defined in here: */
#include <commonlib/loglevel.h>

#define RAM_DEBUG (CONFIG(DEBUG_RAM_SETUP) ? BIOS_DEBUG : BIOS_NEVER)
#define RAM_SPEW  (CONFIG(DEBUG_RAM_SETUP) ? BIOS_SPEW  : BIOS_NEVER)

#ifndef __ROMCC__

#include <console/vtxprintf.h>

void post_code(u8 value);
#if CONFIG(CMOS_POST_EXTRA)
void post_log_extra(u32 value);
struct device;
void post_log_path(const struct device *dev);
void post_log_clear(void);
#else
#define post_log_extra(x) do {} while (0)
#define post_log_path(x) do {} while (0)
#define post_log_clear() do {} while (0)
#endif
/* this function is weak and can be overridden by a mainboard function. */
void mainboard_post(u8 value);
void __noreturn die(const char *fmt, ...);
#define die_with_post_code(value, fmt, ...) \
	do { post_code(value); die(fmt, ##__VA_ARGS__); } while (0)

/*
 * This function is weak and can be overridden to provide additional
 * feedback to the user. Possible use case: Play a beep.
 */
void die_notify(void);

#define __CONSOLE_ENABLE__ \
	((ENV_BOOTBLOCK && CONFIG(BOOTBLOCK_CONSOLE)) || \
	(ENV_POSTCAR && CONFIG(POSTCAR_CONSOLE)) || \
	ENV_VERSTAGE || ENV_ROMSTAGE || ENV_RAMSTAGE || ENV_LIBAGESA || \
	(ENV_SMM && CONFIG(DEBUG_SMI)))

#if __CONSOLE_ENABLE__
asmlinkage void console_init(void);
int console_log_level(int msg_level);
void do_putchar(unsigned char byte);

/* Return number of microseconds elapsed from start of stage or the previous
   get_and_reset() call. */
long console_time_get_and_reset(void);
void console_time_report(void);

#define printk(LEVEL, fmt, args...) do_printk(LEVEL, fmt, ##args)
#define vprintk(LEVEL, fmt, args) do_vprintk(LEVEL, fmt, args)

enum { CONSOLE_LOG_NONE = 0, CONSOLE_LOG_FAST, CONSOLE_LOG_ALL };

#if CONFIG(CONSOLE_OVERRIDE_LOGLEVEL)
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
static inline void vprintk(int LEVEL, const char *fmt, va_list args) {}
static inline void do_putchar(unsigned char byte) {}
static inline long console_time_get_and_reset(void) { return 0; }
static inline void console_time_report(void) {}
#endif

int do_printk(int msg_level, const char *fmt, ...)
	__attribute__((format(printf, 2, 3)));

int do_vprintk(int msg_level, const char *fmt, va_list args);

#else

static inline void romcc_printk(void) { }
#define printk(...) romcc_printk()

#endif /* !__ROMCC__ */

#endif /* CONSOLE_CONSOLE_H_ */
