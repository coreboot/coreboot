#ifndef _PRINTK_H
#define _PRINTK_H
#include <console/loglevel.h>

extern int do_printk(int, const char *, ...);

#define printk_emerg(fmt, arg...)   do_printk(BIOS_EMERG   ,fmt, ##arg)
#define printk_alert(fmt, arg...)   do_printk(BIOS_ALERT   ,fmt, ##arg)
#define printk_crit(fmt, arg...)    do_printk(BIOS_CRIT    ,fmt, ##arg)
#define printk_err(fmt, arg...)     do_printk(BIOS_ERR     ,fmt, ##arg)
#define printk_warning(fmt, arg...) do_printk(BIOS_WARNING ,fmt, ##arg)
#define printk_notice(fmt, arg...)  do_printk(BIOS_NOTICE  ,fmt, ##arg)
#define printk_info(fmt, arg...)    do_printk(BIOS_INFO    ,fmt, ##arg)
#define printk_debug(fmt, arg...)   do_printk(BIOS_DEBUG   ,fmt, ##arg)
#define printk_spew(fmt, arg...)    do_printk(BIOS_SPEW    ,fmt, ##arg)

#endif /* _PRINTK_H */

