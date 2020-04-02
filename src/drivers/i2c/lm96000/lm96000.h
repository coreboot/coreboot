/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef DRIVERS_I2C_LM96000_H
#define DRIVERS_I2C_LM96000_H

#include "chip.h"

#define LM96000_CONFIG			0x40
#define  LM96000_READY			(0x1 << 2)
#define  LM96000_START			(0x1 << 0)

#define LM96000_VIN(v)			(0x20 + (v))
#define LM96000_VIN_LOW_LIMIT(v)	(0x44 + (v) * 2)
#define LM96000_VIN_HIGH_LIMIT(v)	(0x45 + (v) * 2)

#define LM96000_TEMP_IN(temp)		(0x25 + (temp))
#define LM96000_TEMP_LOW_LIMIT(temp)	(0x4e + (temp) * 2)
#define LM96000_TEMP_HIGH_LIMIT(temp)	(0x4f + (temp) * 2)

/* 2B, little endian, MSB is latched upon LSB read */
#define LM96000_FAN_IN(fan)		(0x28 + (fan) * 2)
#define LM96000_FAN_LOW_LIMIT(fan)	(0x54 + (fan) * 2)

#define LM96000_FAN_DUTY(fan)		(0x30 + (fan))
#define LM96000_FAN_CFG(fan)		(0x5c + (fan))
#define  LM96000_FAN_CFG_MODE_SHIFT	5
#define  LM96000_FAN_CFG_MODE_MASK	(0x7 << LM96000_FAN_CFG_MODE_SHIFT)
#define  LM96000_FAN_CFG_PWM_INVERT	(0x1 << 4)
#define  LM96000_FAN_CFG_SPINUP_MASK	(0x7 << 0)
#define LM96000_FAN_FREQ(fan)		(0x5f + (fan))
#define  LM96000_FAN_FREQ_MASK		(0xf << 0)
#define LM96000_FAN_MIN_OFF		0x62
#define  LM96000_FAN_MIN(fan)		(1 << ((fan) + 5))
#define LM96000_FAN_MIN_PWM(fan)	(0x64 + (fan))
#define LM96000_TACH_MONITOR_MODE	0x74
#define  LM96000_TACH_MODE_FAN_SHIFT(f)	((f) * 2)
#define  LM96000_TACH_MODE_FAN_MASK(f)	(0x3 << LM96000_TACH_MODE_FAN_SHIFT(f))

#define LM96000_ZONE_RANGE(zone)	(0x5f + (zone))
#define  LM96000_ZONE_RANGE_SHIFT	4
#define  LM96000_ZONE_RANGE_MASK	(0xf << LM96000_ZONE_RANGE_SHIFT)
#define LM96000_ZONE_SMOOTH(zone)	(0x62 + ((zone) + 1) / 2)
#define  LM96000_ZONE_SMOOTH_EN(zone)	(1 << (((zone) % 2) * 4 + 3))
#define  LM96000_ZONE_SMOOTH_SHFT(zone)	(((zone) % 2) * 4)
#define  LM96000_ZONE_SMOOTH_MASK(zone)	(0x7 << LM96000_ZONE_SMOOTH_SHFT(zone))
#define LM96000_ZONE_TEMP_LOW(zone)	(0x67 + (zone))
#define LM96000_ZONE_TEMP_PANIC(zone)	(0x6a + (zone))
#define LM96000_ZONE_HYSTERESIS(zone)	(0x6d + (zone) / 2)
#define  LM96000_ZONE_HYST_SHIFT(zone)	(4 - ((zone) % 2) * 4)
#define  LM96000_ZONE_HYST_MASK(zone)	(0xf << LM96000_ZONE_HYST_SHIFT(zone))

#endif /* DRIVERS_I2C_LM96000_H */
