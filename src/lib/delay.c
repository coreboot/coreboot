#include <delay.h>
void mdelay(int msecs)
{
	int i;
	for(i = 0; i < msecs; i++) {
		udelay(1000);
	}
}
void delay(int secs)
{
	int i;
	for(i = 0; i < secs; i++) {
		mdelay(1000);
	}
}
