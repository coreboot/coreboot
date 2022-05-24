/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>

#define CS35L53_MAX_GPIOS 2

enum cs35l53_boost_type {
	INTERNAL_BOOST = 0,
	EXTERNAL_BOOST = 1,
};

enum cs35l53_boost_ind_nanohenrys {
	BOOST_IND_1000_NH = 1000,
	BOOST_IND_1200_NH = 1200,
	BOOST_IND_1500_NH = 1500,
	BOOST_IND_2200_NH = 2200,
};

enum cs35l53_asp_sdout_hiz {
	ASP_SDOUT_LOGIC0_UNUSED_LOGIC0_DISABLED = 0,
	ASP_SDOUT_HIZ_UNUSED_LOGIC0_DISABLED = 1,
	ASP_SDOUT_LOGIC0_UNUSED_HIZ_DISABLED = 2,
	ASP_SDOUT_HIZ_UNUSED_HIZ_DISABLED = 3,
};

enum cs35l53_gpio1_src {
	GPIO1_SRC_HIGH_IMPEDANCE = 0,
	GPIO1_SRC_GPIO = 1,
	GPIO1_SRC_SYNC = 2,
	GPIO1_SRC_MCLK_INPUT = 3,
};

enum cs35l53_gpio2_src {
	GPIO2_SRC_HIGH_IMPEDANCE = 0,
	GPIO2_SRC_GPIO = 1,
	GPIO2_SRC_OPEN_DRAIN = 2,
	GPIO2_SRC_MCLK_INPUT = 3,
	GPIO2_SRC_PUSH_PULL_INTB = 4,
	GPIO2_SRC_PUSH_PULL_INT = 5,
};

/*
 * Cirrus Logic  CS35L53 Audio Codec devicetree bindings
 * linux/Documentation/devicetree/bindings/sound/cirrus,cs35l53.yaml
 */
struct drivers_i2c_cs35l53_config {
	const char *name; /* ACPI Device Name */

	const char *sub; /* SUB ID to uniquely identify system */

	/* Device Description */
	const char *desc;

	/* Identifier for chips */
	uint32_t uid;

	/* Interrupt configuration */
	struct acpi_irq irq;

	/* Use GPIO based interrupt instead of PIRQ */
	struct acpi_gpio irq_gpio;

	/* Use GPIO based reset gpio */
	struct acpi_gpio reset_gpio;

	/* I2C Bus Frequency in Hertz (default 400kHz) */
	unsigned int bus_speed;

	/* Define cs35l53 parameters */
	/*
	 * cirrus,boost-type : Configures the type of Boost being used.
	 * Internal boost requires boost-peak-milliamp, boost-ind-nanohenry and
	 * boost-cap-microfarad.
	 * External Boost must have GPIO1 as GPIO output. GPIO1 will be set high to
	 * enable boost voltage.
	 */
	enum cs35l53_boost_type boost_type;

	/*
	 * cirrus,boost-peak-milliamp : Boost-converter peak current limit in mA.
	 * Configures the peak current by monitoring the current through the boost FET.
	 * Range starts at 1600 mA and goes to a maximum of 4500 mA with increments
	 * of 50 mA. See section 4.3.6 of the datasheet for details.
	 */
	unsigned int boost_peak_milliamp;

	/*
	 * cirrus,boost-ind-nanohenry : Boost inductor value, expressed in nH. Valid
	 * values include 1000, 1200, 1500 and 2200.
	 */
	enum cs35l53_boost_ind_nanohenrys boost_ind_nanohenry;

	/*
	 * cirrus,boost-cap-microfarad : Total equivalent boost capacitance on the VBST
	 * and VAMP pins, derated at 11 volts DC. The value must be rounded to the
	 * nearest integer and expressed in uF.
	 */
	unsigned int boost_cap_microfarad;

	/*
	 * cirrus,asp-sdout-hiz : Audio serial port SDOUT Hi-Z control. Sets the Hi-Z
	 * configuration for SDOUT pin of amplifier.
	 * 0 = Logic 0 during unused slots, and while all transmit channels disabled
	 * 1 = Hi-Z during unused slots but logic 0 while all transmit channels disabled
	 * 2 = Logic 0 during unused slots, but Hi-Z while all transmit channels disabled
	 * 3 = Hi-Z during unused slots and while all transmit channels disabled
	 */
	enum cs35l53_asp_sdout_hiz asp_sdout_hiz;

	/*
	 * cirrus,gpio1-polarity-invert : Boolean which specifies whether the GPIO1
	 * level is inverted.
	 */
	bool gpio1_polarity_invert;

	/*
	 * cirrus,gpio2-polarity-invert : Boolean which specifies whether the GPIO2
	 * level is inverted.
	 */
	bool gpio2_polarity_invert;

	/*
	 * cirrus,gpio1-output-enable : Boolean which specifies whether the GPIO1 pin
	 * is configured as an output.
	 */
	bool gpio1_output_enable;

	/*
	 * cirrus,gpio2-output-enable : Boolean which specifies whether the GPIO2 pin
	 * is configured as an output.
	 */
	bool gpio2_output_enable;

	/*
	 * cirrus,gpio1-src-select : Configures the function of the GPIO1 pin.
	 * GPIO1:
	 * 0 = High Impedance (Default)
	 * 1 = GPIO
	 * 2 = Sync
	 * 3 = MCLK input
	 */
	enum cs35l53_gpio1_src gpio1_src_select;

	/*
	 * cirrus,gpio2-src-select : Configures the function of the GPIO2 pin.
	 * GPIO2:
	 * 0 = High Impedance (Default)
	 * 1 = GPIO
	 * 2 = Open Drain INTB
	 * 3 = MCLK input
	 * 4 = Push-pull INTB (active low)
	 * 5 = Push-pull INT (active high)
	 */
	enum cs35l53_gpio2_src gpio2_src_select;
};
