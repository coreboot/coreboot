#include <arch/io.h>
#include <delay.h>

void udelay(unsigned usecs)
{
	int i;
	for(i = 0; i < usecs; i++)
		inb(0x80);
}

