/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <intelblocks/fast_spi.h>
#include <intelblocks/spi.h>
#include <soc/pci_devs.h>

#define PSF_SPI_DESTINATION_ID  0x5140

int spi_soc_devfn_to_bus(unsigned int devfn)
{
	switch (devfn) {
	case PCI_DEVFN_SPI:
		return 0;
	case PCI_DEVFN_GSPI0:
		return 1;
	case PCI_DEVFN_GSPI1:
		return 2;
	case PCI_DEVFN_GSPI2:
		return 3;
	}
	return -1;
}

uint32_t soc_get_spi_psf_destination_id(void)
{
	return PSF_SPI_DESTINATION_ID;
}
