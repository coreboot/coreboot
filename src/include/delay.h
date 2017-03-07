#ifndef DELAY_H
#define DELAY_H

void init_timer(void);

void udelay(unsigned int usecs);
void mdelay(unsigned int msecs);
void delay(unsigned int secs);
#endif /* DELAY_H */
