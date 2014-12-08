#ifndef DELAY_H
#define DELAY_H

void init_timer(void);

void udelay(unsigned usecs);
void mdelay(unsigned msecs);
void delay(unsigned secs);
#endif /* DELAY_H */
