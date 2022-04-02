/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <acpi/acpigen_dptf.h>
#include <stdbool.h>
#include <stdint.h>

/* Defaults */
#define DEFAULT_RAW_UNIT		"ma"

/* DPTF-specific UUIDs */
#define DPTF_PASSIVE_POLICY_1_0_UUID	"42A441D6-AE6A-462B-A84B-4A8CE79027D3"
#define DPTF_CRITICAL_POLICY_UUID	"97C68AE7-15FA-499c-B8C9-5DA81D606E0A"
#define DPTF_ACTIVE_POLICY_UUID		"3A95C389-E4B8-4629-A526-C52C88626BAE"

enum {
	ART_REVISION			= 0,
	DEFAULT_PRIORITY		= 100,
	DEFAULT_TRIP_POINT		= 0xFFFFFFFFull,
	DEFAULT_WEIGHT			= 100,
	DPTF_MAX_ART_THRESHOLDS		= 10,
	FPS_REVISION			= 0,
	PPCC_REVISION			= 2,
	RAPL_PL1_INDEX			= 0,
	RAPL_PL2_INDEX			= 1,
};

/* Convert degrees C to 1/10 degree Kelvin for ACPI */
static int to_acpi_temp(int deg_c)
{
	return deg_c * 10 + 2732;
}

/* Converts ms to 1/10th second for ACPI */
static int to_acpi_time(int ms)
{
	return ms / 100;
}

/* Writes out a 0-argument non-Serialized Method that returns an Integer */
static void write_simple_return_method(const char *name, int value)
{
	acpigen_write_method(name, 0);
	acpigen_write_return_integer(value);
	acpigen_pop_len(); /* Method */
}

/* Writes out 'count' ZEROs in a row */
static void write_zeros(int count)
{
	for (; count; --count)
		acpigen_write_integer(0);
}

/* Return the assigned namestring of any participant */
static const char *namestring_of(enum dptf_participant participant)
{
	switch (participant) {
	case DPTF_CPU:
		return "TCPU";
	case DPTF_CHARGER:
		return "TCHG";
	case DPTF_FAN:
		return "TFN1";
	case DPTF_TEMP_SENSOR_0:
		return "TSR0";
	case DPTF_TEMP_SENSOR_1:
		return "TSR1";
	case DPTF_TEMP_SENSOR_2:
		return "TSR2";
	case DPTF_TEMP_SENSOR_3:
		return "TSR3";
	case DPTF_TEMP_SENSOR_4:
		return "TSR4";
	case DPTF_TPCH:
		return "TPCH";
	case DPTF_POWER:
		return "TPWR";
	case DPTF_BATTERY:
		return "TBAT";
	default:
		return "";
	}
}

/* Helper to get Scope for participants underneath \_SB.DPTF */
static const char *scope_of(enum dptf_participant participant)
{
	static char scope[16];

	if (participant == DPTF_CPU)
		snprintf(scope, sizeof(scope), TCPU_SCOPE ".%s", namestring_of(participant));
	else
		snprintf(scope, sizeof(scope), DPTF_DEVICE_PATH ".%s",
			 namestring_of(participant));

	return scope;
}

/*
 * Most of the DPTF participants are underneath the \_SB.DPTF scope, so we can just get away
 * with using the simple namestring for references, but the TCPU has a different scope, so
 * either an absolute or relative path must be used instead.
 */
static const char *path_of(enum dptf_participant participant)
{
	if (participant == DPTF_CPU)
		return scope_of(participant);
	else
		return namestring_of(participant);
}

/* Write out scope of a participant */
void dptf_write_scope(enum dptf_participant participant)
{
	acpigen_write_scope(scope_of(participant));
}

/*
 * This table describes active cooling relationships between the system's fan and the
 * temperature sensors that it can have an effect on. As ever-increasing temperature thresholds
 * are crossed (_AC9.._AC0, low to high), the corresponding fan percentages listed in this table
 * are used to increase the speed of the fan in order to speed up cooling.
 */
