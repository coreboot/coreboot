/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <option.h>

#include "early_init.h"
#include "i2ec.h"

#define XRAM_BOOTEFFECT_DISABLE 0x47a
#define XRAM_BOOTEFFECT_SUPPORT 0x1e9

void ec_configure_kbled_booteffect(void)
{
	if (!ec_d2i2ec_read(XRAM_BOOTEFFECT_SUPPORT)) {
		printk(BIOS_INFO, "EC: boot effect override not supported by ec firmware\n");
		return;
	}

	bool enable = get_uint_option("kbled_booteffect",
				      CONFIG(EC_CLEVO_IT5570E_KBLED_BOOTEFFECT));

	printk(BIOS_DEBUG, "EC: set booteffect enable=%i\n", enable);
	ec_d2i2ec_write(XRAM_BOOTEFFECT_DISABLE, !enable);
}
