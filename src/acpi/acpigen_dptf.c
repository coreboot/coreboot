/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <acpi/acpigen_dptf.h>

/* Hardcoded paths */
#define TOPLEVEL_DPTF_SCOPE		"\\_SB.DPTF"

/* Defaults */
enum {
	ART_REVISION			= 0,
	DEFAULT_PRIORITY		= 100,
	DEFAULT_WEIGHT			= 100,
	DPTF_MAX_ART_THRESHOLDS		= 10,
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
	default:
		return "";
	}
}

/* Helper to get Scope for participants underneath \_SB.DPTF */
static const char *scope_of(enum dptf_participant participant)
{
	static char scope[16];

	if (participant == DPTF_CPU)
		snprintf(scope, sizeof(scope), "\\_SB.%s", namestring_of(participant));
	else
		snprintf(scope, sizeof(scope), TOPLEVEL_DPTF_SCOPE ".%s",
			 namestring_of(participant));

	return scope;
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

	acpigen_write_scope(TOPLEVEL_DPTF_SCOPE);
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
		acpigen_emit_namestring(namestring_of(DPTF_FAN));
		acpigen_emit_namestring(namestring_of(policies[i].target));
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

	acpigen_write_scope(TOPLEVEL_DPTF_SCOPE);

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
		acpigen_emit_namestring(namestring_of(policies[i].source));
		acpigen_emit_namestring(namestring_of(policies[i].target));
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