static void write_active_relationship_table(const struct dptf_active_policy *policies,
					    int max_count)
{
	char *pkg_count;
	int i, j;

	/* Nothing to do */
	if (!max_count || policies[0].target == DPTF_NONE)
		return;

	acpigen_write_scope(DPTF_DEVICE_PATH);
	acpigen_write_method("_ART", 0);

	/* Return this package */
	acpigen_emit_byte(RETURN_OP);

	/* Keep track of items added to the package */
	pkg_count = acpigen_write_package(1); /* The '1' here is for the revision */
	acpigen_write_integer(ART_REVISION);

	for (i = 0; i < max_count; ++i) {
		/*
		 * These have to be filled out from AC0 down to AC9, filling in only as many
		 * as are used. As soon as one isn't filled in, we're done.
		 */
		if (policies[i].target == DPTF_NONE)
			break;

		(*pkg_count)++;

		/* Source, Target, Percent, Fan % for each of _AC0 ... _AC9 */
		acpigen_write_package(13);
		acpigen_emit_namestring(path_of(DPTF_FAN));
		acpigen_emit_namestring(path_of(policies[i].target));
		acpigen_write_integer(DEFAULT_IF_0(policies[i].weight, DEFAULT_WEIGHT));

		/* Write out fan %; corresponds with target's _ACx methods */
		for (j = 0; j < DPTF_MAX_ART_THRESHOLDS; ++j)
			acpigen_write_integer(policies[i].thresholds[j].fan_pct);

		acpigen_pop_len(); /* inner Package */
	}

	acpigen_pop_len(); /* outer Package */
	acpigen_pop_len(); /* Method _ART */
	acpigen_pop_len(); /* Scope */
}

/*
 * _AC9 through _AC0 represent temperature thresholds, in increasing order, defined from _AC0
 * down, that, when reached, DPTF will activate TFN1 in order to actively cool the temperature
 * sensor(s). As increasing thresholds are reached, the fan is spun faster.
 */
static void write_active_cooling_methods(const struct dptf_active_policy *policies,
					 int max_count)
{
	char name[5];
	int i, j;

	/* Nothing to do */
	if (!max_count || policies[0].target == DPTF_NONE)
		return;

	for (i = 0; i < max_count; ++i) {
		if (policies[i].target == DPTF_NONE)
			break;

		dptf_write_scope(policies[i].target);

		/* Write out as many of _AC0 through _AC9 that are applicable */
		for (j = 0; j < DPTF_MAX_ACX; ++j) {
			if (!policies[i].thresholds[j].temp)
				break;

			snprintf(name, sizeof(name), "_AC%1X", j);
			write_simple_return_method(name, to_acpi_temp(
							   policies[i].thresholds[j].temp));
		}

		acpigen_pop_len(); /* Scope */
	}
}

void dptf_write_active_policies(const struct dptf_active_policy *policies, int max_count)
{
	write_active_relationship_table(policies, max_count);
	write_active_cooling_methods(policies, max_count);
}

/*
 * This writes out the Thermal Relationship Table, which describes the thermal relationships
 * between participants in a thermal zone. This information is used to passively cool (i.e.,
 * throttle) the Source (source of heat), in order to indirectly cool the Target (temperature
 * sensor).
 */
static void write_thermal_relationship_table(const struct dptf_passive_policy *policies,
					     int max_count)
{
	char *pkg_count;
	int i;

	/* Nothing to do */
	if (!max_count || policies[0].source == DPTF_NONE)
		return;

	acpigen_write_scope(DPTF_DEVICE_PATH);

	/*
	 * A _TRT Revision (TRTR) of 1 means that the 'Priority' field is an arbitrary priority
	 * value to be used for this specific relationship. The priority value determines the
	 * order in which various sources are used in a passive thermal action for a given
	 * target.
	 */
	acpigen_write_name_integer("TRTR", 1);

	/* Thermal Relationship Table */
	acpigen_write_method("_TRT", 0);

	/* Return this package */
	acpigen_emit_byte(RETURN_OP);
	pkg_count = acpigen_write_package(0);

	for (i = 0; i < max_count; ++i) {
		/* Stop writing the table once an entry is empty */
		if (policies[i].source == DPTF_NONE)
			break;

		/* Keep track of outer package item count */
		(*pkg_count)++;

		acpigen_write_package(8);

		/* Source, Target, Priority, Sampling Period */
		acpigen_emit_namestring(path_of(policies[i].source));
		acpigen_emit_namestring(path_of(policies[i].target));
		acpigen_write_integer(DEFAULT_IF_0(policies[i].priority, DEFAULT_PRIORITY));
		acpigen_write_integer(to_acpi_time(policies[i].period));

		/* Reserved */
		write_zeros(4);

		acpigen_pop_len(); /* Package */
	}

	acpigen_pop_len(); /* Package */
	acpigen_pop_len(); /* Method */
	acpigen_pop_len(); /* Scope */
}

