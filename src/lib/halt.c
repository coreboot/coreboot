/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/hlt.h>
#include <halt.h>

void halt(void)
{
	while (1)
		hlt();
}
