/* SPDX-License-Identifier: GPL-2.0-only */

#include <delay.h>
void mdelay(unsigned int msecs)
{
	unsigned int i;
	for (i = 0; i < msecs; i++)
		udelay(1000);
}
void delay(unsigned int secs)
{
	unsigned int i;
	for (i = 0; i < secs; i++)
		mdelay(1000);
}
