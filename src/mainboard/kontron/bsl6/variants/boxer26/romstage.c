/* SPDX-License-Identifier: GPL-2.0-only */

#include <option.h>
#include <stdint.h>
#include <console/console.h>
#include <pc80/mc146818rtc.h>
#include <fsp/soc_binding.h>
#include <variant.h>

void variant_memory_init_params(FSPM_UPD *const mupd)
{
	char nvram_var[] = "ethernetx";
	unsigned int i;

	for (i = 0; i < 3; ++i) {
		u8 eth_enable = 1;
		nvram_var[sizeof(nvram_var) - 2] = '1' + i;
		get_option(&eth_enable, nvram_var);
		if (!eth_enable) {
			printk(BIOS_INFO, "Disabling ethernet%u.\n", 1 + i);
			mupd->FspmConfig.PcieRpEnableMask &= ~(1 << (i + 8));
		}
	}
}
