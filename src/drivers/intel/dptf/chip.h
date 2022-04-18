/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _DRIVERS_INTEL_DPTF_CHIP_H_
#define _DRIVERS_INTEL_DPTF_CHIP_H_

#include <acpi/acpigen_dptf.h>
#include <timer.h>

#define DPTF_PASSIVE(src, tgt, tmp, prd) \
	{.source = DPTF_##src, .target = DPTF_##tgt, .temp = (tmp), .period = (prd)}
#define DPTF_CRITICAL(src, tmp, typ) \
	{.source = DPTF_##src, .temp = (tmp), .type = DPTF_CRITICAL_##typ}
#define TEMP_PCT(t, p) {.temp = (t), .fan_pct = (p)}

/* Total number of OEM variables */
#define DPTF_OEM_VARIABLE_COUNT		6

struct drivers_intel_dptf_config {
	struct {
		struct dptf_active_policy active[DPTF_MAX_ACTIVE_POLICIES];
		struct dptf_critical_policy critical[DPTF_MAX_CRITICAL_POLICIES];
		struct dptf_passive_policy passive[DPTF_MAX_PASSIVE_POLICIES];
	} policies;

	struct {
		struct dptf_charger_perf charger_perf[DPTF_MAX_CHARGER_PERF_STATES];
		struct dptf_fan_perf fan_perf[DPTF_MAX_FAN_PERF_STATES];
		struct dptf_power_limits power_limits;
	} controls;

	/* Note that all values in this struct are optional */
	struct {
		struct {
			/* True means _FSL is percentages, False means _FSL is Control values */
			bool fine_grained_control;
			/*
			 * Recommended minimum step size in percentage points to adjust fan
			 * speed when utilizing fine-grained control (1-9)
			 */
			uint8_t step_size;
			/*
			 * True means the platform will issue a Notify (0x80) to the fan device
			 * if a a low fan speed is detected
			 */
			bool low_speed_notify;
		} fan;
		struct {
			/*
			 * The amount of hysteresis implemented in circuitry or in the platform
			 * EC's firmware implementation (using the GTSH object)
			 */
			uint8_t hysteresis;
			/* Name applied to TSR (using the _STR object) */
			const char *desc;
		} tsr[DPTF_MAX_TSR];
	} options;

	/* OEM variables */
	struct {
		uint32_t oem_variables[DPTF_OEM_VARIABLE_COUNT];
	} oem_data;

	/* Rest of platform Power */
	uint32_t prop;
};

#endif /* _DRIVERS_INTEL_DPTF_CHIP_H_ */
