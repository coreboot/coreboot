/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_I2C_SX9324_CHIP_H__
#define __DRIVERS_I2C_SX9324_CHIP_H__

#include <acpi/acpi_device.h>
#include <device/i2c_simple.h>

#define REGISTER(NAME) uint8_t NAME

struct drivers_i2c_sx9324_config {
	/* Device Description */
	const char *desc;

	/* ACPI _UID */
	unsigned int uid;

	/* Bus speed in Hz, default is I2C_SPEED_FAST */
	enum i2c_speed speed;

	/* Use GPIO-based interrupt instead of IO-APIC */
	struct acpi_gpio irq_gpio;

	/* IO-APIC interrupt */
	struct acpi_irq irq;

	/*
	semtech,ph0-pin:
	  description: |
	    Array of 3 entries. Index represent the id of the CS pin.
	    Value indicates how each CS pin is used during phase 0.
	    Each of the 3 pins have the following value -
	    0 : unused (high impedance)
	    1 : measured input
	    2 : dynamic shield
	    3 : grounded.
	    For instance, CS0 measured, CS1 shield and CS2 ground is [1, 2, 3]
	  items:
	    enum: [ 0, 1, 2, 3 ]
	  minItems: 3
	  maxItems: 3
	*/
	uint64_t ph0_pin[3];

	/*
	semtech,ph1-pin:
	  Same as ph0-pin for phase 1.
	*/
	uint64_t ph1_pin[3];

	/*
	semtech,ph2-pin:
	  Same as ph0-pin for phase 2.
	*/
	uint64_t ph2_pin[3];

	/*
	semtech,ph3-pin:
	  Same as ph0-pin for phase 3.
	*/
	uint64_t ph3_pin[3];

	/*
	semtech,ph01-resolution:
	  enum: [8, 16, 32, 64, 128, 256, 512, 1024]
	  description:
	    Capacitance measurement resolution. For phase 0 and 1.
	    Higher the number, higher the resolution.
	  default: 128
	*/
	uint64_t ph01_resolution;

	/*
	semtech,ph23-resolution:
	  Same as ph01-resolution for phase 2 and 3.
	*/
	uint64_t ph23_resolution;

	/*
	semtech,startup-sensor:
	  enum: [0, 1, 2, 3]
	  default: 0
	  description: |
	    Phase used for start-up proximity detection.
	    It is used when we enable a phase to remove static offset and measure
	    only capacitance changes introduced by the user.
	*/
	uint64_t startup_sensor;

	/*
	semtech,ph01-proxraw-strength:
	  minimum: 0
	  maximum: 7
	  default: 1
	  description:
	    PROXRAW filter strength for phase 0 and 1. A value of 0 represents off,
	    and other values represent 1-1/2^N.
	*/
	uint64_t ph01_proxraw_strength;

	/*
	semtech,ph23-proxraw-strength:
	  Same as proxraw-strength01, for phase 2 and 3.
	*/
	uint64_t ph23_proxraw_strength;

	/*
	semtech,avg-pos-strength:
	  enum: [0, 16, 64, 128, 256, 512, 1024, 4294967295]
	  default: 16
	  description: |
	    Average positive filter strength. A value of 0 represents off and
	    UINT_MAX (4294967295) represents infinite. Other values
	    represent 1-1/N.
	*/
	uint64_t avg_pos_strength;

	/*
	semtech,cs-idle-sleep:
	  description:
	    State of CS pins during sleep mode and idle time.
	  enum:
	    - hi-z
	    - gnd
	    - vdd
	*/
	const char *cs_idle_sleep;

	/*
	semtech,int-comp-resistor:
	  description:
	    Internal resistor setting for compensation.
	  enum:
	    - lowest
	    - low
	    - high
	    - highest
	*/
	const char *int_comp_resistor;

	/*
	semtech,input-precharge-resistor-ohms:
	  default: 4000
	  multipleOf: 2000
	  minimum: 0
	  maximum: 30000
	  description:
	    Pre-charge input resistance in Ohm.
	*/
	uint64_t input_precharge_resistor_ohms;

	/*
	semtech,input-analog-gain:
	  minimum: 0
	  maximum: 3
	  description: |
	    Defines the input antenna analog gain
	    0: x1.247
	    1: x1 (default)
	    2: x0.768
	    3: x0.552
	*/
	uint64_t input_analog_gain;

#if CONFIG(DRIVERS_I2C_SX9324_SUPPORT_LEGACY_LINUX_DRIVER)
#include "registers.h"
#endif
};

#undef REGISTER

#endif /* __DRIVERS_I2C_SX9324_CHIP_H__ */
