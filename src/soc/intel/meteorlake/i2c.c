/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <drivers/i2c/designware/dw_i2c.h>
#include <soc/pci_devs.h>

int dw_i2c_soc_devfn_to_bus(unsigned int devfn)
{
	switch (devfn) {
	case PCI_DEVFN_I2C0:
		return 0;
	case PCI_DEVFN_I2C1:
		return 1;
	case PCI_DEVFN_I2C2:
		return 2;
	case PCI_DEVFN_I2C3:
		return 3;
	case PCI_DEVFN_I2C4:
		return 4;
	case PCI_DEVFN_I2C5:
		return 5;
	}
	return -1;
}

int dw_i2c_soc_bus_to_devfn(unsigned int bus)
{
	switch (bus) {
	case 0:
		return PCI_DEVFN_I2C0;
	case 1:
		return PCI_DEVFN_I2C1;
	case 2:
		return PCI_DEVFN_I2C2;
	case 3:
		return PCI_DEVFN_I2C3;
	case 4:
		return PCI_DEVFN_I2C4;
	case 5:
		return PCI_DEVFN_I2C5;
	}
	return -1;
}
