#ifndef DELAY_H
#define DELAY_H
#ifndef __ROMCC__

void init_timer(void);
void udelay(unsigned usecs);
void mdelay(unsigned msecs);
void delay(unsigned secs);

#endif
#endif /* DELAY_H */
