#include <delay.h>
void mdelay(unsigned msecs)
{
	unsigned i;
	for(i = 0; i < msecs; i++) {
		udelay(1000);
	}
}
void delay(unsigned secs)
{
	unsigned i;
	for(i = 0; i < secs; i++) {
		mdelay(1000);
	}
}
