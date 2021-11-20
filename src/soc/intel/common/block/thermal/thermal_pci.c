/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/mmio.h>
#include <intelblocks/thermal.h>
#include <soc/pci_devs.h>

#define THERMAL_SENSOR_POWER_MANAGEMENT 0x1c
#define CATASTROPHIC_TRIP_POINT_MASK 0x1ff

/* Enable thermal sensor power management */
void pch_thermal_configuration(void)
{
	uintptr_t thermalbar;
	uintptr_t thermalbar_pm;
	const struct device *dev;
	struct resource *res;

	dev = pcidev_path_on_root(PCH_DEVFN_THERMAL);
	if (!dev) {
		printk(BIOS_ERR, "ERROR: PCH_DEVFN_THERMAL device not found!\n");
		return;
	}

	res = probe_resource(dev, PCI_BASE_ADDRESS_0);
	if (!res) {
		printk(BIOS_ERR, "ERROR: PCH thermal device not found!\n");
		return;
	}

	/* Get the base address of the resource */
	thermalbar = res->base;

	/* Get the required thermal address to write the register value */
	thermalbar_pm = thermalbar + THERMAL_SENSOR_POWER_MANAGEMENT;

	/* Set Low Temp Threshold (LTT) at TSPM offset 0x1c[8:0] */
	clrsetbits32((void *)thermalbar_pm, CATASTROPHIC_TRIP_POINT_MASK, pch_get_ltt_value());
}
