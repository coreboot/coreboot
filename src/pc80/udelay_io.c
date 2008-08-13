#include <arch/io.h>

void udelay(int usecs)
{
	int i;
	for(i = 0; i < usecs; i++)
		inb(0x80);
}

