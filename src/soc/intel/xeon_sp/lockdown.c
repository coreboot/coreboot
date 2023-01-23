/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/lpc_lib.h>
#include <intelpch/lockdown.h>
#include <soc/lockdown.h>
#include <soc/pm.h>

static void lpc_lockdown_config(void)
{
	/* Set BIOS Interface Lock, BIOS Lock */
	lpc_set_bios_interface_lock_down();

	/* Only allow writes in SMM */
	if (CONFIG(BOOTMEDIA_SMM_BWP)) {
		lpc_set_eiss();
		lpc_enable_wp();
	}
	lpc_set_lock_enable();
}

void soc_lockdown_config(int chipset_lockdown)
{
	lpc_lockdown_config();
	pmc_lockdown_config();
	sata_lockdown_config(chipset_lockdown);
	spi_lockdown_config(chipset_lockdown);
}
