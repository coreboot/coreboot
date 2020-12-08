/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <amdblocks/smbus.h>
#include <console/console.h>
#include <soc/southbridge.h>

/* Before console init */
void fch_pre_init(void)
{
	enable_acpimmio_decode_pm04();
	fch_smbus_init();
	fch_enable_cf9_io();
	fch_enable_legacy_io();
}

/* After console init */
void fch_early_init(void)
{
	fch_print_pmxc0_status();
}
