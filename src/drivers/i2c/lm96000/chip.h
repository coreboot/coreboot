/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef DRIVERS_I2C_LM96000_CHIP_H
#define DRIVERS_I2C_LM96000_CHIP_H

#include <stdint.h>

#define LM96000_VIN_CNT		5
#define LM96000_TEMP_IN_CNT	3
#define LM96000_FAN_IN_CNT	4
#define LM96000_PWM_CTL_CNT	3
#define LM96000_TEMP_ZONE_CNT	3

enum lm96000_vin {
	LM96000_2_5V	= 0,
	LM96000_VCCP	= 1,
	LM96000_3_3V	= 2,
	LM96000_5V	= 3,
	LM96000_12V	= 4,
};

enum lm96000_fan_mode {
	/* Bit 7 merely signifies that the mode is set, so we
	   can map the lower bits directly to register values. */
	LM96000_FAN_IGNORE	= 0x00,
	LM96000_FAN_ZONE_1_AUTO	= 0x80,
	LM96000_FAN_ZONE_2_AUTO	= 0x81,
	LM96000_FAN_ZONE_3_AUTO	= 0x82,
	LM96000_FAN_ALWAYS_FULL	= 0x83,
	LM96000_FAN_DISABLED	= 0x84,
	LM96000_FAN_HOTTEST_23	= 0x85,
	LM96000_FAN_HOTTEST_123	= 0x86,
	LM96000_FAN_MANUAL	= 0x87,
};

enum lm96000_pwm_freq {
	LM96000_PWM_10HZ	= 0,
	LM96000_PWM_15HZ	= 1,
	LM96000_PWM_23HZ	= 2,
	LM96000_PWM_30HZ	= 3,
	LM96000_PWM_38HZ	= 4,
	LM96000_PWM_47HZ	= 5,
	LM96000_PWM_61HZ	= 6,
	LM96000_PWM_94HZ	= 7,
	LM96000_PWM_22_5KHZ	= 8,
	LM96000_PWM_24_0KHZ	= 9,
	LM96000_PWM_25_7KHZ	= 10,
	LM96000_PWM_27_7KHZ	= 12,
	LM96000_PWM_30_0KHZ	= 14,
};

enum lm96000_tach_mode {
				 /* 0 will be used for kHz frequencies */
	LM96000_TACH_MODE_1 = 1, /* use if TACHx isn't used with PWMx */
	LM96000_TACH_MODE_2 = 2, /* use either 2 or 3 if TACHx matches PWMx */
	LM96000_TACH_MODE_3 = 3,
};

enum lm96000_spinup_time {
	LM96000_SPINUP_0MS	= 0,
	LM96000_SPINUP_100MS	= 1,
	LM96000_SPINUP_250MS	= 2,
	LM96000_SPINUP_400MS	= 3,
	LM96000_SPINUP_700MS	= 4,
	LM96000_SPINUP_1000MS	= 5,
	LM96000_SPINUP_2000MS	= 6,
	LM96000_SPINUP_4000MS	= 7,
};

struct lm96000_fan_config {
	enum lm96000_fan_mode mode;
	int invert;	/* invert PWM signal */
	enum lm96000_spinup_time spinup;
	enum lm96000_pwm_freq freq;
	enum lm96000_tach_mode tach;
	union {
		u8 duty_cycle;	/* duty cycle in manual mode */
		u8 min_duty;	/* minimum duty cycle */
	};
};

struct lm96000_temp_zone {
	u8 low_temp;	/* temperature for min. duty cycle (in °C) */
	u8 target_temp;	/* temperature for 100% duty cycle (in °C) */
	u8 panic_temp;	/* temperature for 100% duty cycle on all fans */

	/* This is tied to the zone in the implementation I tested
	   with. (Datasheet clearly states the opposite, that this
	   is tied to each PWM output so YMMV.) */
	enum {
		/* turn fan off below `low_temp - hysteresis` */
		LM96000_LOW_TEMP_OFF = 0,
		/* keep PWM at minimum duty cycle */
		LM96000_LOW_TEMP_MIN = 1,
	} min_off;
	u8 hysteresis;
};

/* Implements only those parts currently used by coreboot mainboards. */
struct drivers_i2c_lm96000_config {
	struct {
		u16 low;	/* in mV */
		u16 high;
	} vin[LM96000_VIN_CNT];

	struct {
		signed char low;	/* in °C */
		signed char high;
	} temp_in[LM96000_TEMP_IN_CNT];

	struct {
		u16 low;	/* in RPM */
	} fan_in[LM96000_FAN_IN_CNT];

	struct lm96000_fan_config fan[LM96000_PWM_CTL_CNT];
	struct lm96000_temp_zone zone[LM96000_TEMP_ZONE_CNT];
};

#endif /* DRIVERS_I2C_LM96000_CHIP_H */