/*
 * When a temperature sensor measures above its the temperature returned in its _PSV Method,
 * DPTF will begin throttling Sources in order to indirectly cool the sensor.
 */
static void write_all_PSV(const struct dptf_passive_policy *policies, int max_count)
{
	int i;

	for (i = 0; i < max_count; ++i) {
		if (policies[i].source == DPTF_NONE)
			break;

		dptf_write_scope(policies[i].target);
		write_simple_return_method("_PSV", to_acpi_temp(policies[i].temp));
		acpigen_pop_len(); /* Scope */
	}
}

void dptf_write_passive_policies(const struct dptf_passive_policy *policies, int max_count)
{
	write_thermal_relationship_table(policies, max_count);
	write_all_PSV(policies, max_count);
}

void dptf_write_critical_policies(const struct dptf_critical_policy *policies, int max_count)
{
	int i;

	for (i = 0; i < max_count; ++i) {
		if (policies[i].source == DPTF_NONE)
			break;

		dptf_write_scope(policies[i].source);

		/* Choose _CRT or _HOT */
		write_simple_return_method(policies[i].type == DPTF_CRITICAL_SHUTDOWN ?
					   "_CRT" : "_HOT", to_acpi_temp(policies[i].temp));

		acpigen_pop_len(); /* Scope */
	}
}

void dptf_write_charger_perf(const struct dptf_charger_perf *states, int max_count)
{
	char *pkg_count;
	int i;

	if (!max_count || !states[0].control)
		return;

	dptf_write_scope(DPTF_CHARGER);

	/* PPSS - Participant Performance Supported States */
	acpigen_write_method("PPSS", 0);
	acpigen_emit_byte(RETURN_OP);

	pkg_count = acpigen_write_package(0);
	for (i = 0; i < max_count; ++i) {
		if (!states[i].control)
			break;

		(*pkg_count)++;

		/*
		 * 0, 0, 0, 0, # Reserved
		 * Control, Raw Performance, Raw Unit, 0 # Reserved
		 */
		acpigen_write_package(8);
		write_zeros(4);
		acpigen_write_integer(states[i].control);
		acpigen_write_integer(states[i].raw_perf);
		acpigen_write_string(DEFAULT_RAW_UNIT);
		acpigen_write_integer(0);
		acpigen_pop_len(); /* inner Package */
	}

	acpigen_pop_len(); /* outer Package */
	acpigen_pop_len(); /* Method PPSS */
	acpigen_pop_len(); /* Scope */
}

void dptf_write_fan_perf(const struct dptf_fan_perf *states, int max_count)
{
	char *pkg_count;
	int i;

	if (!max_count || !states[0].percent)
		return;

	dptf_write_scope(DPTF_FAN);

	/* _FPS - Fan Performance States */
	acpigen_write_name("_FPS");
	pkg_count = acpigen_write_package(1); /* 1 for Revision */
	acpigen_write_integer(FPS_REVISION); /* revision */

	for (i = 0; i < max_count; ++i) {
		/*
		 * Some _FPS tables do include a last entry where Percent is 0, but Power is
		 * called out, so this table is finished when both are zero.
		 */
		if (!states[i].percent && !states[i].power)
			break;

		(*pkg_count)++;
		acpigen_write_package(5);
		acpigen_write_integer(states[i].percent);
		acpigen_write_integer(DEFAULT_TRIP_POINT);
		acpigen_write_integer(states[i].speed);
		acpigen_write_integer(states[i].noise_level);
		acpigen_write_integer(states[i].power);
		acpigen_pop_len(); /* inner Package */
	}

	acpigen_pop_len(); /* Package */
	acpigen_pop_len(); /* Scope */
}

