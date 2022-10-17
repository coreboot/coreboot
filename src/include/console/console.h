/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CONSOLE_CONSOLE_H_
#define CONSOLE_CONSOLE_H_

#include <commonlib/console/post_codes.h>
#include <console/vtxprintf.h>
#include <stdint.h>

/* console.h is supposed to provide the log levels defined in here: */
#include <commonlib/loglevel.h>

#define RAM_DEBUG (CONFIG(DEBUG_RAM_SETUP) ? BIOS_DEBUG : BIOS_NEVER)
#define RAM_SPEW  (CONFIG(DEBUG_RAM_SETUP) ? BIOS_SPEW  : BIOS_NEVER)

void post_code(u8 value);
void mainboard_post(u8 value);
void arch_post_code(u8 value);
void soc_post_code(uint8_t value);

void __noreturn die(const char *fmt, ...);
#define die_with_post_code(value, fmt, ...) \
	do { post_code(value); die(fmt, ##__VA_ARGS__); } while (0)

/*
 * This function is weak and can be overridden to provide additional
 * feedback to the user. Possible use case: Play a beep.
 */
void die_notify(void);

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

#define __CONSOLE_ENABLE__ \
	((ENV_BOOTBLOCK && CONFIG(BOOTBLOCK_CONSOLE)) || \
	 (ENV_POSTCAR && CONFIG(POSTCAR_CONSOLE)) || \
	 ENV_SEPARATE_VERSTAGE || ENV_ROMSTAGE || ENV_RAMSTAGE || \
	 ENV_LIBAGESA || (ENV_SMM && CONFIG(DEBUG_SMI)))

#if __CONSOLE_ENABLE__
int get_log_level(void);
void console_init(void);
int console_log_level(int msg_level);

int printk(int msg_level, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
int vprintk(int msg_level, const char *fmt, va_list args);

void do_putchar(unsigned char byte);

/* Return number of microseconds elapsed from start of stage or the previous
   get_and_reset() call. */
long console_time_get_and_reset(void);
void console_time_report(void);

/*
 * "Fast" basically means only the CBMEM console right now. This is used to still
 * print debug messages there when loglevel disables the other consoles. It is also
 * used to compile-time eliminate code paths that only affect "interactive" consoles
 * (which are all "slow") when none of those are enabled.
 */
enum { CONSOLE_LOG_NONE = 0, CONSOLE_LOG_FAST, CONSOLE_LOG_ALL };
#define HAS_ONLY_FAST_CONSOLES !(CONFIG(SPKMODEM) || CONFIG(CONSOLE_QEMU_DEBUGCON) || \
	CONFIG(CONSOLE_SERIAL) || CONFIG(CONSOLE_NE2K) || CONFIG(CONSOLE_USB) || \
	CONFIG(EM100PRO_SPI_CONSOLE) || CONFIG(CONSOLE_SPI_FLASH) || \
	CONFIG(CONSOLE_SYSTEM76_EC))

#else
static inline int get_log_level(void) { return -1; }
static inline void console_init(void) {}
static inline int console_log_level(int msg_level) { return 0; }
static inline int
	__attribute__((format(printf, 2, 3)))
	printk(int LEVEL, const char *fmt, ...) { return 0; }
static inline int vprintk(int LEVEL, const char *fmt, va_list args) { return 0; }
static inline void do_putchar(unsigned char byte) {}
static inline long console_time_get_and_reset(void) { return 0; }
static inline void console_time_report(void) {}
#endif

#endif /* CONSOLE_CONSOLE_H_ */
