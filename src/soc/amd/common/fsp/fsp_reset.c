/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/reset.h>
#include <assert.h>
#include <console/console.h>
#include <fsp/util.h>
#include <stdint.h>

void chipset_handle_reset(uint32_t status)
{
	printk(BIOS_ERR, "Error: unexpected call to %s(0x%08x).  Doing cold reset.\n",
			__func__, status);
	BUG();
	do_cold_reset();
}
