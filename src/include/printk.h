#ifndef PRINTK_H
#define PRINTK_H

/* These defines copied from linux/include/linux/kernel.h */

#define KERN_EMERG      "<0>"   /* system is unusable                   */
#define KERN_ALERT      "<1>"   /* action must be taken immediately     */
#define KERN_CRIT       "<2>"   /* critical conditions                  */
#define KERN_ERR        "<3>"   /* error conditions                     */
#define KERN_WARNING    "<4>"   /* warning conditions                   */
#define KERN_NOTICE     "<5>"   /* normal but significant condition     */
#define KERN_INFO       "<6>"   /* informational                        */
#define KERN_DEBUG      "<7>"   /* debug-level messages                 */
#define KERN_SPEW       "<8>"   /* Way too many details                 */

extern int console_loglevel;
int printk(const char *fmt, ...);

#ifdef DEBUG
#define DBG(x...) printk(KERN_DEBUG x)
#define PRINTK(x...) printk(x)
#else
#define DBG(x...)
#define PRINTK(x...)
#endif

#endif
