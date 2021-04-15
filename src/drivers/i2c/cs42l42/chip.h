/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>

enum cs42l42_ts_dbnc_rise {
	RISE_DEB_0_MS = 0,
	RISE_DEB_125_MS = 1,
	RISE_DEB_250_MS = 2,
	RISE_DEB_500_MS = 3,
	RISE_DEB_750_MS = 4,
	RISE_DEB_1000_MS = 5,
	RISE_DEB_1250_MS = 6,
	RISE_DEB_1500_MS = 7,
};

enum cs42l42_ts_dbnc_fall {
	FALL_DEB_0_MS = 0,
	FALL_DEB_125_MS = 1,
	FALL_DEB_250_MS = 2,
	FALL_DEB_500_MS = 3,
	FALL_DEB_750_MS = 4,
	FALL_DEB_1000_MS = 5,
	FALL_DEB_1250_MS = 6,
	FALL_DEB_1500_MS = 7,
};

enum cs42l42_hs_bias_ramp_rate {
	HSBIAS_RAMP_FAST_RISE_SLOW_FALL = 0,
	HSBIAS_RAMP_FAST = 1,
	HSBIAS_RAMP_SLOW = 2,
	HSBIAS_RAMP_SLOWEST = 3,
};

/*
 * Cirrus Logic  CS42L42 Audio Codec devicetree bindings
 * linux/Documentation/devicetree/bindings/sound/cs42l42.txt
 */
struct drivers_i2c_cs42l42_config {
	/* Interrupt configuration */
	struct acpi_irq irq;

	/* Use GPIO based interrupt instead of PIRQ */
	struct acpi_gpio irq_gpio;

	/* Use GPIO based reset gpio */
	struct acpi_gpio reset_gpio;

	/* I2C Bus Frequency in Hertz (default 400kHz) */
	unsigned int bus_speed;

	/* Define cs42L42 parameters */
	/*
	 * cirrus,ts-inv : Boolean property. For jacks that invert the tip sense
	 * polarity. Normal jacks will short tip sense pin to HS1 when headphones are
	 * plugged in and leave tip sense floating when not plugged in. Inverting jacks
	 * short tip sense when unplugged and float when plugged in.
	 * false = Non-inverted
	 * true  = Inverted
	 * Default = Non-inverted
	 */
	bool ts_inv;
	/*
	 * cirrus,ts-dbnc-rise : Debounce the rising edge of TIP_SENSE_PLUG. With no
	 * debounce, the tip sense pin might be noisy on a plug event.
	 * Default = RISE_DEB_1000_MS
	 */
	enum cs42l42_ts_dbnc_rise ts_dbnc_rise;
	/*
	 * cirrus,ts-dbnc-fall : Debounce the falling edge of TIP_SENSE_UNPLUG.
	 * With no debounce, the tip sense pin might be noisy on an unplug event.
	 * Default = FALL_DEB_1000_MS
	 */
	enum cs42l42_ts_dbnc_fall ts_dbnc_fall;
	/*
	 * cirrus,btn-det-init-dbnce : This sets how long the driver sleeps after
	 * enabling button detection interrupts. After auto-detection and before
	 * servicing button interrupts, the HS bias needs time to settle. If you
	 * don't wait, there is possibility for erroneous button interrupt.
	 * Value in ms, 0 - 200.
	 * Default = 100ms
	 */
	unsigned int btn_det_init_dbnce;
	/*
	 * cirrus,btn-det-event-dbnce : This sets how long the driver delays after
	 * receiving a button press interrupt. With level detect interrupts, you want
	 * to wait a small amount of time to make sure the button press is making a
	 * clean connection with the bias resistors.
	 * Value in ms, 0 - 20.
	 * Default = 10ms
	 */
	unsigned int btn_det_event_dbnce;
	/*
	 * cirrus,bias-lvls : For a level-detect headset button scheme, each button
	 * will bias the mic pin to a certain voltage. To determine which button was
	 * pressed, the driver will compare this biased voltage to sequential,
	 * decreasing voltages and will stop when a comparator is tripped,
	 * indicating a comparator voltage < bias voltage. This value represents a
	 * percentage of the internally generated HS bias voltage. For different
	 * hardware setups, a designer might want to tweak this. This is an array of
	 * descending values for the comparator voltage.
	 * Array of 4 values
	 * Each 0-63
	 * < x1 x2 x3 x4 >
	 * Default = < 15 8 4 1>
	 */
	uint64_t bias_lvls[4];
	/* headset bias ramp rate */
	enum cs42l42_hs_bias_ramp_rate hs_bias_ramp_rate;
};
