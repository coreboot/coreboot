/* SPDX-License-Identifier: GPL-2.0-only */

#include <option.h>
#include <console/console.h>
#include <fsp/soc_binding.h>
#include <variant.h>

void variant_romstage_params(FSPM_UPD *const mupd)
{
	uint8_t eth_enable = 1;

	get_option(&eth_enable, "ethernet1");
	if (!eth_enable) {
		printk(BIOS_DEBUG, "Disabling ethernet1.\n");
		mupd->FspmConfig.PcieRpEnableMask &= ~(1 << 5);
	}
}
