/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * This file is created based on Intel Alder Lake Processor PCH Datasheet
 * Document number: 621483
 * Chapter number: 7
 */

#include <intelblocks/fast_spi.h>
#include <intelblocks/spi.h>
#include <soc/pci_devs.h>

#define PSF_SPI_DESTINATION_ID	0x23a8

int spi_soc_devfn_to_bus(unsigned int devfn)
{
	switch (devfn) {
	case PCH_DEVFN_GSPI0:
		return 1;
	case PCH_DEVFN_GSPI1:
		return 2;
	case PCH_DEVFN_GSPI2:
		return 3;
	case PCH_DEVFN_GSPI3:
		return 4;
	}
	return -1;
}

uint32_t soc_get_spi_psf_destination_id(void)
{
	return PSF_SPI_DESTINATION_ID;
}
