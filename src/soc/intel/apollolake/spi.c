/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <intelblocks/spi.h>
#include <soc/pci_devs.h>

int spi_soc_devfn_to_bus(unsigned int devfn)
{
	switch (devfn) {
	case PCH_DEVFN_SPI0:
		return 0;
	case PCH_DEVFN_SPI1:
		return 1;
	case PCH_DEVFN_SPI2:
		return 2;
	}
	return -1;
}
