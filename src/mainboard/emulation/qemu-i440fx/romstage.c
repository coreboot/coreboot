/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <cbmem.h>

void mainboard_romstage_entry(void)
{
	cbmem_recovery(false);
}
