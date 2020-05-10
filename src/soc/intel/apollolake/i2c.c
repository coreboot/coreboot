/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci_def.h>
#include <drivers/i2c/designware/dw_i2c.h>
#include <soc/pci_devs.h>

/* Convert I2C bus number to PCI device and function */
int dw_i2c_soc_bus_to_devfn(unsigned int bus)
{
	if (bus <= 3)
		return PCI_DEVFN(PCH_DEV_SLOT_SIO1, bus);
	else if (bus >= 4 && bus <= 7)
		return PCI_DEVFN(PCH_DEV_SLOT_SIO2, (bus - 4));
	else
		return -1;
}

/* Convert PCI device and function to I2C bus number */
int dw_i2c_soc_devfn_to_bus(unsigned int devfn)
{
	if (PCI_SLOT(devfn) == PCH_DEV_SLOT_SIO1)
		return PCI_FUNC(devfn);
	else if (PCI_SLOT(devfn) == PCH_DEV_SLOT_SIO2)
		return PCI_FUNC(devfn) + 4;
	else
		return -1;
}
