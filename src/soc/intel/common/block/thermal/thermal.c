/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <intelblocks/cfg.h>
#include <intelblocks/thermal.h>
#include <soc/pci_devs.h>

#define THERMAL_SENSOR_POWER_MANAGEMENT 0x1c
#define CATASTROPHIC_TRIP_POINT_MASK 0x1ff
#define MAX_TRIP_TEMP 205
/* This is the safest default Trip Temp value */
#define DEFAULT_TRIP_TEMP 50
#define GET_LTT_VALUE(x) (((x) + 50) * (2))

static uint8_t get_thermal_trip_temp(void)
{
	const struct soc_intel_common_config *common_config;
	common_config = chip_get_common_soc_structure();

	return common_config->pch_thermal_trip;
}

/* PCH Low Temp Threshold (LTT) */
static uint16_t pch_get_ltt_value(struct device *dev)
{
	uint16_t ltt_value;
	uint8_t thermal_config;

	thermal_config = get_thermal_trip_temp();
	if (!thermal_config)
		thermal_config = DEFAULT_TRIP_TEMP;

	if (thermal_config > MAX_TRIP_TEMP)
		die("Input PCH temp trip is higher than allowed range!");

	/* Trip Point Temp = (LTT / 2 - 50 degree C) */
	ltt_value = GET_LTT_VALUE(thermal_config);

	return ltt_value;
}

/* Enable thermal sensor power management */
void pch_thermal_configuration(void)
{
	uint16_t reg16;
	uintptr_t thermalbar;
	uintptr_t thermalbar_pm;
	struct device *dev;
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
	reg16 = read16((uint16_t *)thermalbar_pm);
	reg16 &= ~CATASTROPHIC_TRIP_POINT_MASK;
	/* Low Temp Threshold (LTT) */
	reg16 |= pch_get_ltt_value(dev);
	write16((uint16_t *)thermalbar_pm, reg16);
}
