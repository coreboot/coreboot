#ifndef DELAY_H
#define DELAY_H

#if !defined( __ROMCC__)

#if CONFIG_HAVE_INIT_TIMER == 1
void init_timer(void);
#else
#define init_timer() do{} while(0)
#endif

void udelay(unsigned usecs);
void mdelay(unsigned msecs);
void delay(unsigned secs);

#endif
#endif /* DELAY_H */
