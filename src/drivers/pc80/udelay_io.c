#include <arch/io.h>
#include <delay.h>

void init_timer(void)
{
}

void udelay(unsigned usecs)
{
	int i;
	for(i = 0; i < usecs; i++)
		inb(0x80);
}
