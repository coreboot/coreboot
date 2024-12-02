/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/cfg.h>
#include <intelblocks/fast_spi.h>
#include <intelpch/lockdown.h>
#include <soc/lockdown.h>

void sata_lockdown_config(int chipset_lockdown)
{
}

void spi_lockdown_config(int chipset_lockdown)
{
	if (chipset_lockdown == CHIPSET_LOCKDOWN_COREBOOT) {
		fast_spi_set_bde();
	}
}
