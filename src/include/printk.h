#ifndef PRINTK_H
#define PRINTK_H

#ifndef MAXIMUM_CONSOLE_LOGLEVEL
#define MAXIMUM_CONSOLE_LOGLEVEL 8
#endif

#define BIOS_EMERG      0   /* system is unusable                   */
#define BIOS_ALERT      1   /* action must be taken immediately     */
#define BIOS_CRIT       2   /* critical conditions                  */
#define BIOS_ERR        3   /* error conditions                     */
#define BIOS_WARNING    4   /* warning conditions                   */
#define BIOS_NOTICE     5   /* normal but significant condition     */
#define BIOS_INFO       6   /* informational                        */
#define BIOS_DEBUG      7   /* debug-level messages                 */
#define BIOS_SPEW       8   /* Way too many details                 */

extern int console_loglevel;
int do_printk(int msg_level, const char *fmt, ...);

#define printk_emerg(fmt, arg...)   do_printk(BIOS_EMERG   ,fmt, ##arg)
#define printk_alart(fmt, arg...)   do_printk(BIOS_ALERT   ,fmt, ##arg)
#define printk_crit(fmt, arg...)    do_printk(BIOS_CRIT    ,fmt, ##arg)
#define printk_err(fmt, arg...)     do_printk(BIOS_ERR     ,fmt, ##arg)
#define printk_warning(fmt, arg...) do_printk(BIOS_WARNING ,fmt, ##arg)
#define printk_notice(fmt, arg...)  do_printk(BIOS_NOTICE  ,fmt, ##arg)
#define printk_info(fmt, arg...)    do_printk(BIOS_INFO    ,fmt, ##arg)
#define printk_debug(fmt, arg...)   do_printk(BIOS_DEBUG   ,fmt, ##arg)
#define printk_spew(fmt, arg...)    do_printk(BIOS_SPEW    ,fmt, ##arg)

#if MAXIMUM_CONSOLE_LOGLEVEL <= 0
#undef  printk_emerg
#define printk_emerg(fmt, arg...)   do {} while(0)
#endif
#if MAXIMUM_CONSOLE_LOGLEVEL <= 1
#undef  printk_alert
#define printk_alart(fmt, arg...)   do {} while(0)
#endif
#if MAXIMUM_CONSOLE_LOGLEVEL <= 2
#undef  printk_crit
#define printk_crit(fmt, arg...)    do {} while(0)
#endif
#if MAXIMUM_CONSOLE_LOGLEVEL <= 3
#undef  printk_err
#define printk_err(fmt, arg...)     do {} while(0)
#endif
#if MAXIMUM_CONSOLE_LOGLEVEL <= 4
#undef  printk_warning
#define printk_warning(fmt, arg...) do {} while(0)
#endif
#if MAXIMUM_CONSOLE_LOGLEVEL <= 5
#undef  printk_notice
#define printk_notice(fmt, arg...)  do {} while(0)
#endif
#if MAXIMUM_CONSOLE_LOGLEVEL <= 6
#undef  printk_info
#define printk_info(fmt, arg...)    do {} while(0)
#endif
#if MAXIMUM_CONSOLE_LOGLEVEL <= 7
#undef  printk_debug
#define printk_debug(fmt, arg...)   do {} while(0)
#endif
#if MAXIMUM_CONSOLE_LOGLEVEL <= 8
#undef  printk_spew
#define printk_spew(fmt, arg...)    do {} while(0)
#endif

#endif
