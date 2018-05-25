/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015-2016 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <assert.h>
#include <console/console.h>
#include <device/device.h>
#include <romstage_handoff.h>
#include <soc/ramstage.h>
#include <soc/reg_access.h>

/* Cat Trip Clear value must be less than Cat Trip Set value */
#define PLATFORM_CATASTROPHIC_TRIP_CELSIUS	105
#define PLATFORM_CATASTROPHIC_CLEAR_CELSIUS	65

static const struct reg_script thermal_init_script[] = {

	/* Setup RMU Thermal sensor registers for Ratiometric mode. */
	REG_SOC_UNIT_RMW(QUARK_SCSS_SOC_UNIT_TSCGF1_CONFIG,
		~(B_TSCGF1_CONFIG_ISNSCURRENTSEL_MASK
			| B_TSCGF1_CONFIG_ISNSCHOPSEL_MASK
			| B_TSCGF1_CONFIG_ISNSINTERNALVREFEN
			| B_TSCGF1_CONFIG_IBGEN
			| B_TSCGF1_CONFIG_IBGCHOPEN),
		((V_TSCGF1_CONFIG_ISNSCURRENTSEL_RATIO_MODE
			<< B_TSCGF1_CONFIG_ISNSCURRENTSEL_BP)
		| (V_TSCGF1_CONFIG_ISNSCHOPSEL_RATIO_MODE
			<< B_TSCGF1_CONFIG_ISNSCHOPSEL_BP)
		| (V_TSCGF1_CONFIG_ISNSINTERNALVREFEN_RATIO_MODE
			<< B_TSCGF1_CONFIG_ISNSINTERNALVREFEN_BP)
		| (V_TSCGF1_CONFIG_IBGEN_RATIO_MODE
			<< B_TSCGF1_CONFIG_IBGEN_BP)
		| (V_TSCGF1_CONFIG_IBGCHOPEN_RATIO_MODE
			<< B_TSCGF1_CONFIG_IBGCHOPEN_BP))),

	REG_SOC_UNIT_RMW(QUARK_SCSS_SOC_UNIT_TSCGF2_CONFIG2,
		~(B_TSCGF2_CONFIG2_ICALCONFIGSEL_MASK
			| B_TSCGF2_CONFIG2_ISPARECTRL_MASK
			| B_TSCGF2_CONFIG2_ICALCOARSETUNE_MASK),
		((V_TSCGF2_CONFIG2_ICALCONFIGSEL_RATIO_MODE
			<< B_TSCGF2_CONFIG2_ICALCONFIGSEL_BP)
		| (V_TSCGF2_CONFIG2_ISPARECTRL_RATIO_MODE
			<< B_TSCGF2_CONFIG2_ISPARECTRL_BP)
		| (V_TSCGF2_CONFIG2_ICALCOARSETUNE_RATIO_MODE
			<< B_TSCGF2_CONFIG2_ICALCOARSETUNE_BP))),

	REG_SOC_UNIT_RMW(QUARK_SCSS_SOC_UNIT_TSCGF2_CONFIG,
		~(B_TSCGF2_CONFIG_IDSCONTROL_MASK
			| B_TSCGF2_CONFIG_IDSTIMING_MASK),
		((V_TSCGF2_CONFIG_IDSCONTROL_RATIO_MODE
			<< B_TSCGF2_CONFIG_IDSCONTROL_BP)
		| (V_TSCGF2_CONFIG_IDSTIMING_RATIO_MODE
			<< B_TSCGF2_CONFIG_IDSTIMING_BP))),

	REG_SOC_UNIT_RMW(QUARK_SCSS_SOC_UNIT_TSCGF3_CONFIG,
		~B_TSCGF3_CONFIG_ITSGAMMACOEFF_MASK,
		V_TSCGF3_CONFIG_ITSGAMMACOEFF_RATIO_MODE
			<< B_TSCGF3_CONFIG_ITSGAMMACOEFF_BP),

	/* Enable RMU Thermal sensor with a Catastrophic Trip point. */

	/* Set up Catastrophic Trip point.
	*
	* Trip Register fields are 8-bit temperature values of granularity 1
	* degree C where 0x00 corresponds to -50 degrees C and 0xFF corresponds
	* to 205 degrees C.
	*
	* Add 50 to Celsius values to get values for register fields.
	*/
	REG_RMU_TEMP_RMW(QUARK_NC_RMU_REG_TS_TRIP,
		~(TS_CAT_TRIP_SET_THOLD_MASK | TS_CAT_TRIP_CLEAR_THOLD_MASK),
		(((PLATFORM_CATASTROPHIC_TRIP_CELSIUS + 50)
			<< TS_CAT_TRIP_SET_THOLD_BP)
		| ((PLATFORM_CATASTROPHIC_CLEAR_CELSIUS + 50)
			<< TS_CAT_TRIP_CLEAR_THOLD_BP))),

	/* To enable the TS do the following:
	 *    1)  Take the TS out of reset by setting itsrst to 0x0.
	 *    2)  Enable the TS using RMU Thermal sensor mode register.
	 */
	REG_SOC_UNIT_AND(QUARK_SCSS_SOC_UNIT_TSCGF3_CONFIG,
		~B_TSCGF3_CONFIG_ITSRST),
	REG_RMU_TEMP_OR(QUARK_NC_RMU_REG_TS_MODE, TS_ENABLE),

	/* Lock all RMU Thermal sensor control & trip point registers. */
	REG_RMU_TEMP_OR(QUARK_NC_RMU_REG_CONFIG, TS_LOCK_THRM_CTRL_REGS_ENABLE
		| TS_LOCK_AUX_TRIP_PT_REGS_ENABLE),
	REG_SCRIPT_END
};

static void chip_init(void *chip_info)
{
	/* Validate the temperature settings */
	ASSERT(PLATFORM_CATASTROPHIC_TRIP_CELSIUS <= 255);
	ASSERT(PLATFORM_CATASTROPHIC_TRIP_CELSIUS
		> PLATFORM_CATASTROPHIC_CLEAR_CELSIUS);

	/* Set the temperature settings */
	reg_script_run(thermal_init_script);

	/* Verify that the thermal configuration is locked */
	ASSERT((reg_rmu_temp_read(QUARK_NC_RMU_REG_CONFIG)
		& (TS_LOCK_THRM_CTRL_REGS_ENABLE
			| TS_LOCK_AUX_TRIP_PT_REGS_ENABLE))
		== (TS_LOCK_THRM_CTRL_REGS_ENABLE
			| TS_LOCK_AUX_TRIP_PT_REGS_ENABLE));

	/* Perform silicon specific init. */
	fsp_silicon_init(romstage_handoff_is_resume());
}

static void pci_domain_set_resources(struct device *dev)
{
	assign_resources(dev->link_list);
}

static struct device_operations pci_domain_ops = {
	.read_resources	= pci_domain_read_resources,
	.set_resources	= pci_domain_set_resources,
	.scan_bus	= pci_domain_scan_bus,
};

static void chip_enable_dev(struct device *dev)
{

	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		dev->ops = &pci_domain_ops;
}

struct chip_operations soc_intel_quark_ops = {
	CHIP_NAME("Intel Quark")
	.init		= &chip_init,
	.enable_dev	= chip_enable_dev,
};
