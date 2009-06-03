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

extern unsigned int console_loglevel;
int do_printk(int msg_level, const char *fmt, ...) __attribute__((format(printf, 2, 3)));

#define printk_emerg(fmt, arg...)   do_printk(BIOS_EMERG   ,fmt, ##arg)
#define printk_alert(fmt, arg...)   do_printk(BIOS_ALERT   ,fmt, ##arg)
#define printk_crit(fmt, arg...)    do_printk(BIOS_CRIT    ,fmt, ##arg)
#define printk_err(fmt, arg...)     do_printk(BIOS_ERR     ,fmt, ##arg)
#define printk_warning(fmt, arg...) do_printk(BIOS_WARNING ,fmt, ##arg)
#define printk_notice(fmt, arg...)  do_printk(BIOS_NOTICE  ,fmt, ##arg)
#define printk_info(fmt, arg...)    do_printk(BIOS_INFO    ,fmt, ##arg)
#define printk_debug(fmt, arg...)   do_printk(BIOS_DEBUG   ,fmt, ##arg)
#define printk_spew(fmt, arg...)    do_printk(BIOS_SPEW    ,fmt, ##arg)

#if MAXIMUM_CONSOLE_LOGLEVEL <= BIOS_EMERG
#undef  printk_emerg
#define printk_emerg(fmt, arg...)   do {} while(0)
#endif
#if MAXIMUM_CONSOLE_LOGLEVEL <= BIOS_ALERT
#undef  printk_alert
#define printk_alert(fmt, arg...)   do {} while(0)
#endif
#if MAXIMUM_CONSOLE_LOGLEVEL <= BIOS_CRIT
#undef  printk_crit
#define printk_crit(fmt, arg...)    do {} while(0)
#endif
#if MAXIMUM_CONSOLE_LOGLEVEL <= BIOS_ERR
#undef  printk_err
#define printk_err(fmt, arg...)     do {} while(0)
#endif
#if MAXIMUM_CONSOLE_LOGLEVEL <= BIOS_WARNING
#undef  printk_warning
#define printk_warning(fmt, arg...) do {} while(0)
#endif
#if MAXIMUM_CONSOLE_LOGLEVEL <= BIOS_NOTICE
#undef  printk_notice
#define printk_notice(fmt, arg...)  do {} while(0)
#endif
#if MAXIMUM_CONSOLE_LOGLEVEL <= BIOS_INFO
#undef  printk_info
#define printk_info(fmt, arg...)    do {} while(0)
#endif
#if MAXIMUM_CONSOLE_LOGLEVEL <= BIOS_DEBUG
#undef  printk_debug
#define printk_debug(fmt, arg...)   do {} while(0)
#endif
#if MAXIMUM_CONSOLE_LOGLEVEL <= BIOS_SPEW
#undef  printk_spew
#define printk_spew(fmt, arg...)    do {} while(0)
#endif

#define print_emerg(STR)   printk_emerg  ("%s", (STR))
#define print_alert(STR)   printk_alert  ("%s", (STR))
#define print_crit(STR)    printk_crit   ("%s", (STR))
#define print_err(STR)     printk_err    ("%s", (STR))
#define print_warning(STR) printk_warning("%s", (STR))
#define print_notice(STR)  printk_notice ("%s", (STR))
#define print_info(STR)    printk_info   ("%s", (STR))
#define print_debug(STR)   printk_debug  ("%s", (STR))
#define print_spew(STR)    printk_spew   ("%s", (STR))

#define print_emerg_char(CH)   printk_emerg  ("%c", (CH))
#define print_alert_char(CH)   printk_alert  ("%c", (CH))
#define print_crit_char(CH)    printk_crit   ("%c", (CH))
#define print_err_char(CH)     printk_err    ("%c", (CH))
#define print_warning_char(CH) printk_warning("%c", (CH))
#define print_notice_char(CH)  printk_notice ("%c", (CH))
#define print_info_char(CH)    printk_info   ("%c", (CH))
#define print_debug_char(CH)   printk_debug  ("%c", (CH))
#define print_spew_char(CH)    printk_spew   ("%c", (CH))

#define print_emerg_hex8(HEX)   printk_emerg  ("%02x",  (HEX))
#define print_alert_hex8(HEX)   printk_alert  ("%02x",  (HEX))
#define print_crit_hex8(HEX)    printk_crit   ("%02x",  (HEX))
#define print_err_hex8(HEX)     printk_err    ("%02x",  (HEX))
#define print_warning_hex8(HEX) printk_warning("%02x",  (HEX))
#define print_notice_hex8(HEX)  printk_notice ("%02x",  (HEX))
#define print_info_hex8(HEX)    printk_info   ("%02x",  (HEX))
#define print_debug_hex8(HEX)   printk_debug  ("%02x",  (HEX))
#define print_spew_hex8(HEX)    printk_spew   ("%02x",  (HEX))

#define print_emerg_hex16(HEX)   printk_emerg  ("%04x", (HEX))
#define print_alert_hex16(HEX)   printk_alert  ("%04x", (HEX))
#define print_crit_hex16(HEX)    printk_crit   ("%04x", (HEX))
#define print_err_hex16(HEX)     printk_err    ("%04x", (HEX))
#define print_warning_hex16(HEX) printk_warning("%04x", (HEX))
#define print_notice_hex16(HEX)  printk_notice ("%04x", (HEX))
#define print_info_hex16(HEX)    printk_info   ("%04x", (HEX))
#define print_debug_hex16(HEX)   printk_debug  ("%04x", (HEX))
#define print_spew_hex16(HEX)    printk_spew   ("%04x", (HEX))

#define print_emerg_hex32(HEX)   printk_emerg  ("%08x", (HEX))
#define print_alert_hex32(HEX)   printk_alert  ("%08x", (HEX))
#define print_crit_hex32(HEX)    printk_crit   ("%08x", (HEX))
#define print_err_hex32(HEX)     printk_err    ("%08x", (HEX))
#define print_warning_hex32(HEX) printk_warning("%08x", (HEX))
#define print_notice_hex32(HEX)  printk_notice ("%08x", (HEX))
#define print_info_hex32(HEX)    printk_info   ("%08x", (HEX))
#define print_debug_hex32(HEX)   printk_debug  ("%08x", (HEX))
#define print_spew_hex32(HEX)    printk_spew   ("%08x", (HEX))

#if CONFIG_CONSOLE_VGA == 1
void vga_console_init(void);
#endif

#endif /* CONSOLE_CONSOLE_H_ */
