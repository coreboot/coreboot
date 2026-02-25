/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <intelblocks/gspi.h>
#include <soc/pci_devs.h>

/*
 * Ensure the platform defines SOC_GSPI_DEVFN(n) macro to map GSPI bus numbers
 * to their PCI device/function values. The macro should be defined in the
 * platform's soc/pci_devs.h header.
 */

_Static_assert(CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX <= 7,
	       "CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX greater than 7 is not supported.");

int gspi_soc_bus_to_devfn(unsigned int gspi_bus)
{
	switch (gspi_bus) {
	case 0:
		return SOC_GSPI_DEVFN(0);
	case 1:
		return SOC_GSPI_DEVFN(1);
#if CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX > 2
	case 2:
		return SOC_GSPI_DEVFN(2);
#endif
#if CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX > 3
	case 3:
		return SOC_GSPI_DEVFN(3);
#endif
#if CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX > 4
	case 4:
		return SOC_GSPI_DEVFN(4);
#endif
#if CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX > 5
	case 5:
		return SOC_GSPI_DEVFN(5);
#endif
#if CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX > 6
	case 6:
		return SOC_GSPI_DEVFN(6);
#endif
	}
	return -1;
}
