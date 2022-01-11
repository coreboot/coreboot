/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <intelblocks/spi.h>
#include <soc/pci_devs.h>

int spi_soc_devfn_to_bus(unsigned int devfn)
{
	/* Denverton doesn't have GSPI controllers, only Fast SPI */

	if (devfn == PCH_DEVFN_SPI)
		return 0;
	else
		return -1;
}
