#ifndef DELAY_H
#define DELAY_H

#if IS_ENABLED(CONFIG_GENERIC_UDELAY)
static inline void init_timer(void) {}
#else
void init_timer(void);
#endif

void udelay(unsigned usecs);
void mdelay(unsigned msecs);
void delay(unsigned secs);
#endif /* DELAY_H */
