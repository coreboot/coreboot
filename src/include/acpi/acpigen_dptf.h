/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef ACPI_ACPIGEN_DPTF_H
#define ACPI_ACPIGEN_DPTF_H

#include <device/device.h>
#include <stdbool.h>
#include <stdint.h>

/* A common idiom is to use a default value if none is provided (i.e., == 0) */
#define DEFAULT_IF_0(thing, default_) ((thing) ? (thing) : (default_))

/* Hardcoded paths */
#define DPTF_DEVICE_PATH	"\\_SB.DPTF"
#define TCPU_SCOPE		"\\_SB.PCI0"

/* List of available participants (i.e., they can participate in policies) */
enum dptf_participant {
	DPTF_NONE,
	DPTF_CPU,
	DPTF_CHARGER,
	DPTF_FAN,
	DPTF_TEMP_SENSOR_0,
	DPTF_TEMP_SENSOR_1,
	DPTF_TEMP_SENSOR_2,
	DPTF_TEMP_SENSOR_3,
	DPTF_TEMP_SENSOR_4,
	DPTF_TPCH,
	DPTF_POWER,
	DPTF_BATTERY,
	DPTF_PARTICIPANT_COUNT,
};

/* DPTF compile-time constants */
enum {
	/* A device can only define _AC0 .. _AC9 i.e. between 0 and 10 Active Cooling Methods */
	DPTF_MAX_ACX			= 10,
	DPTF_MAX_ACTIVE_POLICIES	= (DPTF_PARTICIPANT_COUNT-1),
	DPTF_MAX_PASSIVE_POLICIES	= (DPTF_PARTICIPANT_COUNT-1),
	DPTF_MAX_CRITICAL_POLICIES	= (DPTF_PARTICIPANT_COUNT-1),

	/* Maximum found by automatic inspection (awk) */
	DPTF_MAX_CHARGER_PERF_STATES	= 10,
	DPTF_MAX_FAN_PERF_STATES	= 20,

	/* From ACPI spec 6.3 */
	DPTF_FIELD_UNUSED		= 0xFFFFFFFFull,

	/* Max supported by DPTF */
	DPTF_MAX_TSR			= 5,
};

/* Active Policy */
struct dptf_active_policy {
	/* Device capable of being affected by the fan */
	enum dptf_participant target;
	/* Source's contribution to the Target's cooling capability as a percentage */
	uint8_t weight;
	/* When target reaches temperature 'temp', the source will turn on at 'fan_pct' % */
	struct {
		/* (degrees C) */
		uint8_t temp;
		/* 0 - 100 */
		uint8_t fan_pct;
	} thresholds[DPTF_MAX_ACX];
};

/* Passive Policy */
struct dptf_passive_policy {
	/* The device that can be throttled */
	enum dptf_participant source;
	/* The device that controls the throttling */
	enum dptf_participant target;
	/* How often to check the temperature for required throttling (ms) */
	uint16_t period;
	/* The trip point for turning on throttling (degrees C) */
	uint8_t temp;
	/* Relative priority between Policies */
	uint8_t priority;
};

/* Critical Policy type: graceful S4 transition or graceful shutdown */
enum dptf_critical_policy_type {
	DPTF_CRITICAL_S4,
	DPTF_CRITICAL_SHUTDOWN,
};

/* Critical Policy */
struct dptf_critical_policy {
	/* The device that can trigger a critical event */
	enum dptf_participant source;
	/* What type of critical policy */
	enum dptf_critical_policy_type type;
	/* Temperature to activate policy, degrees C */
	uint8_t temp;
};

/* Different levels of charging capability, chosen by passive policies */
struct dptf_charger_perf {
	/* Control value */
	uint8_t control;
	/* Charging performance, in mA */
	uint16_t raw_perf;
};

/* Different levels of fan activity, chosen by active policies */
struct dptf_fan_perf {
	/* Fan percentage level */
	uint8_t percent;
	/* Fan speed, in RPM */
	uint16_t speed;
	/* Noise level, in 0.1 dBs */
	uint16_t noise_level;
	/* Power in mA */
	uint16_t power;
};

