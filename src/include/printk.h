#ifndef PRINTK_H
#define PRINTK_H

#include <loglevel.h>

extern int console_loglevel;
int do_printk(int msg_level, const char *fmt, ...);

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
#define printk_alart(fmt, arg...)   do {} while(0)
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

#endif
