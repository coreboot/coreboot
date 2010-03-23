#ifndef CONSOLE_CONSOLE_H_
#define CONSOLE_CONSOLE_H_

#include <stdint.h>
#include <console/loglevel.h>

void console_init(void);
void console_tx_byte(unsigned char byte);
void console_tx_flush(void);
unsigned char console_rx_byte(void);
int console_tst_byte(void);
void post_code(uint8_t value);
void __attribute__ ((noreturn)) die(const char *msg);

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
int do_printk(int msg_level, const char *fmt, ...) __attribute__((format(printf, 2, 3)));

#undef WE_CLEANED_UP_ALL_SIDE_EFFECTS
/* We saw some strange effects in the past like coreboot crashing while
 * disabling cache as ram for a maximum console log level of 6 and above while
 * it worked fine without. In order to catch such issues reliably we are
 * always doing a function call to do_printk with the full number of arguments.
 * This slightly increases the code size and some unprinted strings will end
 * up in the final coreboot binary (most of them compressed). If you want to
 * avoid this, do a
 * #define WE_CLEANED_UP_ALL_SIDE_EFFECTS
 */
#ifdef WE_CLEANED_UP_ALL_SIDE_EFFECTS

#define printk(LEVEL, fmt, args...)				\
	do {							\
		if (CONFIG_MAXIMUM_CONSOLE_LOGLEVEL >= LEVEL) {	\
			do_printk(LEVEL, fmt, ##args);		\
		}						\
	} while(0)

#else

#define printk(LEVEL, fmt, args...)				\
	do {							\
		if (CONFIG_MAXIMUM_CONSOLE_LOGLEVEL >= LEVEL) {	\
			do_printk(LEVEL, fmt, ##args);		\
		} else {					\
			do_printk(BIOS_NEVER, fmt, ##args);	\
		}						\
	} while(0)
#endif

#define print_emerg(STR)   printk(BIOS_EMERG,  "%s", (STR))
#define print_alert(STR)   printk(BIOS_ALERT,  "%s", (STR))
#define print_crit(STR)    printk(BIOS_CRIT,   "%s", (STR))
#define print_err(STR)     printk(BIOS_ERR,    "%s", (STR))
#define print_warning(STR) printk(BIOS_WARNING,"%s", (STR))
#define print_notice(STR)  printk(BIOS_NOTICE, "%s", (STR))
#define print_info(STR)    printk(BIOS_INFO,   "%s", (STR))
#define print_debug(STR)   printk(BIOS_DEBUG,  "%s", (STR))
#define print_spew(STR)    printk(BIOS_SPEW,   "%s", (STR))

#define print_emerg_char(CH)   printk(BIOS_EMERG,  "%c", (CH))
#define print_alert_char(CH)   printk(BIOS_ALERT,  "%c", (CH))
#define print_crit_char(CH)    printk(BIOS_CRIT,   "%c", (CH))
#define print_err_char(CH)     printk(BIOS_ERR,    "%c", (CH))
#define print_warning_char(CH) printk(BIOS_WARNING,"%c", (CH))
#define print_notice_char(CH)  printk(BIOS_NOTICE, "%c", (CH))
#define print_info_char(CH)    printk(BIOS_INFO,   "%c", (CH))
#define print_debug_char(CH)   printk(BIOS_DEBUG,  "%c", (CH))
#define print_spew_char(CH)    printk(BIOS_SPEW,   "%c", (CH))

#define print_emerg_hex8(HEX)   printk(BIOS_EMERG,  "%02x",  (HEX))
#define print_alert_hex8(HEX)   printk(BIOS_ALERT,  "%02x",  (HEX))
#define print_crit_hex8(HEX)    printk(BIOS_CRIT,   "%02x",  (HEX))
#define print_err_hex8(HEX)     printk(BIOS_ERR,    "%02x",  (HEX))
#define print_warning_hex8(HEX) printk(BIOS_WARNING,"%02x",  (HEX))
#define print_notice_hex8(HEX)  printk(BIOS_NOTICE, "%02x",  (HEX))
#define print_info_hex8(HEX)    printk(BIOS_INFO,   "%02x",  (HEX))
#define print_debug_hex8(HEX)   printk(BIOS_DEBUG,  "%02x",  (HEX))
#define print_spew_hex8(HEX)    printk(BIOS_SPEW,   "%02x",  (HEX))

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

#if CONFIG_CONSOLE_VGA == 1
void vga_console_init(void);
#endif

#endif /* CONSOLE_CONSOLE_H_ */
