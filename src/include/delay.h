#ifndef DELAY_H
#define DELAY_H
#if !defined( __ROMCC__) && defined (__GNUC__)

void udelay(unsigned usecs);
void mdelay(unsigned msecs);
void delay(unsigned secs);

#endif
#endif /* DELAY_H */
