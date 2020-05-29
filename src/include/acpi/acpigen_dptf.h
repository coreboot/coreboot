/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef ACPI_ACPIGEN_DPTF_H
#define ACPI_ACPIGEN_DPTF_H

#include <device/device.h>
#include <stdbool.h>

/* A common idiom is to use a default value if none is provided (i.e., == 0) */
#define DEFAULT_IF_0(thing, default_) ((thing) ? (thing) : (default_))

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
	DPTF_PARTICIPANT_COUNT,
};

/* DPTF compile-time constants */
enum {
	/* A device can only define _AC0 .. _AC9 i.e. between 0 and 10 Active Cooling Methods */
	DPTF_MAX_ACX			= 10,
	DPTF_MAX_ACTIVE_POLICIES	= (DPTF_PARTICIPANT_COUNT-1),
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

/*
 * This function provides tables of temperature and corresponding fan or percent.  When the
 * temperature thresholds are met (_AC0 - _AC9), the fan is driven to corresponding percentage
 * of full speed.
 */
void dptf_write_active_policies(const struct dptf_active_policy *policies, int max_count);

/* Helper method to open the scope for a given participant. */
void dptf_write_scope(enum dptf_participant participant);

/*
 * Write out a _STA that will check the value of the DPTE field in GNVS, and return 0xF if DPTE
 * is 1, otherwise it will return 0.
 */
void dptf_write_STA(void);

#endif /* ACPI_ACPIGEN_DPTF_H */
