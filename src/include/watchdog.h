#ifndef WATCHDOG_H
#define WATCHDOG_H

#if CONFIG_USE_WATCHDOG_ON_BOOT
void watchdog_off(void);
#else
#define watchdog_off() while(0) {}
#endif

#endif /* WATCHDOG_H */
