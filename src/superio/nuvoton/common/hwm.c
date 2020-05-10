/* SPDX-License-Identifier: GPL-2.0-or-later */

/* Nuvoton is a Winbond spin-off, so this code is for both */

#include <stdint.h>
#include <superio/hwm5_conf.h>
#include "hwm.h"

#define HWM_BANK_SELECT	0x4e

void nuvoton_hwm_select_bank(const u16 base, const u8 bank)
{
	pnp_write_hwm5_index(base, HWM_BANK_SELECT, bank);
}