void dptf_write_power_limits(const struct dptf_power_limits *limits)
{
	char *pkg_count;

	/* Nothing to do */
	if (!limits->pl1.min_power && !limits->pl2.min_power)
		return;

	dptf_write_scope(DPTF_CPU);
	acpigen_write_method("PPCC", 0);

	pkg_count = acpigen_write_package(1); /* 1 for the Revision */
	acpigen_write_integer(PPCC_REVISION); /* revision */

	if (limits->pl1.min_power) {
		(*pkg_count)++;
		acpigen_write_package(6);
		acpigen_write_integer(RAPL_PL1_INDEX);
		acpigen_write_integer(limits->pl1.min_power);
		acpigen_write_integer(limits->pl1.max_power);
		acpigen_write_integer(limits->pl1.time_window_min);
		acpigen_write_integer(limits->pl1.time_window_max);
		acpigen_write_integer(limits->pl1.granularity);
		acpigen_pop_len(); /* inner Package */
	}

	if (limits->pl2.min_power) {
		(*pkg_count)++;
		acpigen_write_package(6);
		acpigen_write_integer(RAPL_PL2_INDEX);
		acpigen_write_integer(limits->pl2.min_power);
		acpigen_write_integer(limits->pl2.max_power);
		acpigen_write_integer(limits->pl2.time_window_min);
		acpigen_write_integer(limits->pl2.time_window_max);
		acpigen_write_integer(limits->pl2.granularity);
		acpigen_pop_len(); /* inner Package */
	}

	acpigen_pop_len(); /* outer Package */
	acpigen_pop_len(); /* Method */
	acpigen_pop_len(); /* Scope */
}

void dptf_write_STR(const char *str)
{
	if (!str)
		return;

	acpigen_write_name_string("_STR", str);
}

void dptf_write_fan_options(bool fine_grained, int step_size, bool low_speed_notify)
{
	acpigen_write_name("_FIF");
	acpigen_write_package(4);

	acpigen_write_integer(0); /* Revision */
	acpigen_write_integer(fine_grained);
	acpigen_write_integer(step_size);
	acpigen_write_integer(low_speed_notify);
	acpigen_pop_len(); /* Package */
}

void dptf_write_tsr_hysteresis(uint8_t hysteresis)
{
	if (!hysteresis)
		return;

	acpigen_write_name_integer("GTSH", hysteresis);
}

void dptf_write_enabled_policies(const struct dptf_active_policy *active_policies,
				 int active_count,
				 const struct dptf_passive_policy *passive_policies,
				 int passive_count,
				 const struct dptf_critical_policy *critical_policies,
				 int critical_count)
{
	bool is_active_used;
	bool is_passive_used;
	bool is_critical_used;
	int pkg_count;

	is_active_used = (active_count && active_policies[0].target != DPTF_NONE);
	is_passive_used = (passive_count && passive_policies[0].target != DPTF_NONE);
	is_critical_used = (critical_count && critical_policies[0].source != DPTF_NONE);
	pkg_count = is_active_used + is_passive_used + is_critical_used;

	if (!pkg_count)
		return;

	acpigen_write_scope(DPTF_DEVICE_PATH);
	acpigen_write_name("IDSP");
	acpigen_write_package(pkg_count);

	if (is_active_used)
		acpigen_write_uuid(DPTF_ACTIVE_POLICY_UUID);

	if (is_passive_used)
		acpigen_write_uuid(DPTF_PASSIVE_POLICY_1_0_UUID);

	if (is_critical_used)
		acpigen_write_uuid(DPTF_CRITICAL_POLICY_UUID);

	acpigen_pop_len(); /* Package */
	acpigen_pop_len(); /* Scope */
}
