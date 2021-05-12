/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_ACPI_THERMAL_ZONE_H__
#define __DRIVERS_ACPI_THERMAL_ZONE_H__

#include <types.h>

/*
 * All temperature units are in Celsius.
 * All time units are in seconds.
 */
struct drivers_acpi_thermal_zone_config {
	/* Description of the thermal zone */
	const char *description;

	/*
	 * Device that will provide the temperature reading
	 *
	 * This device must have an ACPI method named `TMP` that accepts the
	 * sensor ID as the first argument. It must then return an Integer containing the
	 * sensor's temperature in deci-Kelvin.
	 */
	DEVTREE_CONST struct device *temperature_controller;

	/* Used to identify the temperature sensor */
	unsigned int sensor_id;

	/* The polling period in seconds for this thermal zone. */
	unsigned int polling_period;

	/* The temperature (_CRT) at which the OS must shutdown the system. */
	unsigned int critical_temperature;

	/* The temperature (_HOT) at which the OS may choose to hibernate the system */
	unsigned int hibernate_temperature;

	struct acpi_thermal_zone_passive_config {
		/*
		 * The temperature (_PSV) at which the OS must activate passive cooling (i.e.,
		 * throttle the CPUs).
		 */
		unsigned int temperature;

		/**
		 * DeltaP[%] = _TC1 * (Tn - Tn-1) + _TC2 * (Tn - Tt)
		 * Where:
		 * Tn = current temperature
		 * Tt = target temperature (_PSV)
		 *
		 * If any of these values are 0, then one of the following defaults will be
		 * used: TC1: 2, TC2: 5, TSP: 10
		 */
		unsigned int time_constant_1;
		unsigned int time_constant_2;
		unsigned int time_sampling_period;

	} passive_config;

	/* Place the ThermalZone in the \_TZ scope */
	bool use_acpi1_thermal_zone_scope;
};

#endif /* __DRIVERS_ACPI_THERMAL_ZONE_H__ */
