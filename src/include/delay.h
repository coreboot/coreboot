#ifndef DELAY_H
#define DELAY_H

#if CONFIG_HAVE_INIT_TIMER
void init_timer(void);
#else
#define init_timer() do{} while(0)
#endif

void udelay(unsigned usecs);
void mdelay(unsigned msecs);
void delay(unsigned secs);
#endif /* DELAY_H */
