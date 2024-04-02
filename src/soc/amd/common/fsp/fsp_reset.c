/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/reset.h>
#include <assert.h>
#include <console/console.h>
#include <fsp/util.h>
#include <stdint.h>

void chipset_handle_reset(efi_return_status_t status)
{
	fsp_printk(status, BIOS_ERR, "Doing cold reset due to unexpected call to %s", __func__);
	BUG();
	do_cold_reset();
}
