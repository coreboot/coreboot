/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * This file is created based on Intel Tiger Lake Processor PCH Datasheet
 * Document number: 575857
 * Chapter number: 7
 */

#include <intelblocks/spi.h>
#include <intelblocks/fast_spi.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>

int spi_soc_devfn_to_bus(unsigned int devfn)
{
	switch (devfn) {
	case PCH_DEVFN_SPI:
		return 0;
	case PCH_DEVFN_GSPI0:
		return 1;
	case PCH_DEVFN_GSPI1:
		return 2;
	case PCH_DEVFN_GSPI2:
		return 3;
	}
	return -1;
}

uint32_t soc_get_spi_dmi_destination_id(void)
{
	return SPI_DMI_DESTINATION_ID;
}
