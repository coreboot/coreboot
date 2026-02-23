/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <drivers/i2c/designware/dw_i2c.h>
#include <soc/pci_devs.h>

/*
 * SoC-specific I2C device function to bus number mapping.
 *
 * Each SoC must define:
 * - CONFIG_SOC_INTEL_I2C_DEV_MAX: Maximum number of I2C controllers
 * - SOC_I2C_DEVFN(n): Macro that expands to the SoC-specific I2C device
 *   function name (e.g., PCH_DEVFN_I2Cn or PCI_DEVFN_I2Cn)
 */

_Static_assert(CONFIG_SOC_INTEL_I2C_DEV_MAX <= 8,
	       "CONFIG_SOC_INTEL_I2C_DEV_MAX greater than 8 is not supported.");

int dw_i2c_soc_devfn_to_bus(unsigned int devfn)
{
	switch (devfn) {
	case SOC_I2C_DEVFN(0):
		return 0;
	case SOC_I2C_DEVFN(1):
		return 1;
	case SOC_I2C_DEVFN(2):
		return 2;
	case SOC_I2C_DEVFN(3):
		return 3;
#if CONFIG_SOC_INTEL_I2C_DEV_MAX > 4
	case SOC_I2C_DEVFN(4):
		return 4;
#endif
#if CONFIG_SOC_INTEL_I2C_DEV_MAX > 5
	case SOC_I2C_DEVFN(5):
		return 5;
#endif
#if CONFIG_SOC_INTEL_I2C_DEV_MAX > 6
	case SOC_I2C_DEVFN(6):
		return 6;
#endif
#if CONFIG_SOC_INTEL_I2C_DEV_MAX > 7
	case SOC_I2C_DEVFN(7):
		return 7;
#endif
	}
	return -1;
}

int dw_i2c_soc_bus_to_devfn(unsigned int bus)
{
	switch (bus) {
	case 0:
		return SOC_I2C_DEVFN(0);
	case 1:
		return SOC_I2C_DEVFN(1);
	case 2:
		return SOC_I2C_DEVFN(2);
	case 3:
		return SOC_I2C_DEVFN(3);
#if CONFIG_SOC_INTEL_I2C_DEV_MAX > 4
	case 4:
		return SOC_I2C_DEVFN(4);
#endif
#if CONFIG_SOC_INTEL_I2C_DEV_MAX > 5
	case 5:
		return SOC_I2C_DEVFN(5);
#endif
#if CONFIG_SOC_INTEL_I2C_DEV_MAX > 6
	case 6:
		return SOC_I2C_DEVFN(6);
#endif
#if CONFIG_SOC_INTEL_I2C_DEV_MAX > 7
	case 7:
		return SOC_I2C_DEVFN(7);
#endif
	}
	return -1;
}
