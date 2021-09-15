/* SPDX-License-Identifier: GPL-2.0-only */

#include <unistd.h>

unsigned long virtual_offset = 0;

int getpagesize(void)
{
	return 4096;
}
