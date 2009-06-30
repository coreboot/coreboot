#ifndef PART_WATCHDOG_H
#define PART_WATCHDOG_H

#if CONFIG_USE_WATCHDOG_ON_BOOT == 1
void watchdog_off(void);
#else
#define watchdog_off()
#endif

#endif /* PART_WATCHDOG_H */
