#include <arch/io.h>

void udelay(int usecs)
{
	int i;
	for(i = 0; i < usecs; i++)
		outb(i&0xff, 0x80);
}

