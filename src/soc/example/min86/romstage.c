/* SPDX-License-Identifier: GPL-2.0-only */

#include <romstage_common.h>
#include <halt.h>

void __noreturn romstage_main(void)
{
	/* Needed for __noreturn */
	halt();
}
