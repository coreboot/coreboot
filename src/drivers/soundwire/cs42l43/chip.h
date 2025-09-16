/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_SOUNDWIRE_CS42L43_CHIP_H__
#define __DRIVERS_SOUNDWIRE_CS42L43_CHIP_H__

#include <acpi/acpi.h>
#include <acpi/acpi_device.h>
#include <mipi/ids.h>
#include <stdbool.h>

enum cs42l43_detect_us {
	DET_20_US = 20,
	DET_100_US = 100,
	DET_1_MS = 1000,
	DET_10_MS = 10000,
	DET_50_MS = 50000,
	DET_75_MS = 75000,
	DET_100_MS = 100000,
	DET_200_MS = 200000,
};

enum cs42l43_db_ms {
	DB_0_MS = -1, // To be handled as "0" in fill_ssdt function
	DB_125_MS = 125,
	DB_250_MS = 250,
	DB_500_MS = 500,
	DB_750_MS = 750,
	DB_1000_MS = 1000,
	DB_1250_MS = 1250,
	DB_1500_MS = 1500,
};

enum cs42l43_bias_ramp_ms {
	RAMP_10_MS = 10,
	RAMP_40_MS = 40,
	RAMP_90_MS = 90,
	RAMP_170_MS = 170,
};

enum cs42l43_bias_sense {
	BIAS_SENSE_OFF = -1,	// To be handled as "0" in fill_ssdt function
	BIAS_SENSE_14_UA = 14,
	BIAS_SENSE_24_UA = 24,
	BIAS_SENSE_43_UA = 43,
	BIAS_SENSE_52_UA = 52,
	BIAS_SENSE_61_UA = 61,
	BIAS_SENSE_71_UA = 71,
	BIAS_SENSE_90_UA = 90,
	BIAS_SENSE_99_UA = 99,
};

struct drivers_soundwire_cs42l43_config {
	char acpi_name[ACPI_NAME_BUFFER_SIZE]; /* Set by the acpi_name ops */
	const char *desc;

	const char *sub; /* SUB ID to uniquely identify system */

	/* Optional jack / accessory detect properties (from cirrus,cs42l43.yaml) */
	/*
	 * cirrus,bias-low: Select a 1.8V headset mic bias rather than 2.8V.
	 * false: 2.8V, true: 1.8V
	 * Default: false
	 */
	bool bias_low;
	/*
	 * cirrus,bias-sense-microamp: Current at which the headset micbias sense clamp will engage
	 * 0 disables it.
	 * Valid values: {0,14,24,43,52,61,71,90,99} (microamps)
	 * Default: 14
	 */
	enum cs42l43_bias_sense bias_sense_microamp;
	/*
	 * cirrus,bias-ramp-ms: Time in milliseconds the hardware allows for the headset micbias to
	 * ramp up.
	 * Valid values: {10,40,90,170} (ms)
	 * Default: 170
	 */
	enum cs42l43_bias_ramp_ms bias_ramp_ms;
	/*
	 * cirrus,detect-us: Time in microseconds the type detection will run for. Long values will
	 * cause more audible effects, but give more accurate detection.
	 * Valid values: {20,100,1000,10000,50000,75000,100000,200000} (us)
	 * Default: 1000
	 */
	enum cs42l43_detect_us detect_us;
	/*
	 * cirrus,button-automute: Enable the hardware automuting of decimator 1 when a headset button is
	 * pressed.
	 */
	bool button_automute;
	/*
	 * cirrus,buttons-ohms: Impedance in Ohms for each headset button, these should be listed in
	 * ascending order.
	 * Valid count: 1..6 entries each > 0 (Ohms)
	 */
	uint64_t buttons_ohms[6];
	int buttons_ohms_count;
	/*
	 * cirrus,tip-debounce-ms: Software debounce on tip sense triggering in milliseconds.
	 * Default: 0
	 */
	int tip_debounce_ms;
	/*
	 * cirrus,tip-invert: Indicates tip detect polarity, inverted implies open-circuit whilst the
	 * jack is inserted.
	 */
	bool tip_invert;
	/*
	 * cirrus,tip-disable-pullup: Indicates if the internal pullup on the tip detect should be disabled.
	 */
	bool tip_disable_pullup;
	/*
	 * cirrus,tip-fall-db-ms: Time in milliseconds a falling edge on the tip detect should be hardware
	 * debounced for. Note the falling edge is considered after the invert.
	 * Valid values: {0,125,250,500,750,1000,1250,1500} (ms)
	 * Default: 500
	 */
	enum cs42l43_db_ms tip_fall_db_ms;
	/*
	 * cirrus,tip-rise-db-ms: Time in milliseconds a rising edge on the tip detect should be hardware
	 * debounced for. Note the rising edge is considered after the invert.
	 * Valid values: {0,125,250,500,750,1000,1250,1500} (ms)
	 * Default: 500
	 */
	enum cs42l43_db_ms tip_rise_db_ms;
	/*
	 * cirrus,use-ring-sense: Indicates if the ring sense should be used.
	 */
	bool use_ring_sense;
	/*
	 * cirrus,ring-invert: Indicates ring detect polarity, inverted implies open-circuit whilst the
	 * jack is inserted.
	 */
	bool ring_invert;
	/*
	 * cirrus,ring-disable-pullup: Indicates if the internal pullup on the ring detect should be disabled.
	 */
	bool ring_disable_pullup;
	/*
	 * cirrus,ring-fall-db-ms: Time in milliseconds a falling edge on the ring detect should be hardware
	 * debounced for. Note the falling edge is considered after the invert.
	 * Valid values: {0,125,250,500,750,1000,1250,1500} (ms)
	 * Default: 500
	 */
	enum cs42l43_db_ms ring_fall_db_ms;
	/*
	 * cirrus,ring-rise-db-ms: Time in milliseconds a rising edge on the ring detect should be hardware
	 * debounced for. Note the rising edge is considered after the invert.
	 * Valid values: {0,125,250,500,750,1000,1250,1500} (ms)
	 * Default: 500
	 */
	enum cs42l43_db_ms ring_rise_db_ms;
};

#endif /* __DRIVERS_SOUNDWIRE_CS42L43_CHIP_H__ */