/* Running Average Power Limits (RAPL) */
struct dptf_power_limit_config {
	/* Minimum level of power limit, in mW */
	uint32_t min_power;
	/* Maximum level of power limit, in mW */
	uint32_t max_power;
	/* Minimum time window running average is over, in seconds */
	uint32_t time_window_min;
	/* Maximum time window running average is over, in seconds */
	uint32_t time_window_max;
	/* Granularity of the power limit setting (between min and max), in mW */
	uint16_t granularity;
};

/* Only PL1 and PL2 are controllable via DPTF */
struct dptf_power_limits {
	struct dptf_power_limit_config pl1;
	struct dptf_power_limit_config pl2;
};

/*
 * This function writes out \_SB.DPTF.IDSP, which describes the different DPTF policies that
 * this implementation is using.
 */
void dptf_write_enabled_policies(const struct dptf_active_policy *active_policies,
				 int active_count,
				 const struct dptf_passive_policy *passive_policies,
				 int passive_count,
				 const struct dptf_critical_policy *critical_policies,
				 int critical_count);

/*
 * This function provides tables of temperature and corresponding fan or percent.  When the
 * temperature thresholds are met (_AC0 - _AC9), the fan is driven to corresponding percentage
 * of full speed.
 */
void dptf_write_active_policies(const struct dptf_active_policy *policies, int max_count);

/*
 * This function uses the definition of the passive policies to write out _PSV Methods on all
 * participants that define it.  It also writes out the Thermal Relationship Table
 * (\_SB.DPTF._TRT), which describes various passive (i.e., throttling) policies that can be
 * applies when temperature sensors reach the _PSV threshold.
 */
void dptf_write_passive_policies(const struct dptf_passive_policy *policies, int max_count);

/*
 * Critical policies are temperature thresholds that, when reached, will cause the system to
 * take some emergency action in order to eliminate excess temperatures from damaging the
 * system. The emergency actions are a graceful suspend or a graceful shutdown.
 */
void dptf_write_critical_policies(const struct dptf_critical_policy *policies, int max_count);

/*
 * These are various performance levels for battery charging. They can be used in conjunction
 * with passive policies to lower the charging rate when the _PSV threshold is met.
 */
void dptf_write_charger_perf(const struct dptf_charger_perf *perf, int max_count);

/*
 * This function writes an ACPI table describing various performance levels possible for active
 * policies. They indicate, for a given fan percentage level:
 *  1) What the corresponding speed is (in RPM)
 *  2) The expected noise level (in tenths of decibels AKA centibels, or DPTF_FIELD_UNUSED)
 *  3) The power consumption (in mW, or DPTF_FIELD_UNUSED to indicate this field is unused).
 *  4) The corresponding active cooling trip point (from _ART) (typically left as
 *     DPTF_FIELD_UNUSED).
 */
void dptf_write_fan_perf(const struct dptf_fan_perf *perf, int max_count);

/*
 * This function writes out a PPCC table, which indicates power ranges that different Intel
 * Running Average Power Limits (RAPLs) can take, as well as the time period they average over
 * and the minimum adjustment amount.
 */
void dptf_write_power_limits(const struct dptf_power_limits *limits);

/* Set the _STR Name */
void dptf_write_STR(const char *str);

/* Set options in the _FIF table */
void dptf_write_fan_options(bool fine_grained, int step_size, bool low_speed_notify);

/*
 * Sets the amount of inherent hysteresis in temperature sensor readings (either from hardware
 * circuitry or possibly from the EC's firmware implementation.
 */
void dptf_write_tsr_hysteresis(uint8_t hysteresis);

/* Helper method to open the scope for a given participant. */
void dptf_write_scope(enum dptf_participant participant);

/*
 * Write out a _STA that will check the value of the DPTE field in GNVS, and return 0xF if DPTE
 * is 1, otherwise it will return 0.
 */
void dptf_write_STA(void);

#endif /* ACPI_ACPIGEN_DPTF_H */
