/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <intelblocks/gspi.h>
#include <soc/pci_devs.h>

int gspi_soc_bus_to_devfn(unsigned int gspi_bus)
{
	switch (gspi_bus) {
	case 0:
		return PCI_DEVFN_GSPI0;
	case 1:
		return PCI_DEVFN_GSPI1;
	case 2:
		return PCI_DEVFN_GSPI2;
	}
	return -1;
}
