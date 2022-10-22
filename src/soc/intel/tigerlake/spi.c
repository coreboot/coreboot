/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * This file is created based on Intel Tiger Lake Processor PCH Datasheet
 * Document number: 575857
 * Chapter number: 7
 */

#include <intelblocks/fast_spi.h>
#include <intelblocks/spi.h>
#include <soc/pci_devs.h>
#include <stdint.h>

#define PSF_SPI_DESTINATION_ID_H	0x23b0
#define PSF_SPI_DESTINATION_ID		0x23a8

int spi_soc_devfn_to_bus(unsigned int devfn)
{
	switch (devfn) {
	case PCH_DEVFN_GSPI0:
		return 1;
	case PCH_DEVFN_GSPI1:
		return 2;
	case PCH_DEVFN_GSPI2:
		return 3;
	}
	return -1;
}

uint32_t soc_get_spi_psf_destination_id(void)
{
	if (CONFIG(SOC_INTEL_TIGERLAKE_PCH_H))
		return PSF_SPI_DESTINATION_ID_H;
	return PSF_SPI_DESTINATION_ID;
}
