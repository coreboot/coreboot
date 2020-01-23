/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef SUPERIO_FINTEK_FAN_CONTROL_H
#define SUPERIO_FINTEK_FAN_CONTROL_H

#include <stdint.h>

typedef enum {
	IGNORE_SENSOR = 0,
	EXTERNAL_SENSOR1,
	EXTERNAL_SENSOR2,
	EXTERNAL_SENSOR3,
	EXTERNAL_SENSOR4
} external_sensor;

typedef enum {
	TEMP_SENSOR_THERMISTOR = 0,
	TEMP_SENSOR_BJT,
	TEMP_SENSOR_DEFAULT
} temp_sensor_type;

typedef enum {
	FAN_TYPE_PWM_PUSH_PULL = 0,
	FAN_TYPE_DAC_POWER,
	FAN_TYPE_PWM_OPEN_DRAIN,
	FAN_TYPE_RESERVED
} fan_type;
#define FAN_TYPE_PWM_CHECK			1 /* bit 0 must be 0 for PWM */

typedef enum {
	FAN_MODE_AUTO_RPM = 0,
	FAN_MODE_AUTO_PWM_DAC,
	FAN_MODE_MANUAL_RPM,
	FAN_MODE_MANUAL_PWM_DAC,
	FAN_MODE_DEFAULT
} fan_mode;

typedef enum {
	FAN_PWM_FREQ_23500 = 0,
	FAN_PWM_FREQ_11750,
	FAN_PWM_FREQ_5875,
	FAN_PWM_FREQ_220
} fan_pwm_freq;

typedef enum {
	FAN_TEMP_PECI = 0,
	FAN_TEMP_EXTERNAL_1,
	FAN_TEMP_EXTERNAL_2,
	FAN_TEMP_TSI = 4,
	FAN_TEMP_MXM,
} fan_temp_source;

typedef enum {
	FAN_UP_RATE_2HZ = 0,
	FAN_UP_RATE_5HZ,
	FAN_UP_RATE_10HZ,
	FAN_UP_RATE_20HZ,
	FAN_UP_RATE_DEFAULT,
	FAN_UP_RATE_JUMP = 8
} fan_rate_up;

typedef enum {
	FAN_DOWN_RATE_2HZ = 0,
	FAN_DOWN_RATE_5HZ,
	FAN_DOWN_RATE_10HZ,
	FAN_DOWN_RATE_20HZ,
	FAN_DOWN_RATE_DEFAULT,
	FAN_DOWN_RATE_SAME_AS_UP,
	FAN_DOWN_RATE_JUMP = 8
} fan_rate_down;

typedef enum {
	FAN_FOLLOW_STEP = 0,
	FAN_FOLLOW_INTERPOLATION
} fan_follow;

struct fintek_fan {
	uint8_t fan;
	external_sensor sensor;
	temp_sensor_type stype;
	fan_temp_source temp_source;
	fan_type ftype;
	fan_mode fmode;
	fan_pwm_freq fan_freq;
	fan_rate_up rate_up;
	fan_rate_down rate_down;
	fan_follow follow;
	uint8_t *boundaries;
	uint8_t *sections;
};

#define HWM_STATUS_SUCCESS			0
#define HWM_STATUS_INVALID_FAN			-1
#define HWM_STATUS_INVALID_TEMP_SOURCE		-2
#define HWM_STATUS_INVALID_TYPE			-3
#define HWM_STATUS_INVALID_MODE			-4
#define HWM_STATUS_INVALID_RATE			-5
#define HWM_STATUS_INVALID_FREQUENCY		-6
#define HWM_STATUS_INVALID_TEMP_SENSOR		-7
#define HWM_STATUS_INVALID_BOUNDARY_VALUE	-8
#define HWM_STATUS_INVALID_SECTION_VALUE	-9
#define HWM_STATUS_BOUNDARY_WRONG_ORDER		-10
#define HWM_STATUS_SECTIONS_WRONG_ORDER		-11
#define HWM_STATUS_WARNING_SENSOR_DISCONNECTED	1
#define HWM_STATUS_WARNING_FAN_NOT_PWM		2

#define CPU_DAMAGE_TEMP				110

/*
 * Boundaries order is from highest temp. to lowest. Values from 0 to 127.
 * Boundaries should be defined as u8 boundaries[FINTEK_BOUNDARIES_SIZE].
 */
#define FINTEK_BOUNDARIES_SIZE			4
/*
 * Section defines the duty_cycle/voltage to be used based on where the
 * temperature lies with respect to the boundaries. There are 5 sections
 * (4 boundaries) and the order must be from highest to lowest. Values
 * from 0% to 100%, will be converted internally to percent of 255.
 * Sections should be defined as u8 sections[FINTEK_SECTIONS_SIZE].
 */
#define FINTEK_SECTIONS_SIZE			5

/*
 * When using external sensor, its type must be defined. When using PECI,
 * TSI or MXM use IGNORE_SENSOR to indicate so.
 */
int set_sensor_type(u16 base_address, external_sensor sensor,
						temp_sensor_type type);

/*
 * Define the temperature source used to control a fan.
 */
int set_fan_temperature_source(u16 base_address, u8 fan,
						fan_temp_source source);

/*
 * Define if fan is controlled through PWM or absolute voltage powering it
 * (DAC). Then, under mode, define if control is automatic (SIO) or manual
 * (CPU, through ACPI). Notice there needs to be a match between type and
 * mode (PWM with PWM or DAC with DAC).
 */
int set_fan_type_mode(u16 base_address, u8 fan, fan_type type, fan_mode mode);

/*
 * For fans controlled through pulse width, define the base frequency used.
 */
int set_pwm_frequency(u16 base_address, u8 fan, fan_pwm_freq frequency);

/*
 * For fintek SIO HWM there are 4 (temperature) boundaries points, defining
 * 5 sections (1 fan speed per section). Start with the highest temperature/
 * speed. Temperature is in Celsius, speed is in percentile of max speed. The
 * highest speed should be 100%, no requirements for minimum speed, could be
 * 0 or above 0.
 */
int set_sections(u16 base_address, u8 fan, u8 *boundaries, u8 *sections);

/*
 * Define how often temperature is measured to change fan speed.
 */
int set_fan_speed_change_rate(u16 base_address, u8 fan, fan_rate_up rate_up,
						fan_rate_down rate_down);

/*
 * There a 2 ways a fan can be controlled: A single speed per section, or
 * interpolation. Under interpolation, the section speed is the speed at the
 * lowest temperature of the section (0 Celsius for the lowest section), and
 * it's the speed of the next section at the boundary to the next section.
 * In between these 2 points, it's a linear function. For example, midway
 * between temperature points it'll have a speed that is midway between the
 * section speed and next section speed. Obviously, there's no variation for
 * the highest section, reason why it must be 100% max speed.
 */
int set_fan_follow(u16 base_address, u8 fan, fan_follow follow);

/*
 * This is an upper level API which calls all the above APIs in the
 * appropriate order. Any API failure will be displayed. Alerts will
 * also be displayed, but will not interrupt the sequence, while errors
 * will interrupt the sequence.
 */
int set_fan(struct fintek_fan *fan_init);

#endif /* SUPERIO_FINTEK_FAN_CONTROL_H */
