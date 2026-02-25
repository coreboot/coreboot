/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <intelblocks/fast_spi.h>
#include <intelblocks/spi.h>
#include <soc/pci_devs.h>
#include <stdint.h>

/*
 * Ensure the platform defines SOC_GSPI_DEVFN(n) macro to map GSPI bus numbers
 * to their PCI device/function values. The macro should be defined in the
 * platform's soc/pci_devs.h header.
 */

_Static_assert(CONFIG_SOC_INTEL_SPI_DEV_MAX <= 5,
	       "CONFIG_SOC_INTEL_SPI_DEV_MAX greater than 5 is not supported.");

int spi_soc_devfn_to_bus(unsigned int devfn)
{
	switch (devfn) {
#if CONFIG_SOC_INTEL_SPI_DEV_MAX > 0
	case SOC_GSPI_DEVFN(0):
		return 1;
#endif
#if CONFIG_SOC_INTEL_SPI_DEV_MAX > 1
	case SOC_GSPI_DEVFN(1):
		return 2;
#endif
#if CONFIG_SOC_INTEL_SPI_DEV_MAX > 2
	case SOC_GSPI_DEVFN(2):
		return 3;
#endif
#if CONFIG_SOC_INTEL_SPI_DEV_MAX > 3
	case SOC_GSPI_DEVFN(3):
		return 4;
#endif
#if CONFIG_SOC_INTEL_SPI_DEV_MAX > 4
	case SOC_GSPI_DEVFN(4):
		return 5;
#endif
	}
	return -1;
}

#if CONFIG(SOC_INTEL_COMMON_FEATURE_SPI_DEVFN_PSF)
uint32_t soc_get_spi_psf_destination_id(void)
{
	return CONFIG_SOC_INTEL_SPI_PSF_DESTINATION_ID;
}
#endif
