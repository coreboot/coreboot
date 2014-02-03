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
#include <console/loglevel.h>
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

#include <console/vtxprintf.h>

#if defined(__BOOT_BLOCK__) && !CONFIG_BOOTBLOCK_CONSOLE || \
    defined(__SMM__) && !CONFIG_DEBUG_SMI || \
    (defined(__PRE_RAM__) && !defined(__BOOT_BLOCK__)) && !CONFIG_EARLY_CONSOLE

/* Do nothing. */
static inline void printk(int LEVEL, const char *fmt, ...) {}
static inline void do_vtxprintf(const char *fmt, va_list args) {}
static inline void do_putchar(unsigned char byte) {}

#else

int do_printk(int msg_level, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
void do_vtxprintf(const char *fmt, va_list args);
void do_putchar(unsigned char byte);

#define printk(LEVEL, fmt, args...)				\
	do {							\
		do_printk(LEVEL, fmt, ##args);		\
	} while(0)

#endif

#define print_emerg(STR)         printk(BIOS_EMERG,  "%s", (STR))
#define print_alert(STR)         printk(BIOS_ALERT,  "%s", (STR))
#define print_crit(STR)          printk(BIOS_CRIT,   "%s", (STR))
#define print_err(STR)           printk(BIOS_ERR,    "%s", (STR))
#define print_warning(STR)       printk(BIOS_WARNING,"%s", (STR))
#define print_notice(STR)        printk(BIOS_NOTICE, "%s", (STR))
#define print_info(STR)          printk(BIOS_INFO,   "%s", (STR))
#define print_debug(STR)         printk(BIOS_DEBUG,  "%s", (STR))
#define print_spew(STR)          printk(BIOS_SPEW,   "%s", (STR))

#define print_emerg_char(CH)     printk(BIOS_EMERG,  "%c", (CH))
#define print_alert_char(CH)     printk(BIOS_ALERT,  "%c", (CH))
#define print_crit_char(CH)      printk(BIOS_CRIT,   "%c", (CH))
#define print_err_char(CH)       printk(BIOS_ERR,    "%c", (CH))
#define print_warning_char(CH)   printk(BIOS_WARNING,"%c", (CH))
#define print_notice_char(CH)    printk(BIOS_NOTICE, "%c", (CH))
#define print_info_char(CH)      printk(BIOS_INFO,   "%c", (CH))
#define print_debug_char(CH)     printk(BIOS_DEBUG,  "%c", (CH))
#define print_spew_char(CH)      printk(BIOS_SPEW,   "%c", (CH))

#define print_emerg_hex8(HEX)    printk(BIOS_EMERG,  "%02x",  (HEX))
#define print_alert_hex8(HEX)    printk(BIOS_ALERT,  "%02x",  (HEX))
#define print_crit_hex8(HEX)     printk(BIOS_CRIT,   "%02x",  (HEX))
#define print_err_hex8(HEX)      printk(BIOS_ERR,    "%02x",  (HEX))
#define print_warning_hex8(HEX)  printk(BIOS_WARNING,"%02x",  (HEX))
#define print_notice_hex8(HEX)   printk(BIOS_NOTICE, "%02x",  (HEX))
#define print_info_hex8(HEX)     printk(BIOS_INFO,   "%02x",  (HEX))
#define print_debug_hex8(HEX)    printk(BIOS_DEBUG,  "%02x",  (HEX))
#define print_spew_hex8(HEX)     printk(BIOS_SPEW,   "%02x",  (HEX))

#define print_emerg_hex16(HEX)   printk(BIOS_EMERG,  "%04x", (HEX))
#define print_alert_hex16(HEX)   printk(BIOS_ALERT,  "%04x", (HEX))
#define print_crit_hex16(HEX)    printk(BIOS_CRIT,   "%04x", (HEX))
#define print_err_hex16(HEX)     printk(BIOS_ERR,    "%04x", (HEX))
#define print_warning_hex16(HEX) printk(BIOS_WARNING,"%04x", (HEX))
#define print_notice_hex16(HEX)  printk(BIOS_NOTICE, "%04x", (HEX))
#define print_info_hex16(HEX)    printk(BIOS_INFO,   "%04x", (HEX))
#define print_debug_hex16(HEX)   printk(BIOS_DEBUG,  "%04x", (HEX))
#define print_spew_hex16(HEX)    printk(BIOS_SPEW,   "%04x", (HEX))

#define print_emerg_hex32(HEX)   printk(BIOS_EMERG,  "%08x", (HEX))
#define print_alert_hex32(HEX)   printk(BIOS_ALERT,  "%08x", (HEX))
#define print_crit_hex32(HEX)    printk(BIOS_CRIT,   "%08x", (HEX))
#define print_err_hex32(HEX)     printk(BIOS_ERR,    "%08x", (HEX))
#define print_warning_hex32(HEX) printk(BIOS_WARNING,"%08x", (HEX))
#define print_notice_hex32(HEX)  printk(BIOS_NOTICE, "%08x", (HEX))
#define print_info_hex32(HEX)    printk(BIOS_INFO,   "%08x", (HEX))
#define print_debug_hex32(HEX)   printk(BIOS_DEBUG,  "%08x", (HEX))
#define print_spew_hex32(HEX)    printk(BIOS_SPEW,   "%08x", (HEX))

#else

#include "arch/x86/lib/romcc_console.c"

#endif /* __ROMCC__ */

#endif /* CONSOLE_CONSOLE_H_ */
