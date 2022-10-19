/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <acpi/acpigen_dptf.h>
#include <ec/google/common/dptf.h>

/*
 * The Chrome EC is typically in charge of many system functions, including battery charging and
 * fan PWM control. This places it in the middle of a DPTF implementation and therefore, many of
 * the "helper" ACPI Methods themselves call EC Methods. Because of that, the responsibility for
 * producing the corresponding AML lies here.
 */

/* DPTF Event types */
enum {
	TRIP_POINTS_CHANGED_EVENT	= 0x81,
	THERMAL_EVENT			= 0x90,
};

/* EC constants */
enum {
	EC_FAN_DUTY_AUTO		= 0xFF,
};

static void write_charger_PPPC(const struct device *ec)
{
	acpigen_write_method_serialized("PPPC", 0);

	/*
	 * Convert size of PPSS table to index
	 *
	 * Store (SizeOf (PPSS), Local0)
	 * Decrement (Local0)
	 */
	acpigen_write_store();
	acpigen_emit_byte(SIZEOF_OP);
	acpigen_emit_namestring("PPSS");
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_emit_byte(DECREMENT_OP);
	acpigen_emit_byte(LOCAL0_OP);

	/*
	 * Check if charging is disabled (AC removed)
	 *
	 * If (\_SB.PCI0.LPCB.EC0.ACEX () = Zero) {
	 *    Return (Local0)
	 * }
	 */
	acpigen_write_if();
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_namestring(acpi_device_path_join(ec, "ACEX"));
	acpigen_emit_byte(ZERO_OP);
	acpigen_write_return_op(LOCAL0_OP);
	acpigen_pop_len(); /* If */

	/* Return highest power state (index 0) */
	acpigen_write_return_op(ZERO_OP);

	acpigen_pop_len(); /* Method */
}

static void write_charger_SPPC(const struct device *ec)
{
	/*
	 * SPPC - Set charger current limit
	 * Method(SPPC, 1) {
	 *   Store (DeRefOf (Index (DeRefOf (Index
	 *      (PPSS, ToInteger (Arg0))), 4)), Local0)
	 *   \_SB.PCI0.LPCB.EC0.CHGS (Local0)
	 * }
	 */

	acpigen_write_method_serialized("SPPC", 1);

	/* Retrieve Control (index 4) for specified PPSS level */
	acpigen_emit_byte(STORE_OP);
	acpigen_emit_byte(DEREF_OP);
	acpigen_emit_byte(INDEX_OP);
	acpigen_emit_byte(DEREF_OP);
	acpigen_emit_byte(INDEX_OP);
	acpigen_emit_namestring("PPSS");
	acpigen_write_to_integer(ARG0_OP, ZERO_OP);
	acpigen_emit_byte(ZERO_OP); /* 3rd arg to Index */
	acpigen_write_integer(4); /* Index */
	acpigen_emit_byte(ZERO_OP); /* 3rd arg to Index */
	acpigen_emit_byte(LOCAL0_OP);

	/* Pass Control value to EC to limit charging */
	acpigen_emit_namestring(acpi_device_path_join(ec, "CHGS"));
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_pop_len(); /* Method */
}

static void write_fan_fst(const struct device *ec)
{
	/* TFST is a package that is used to store data from FAND */
	acpigen_write_name("TFST");
	acpigen_write_package(3);
	acpigen_write_integer(0); /* Revision */
	acpigen_write_integer(0); /* Control */
	acpigen_write_integer(0); /* Speed */
	acpigen_pop_len(); /* Package */

	/* _FST */
	acpigen_write_method_serialized("_FST", 0);
	acpigen_write_store();
	acpigen_emit_namestring(acpi_device_path_join(ec, "FAND"));
	acpigen_emit_byte(INDEX_OP);
	acpigen_emit_namestring("TFST");
	acpigen_write_integer(1);
	acpigen_emit_byte(ZERO_OP); /* 3rd arg to Index */
	acpigen_write_store();
	acpigen_emit_namestring(acpi_device_path_join(ec, "FAN0"));
	acpigen_emit_byte(INDEX_OP);
	acpigen_emit_namestring("TFST");
	acpigen_write_integer(2);
	acpigen_emit_byte(ZERO_OP);
	acpigen_emit_byte(RETURN_OP);
	acpigen_emit_namestring("TFST");
	acpigen_pop_len(); /* Method _FST */
}

static void write_fan_fsl(const struct device *ec)
{
	/* _FSL */
	acpigen_write_method_serialized("_FSL", 1);
	acpigen_write_store();
	acpigen_emit_byte(ARG0_OP);
	acpigen_emit_namestring(acpi_device_path_join(ec, "FAND"));
	acpigen_pop_len(); /* Method _FSL */
}

/*
 * Emit code to execute if the policy is enabled after this function is called, and also
 * remember to manually add a acpigen_pop_len() afterwards!
 */
static void write_is_policy_enabled(bool enabled)
{
	/*
	 * Local0 = SizeOf (IDSP)
	 * Local1 = 0
	 * Local2 = 0
	 *
	 * While (Local1 < Local0) {
	 *    If (IDSP[Local1] == Arg0 && Arg1 == enabled) {
	 *        Local2 = 1
	 *    }
	 *    Local1++
	 * }
	 *
	 * If (Local2 == 1) {
	 * ..........
	 */

	/* Local0 = SizeOf (IDSP) */
	acpigen_write_store();
	acpigen_emit_byte(SIZEOF_OP);
	acpigen_emit_namestring("IDSP");
	acpigen_emit_byte(LOCAL0_OP);

	/* Local1 = 0 (index variable) */
	acpigen_write_store();
	acpigen_write_zero();
	acpigen_emit_byte(LOCAL1_OP);

	/* Local2 = 0 (out variable, 1=found, 0=not found) */
	acpigen_write_store();
	acpigen_write_zero();
	acpigen_emit_byte(LOCAL2_OP);

	/*
	 * While (Local1 < Local0) {
	 */
	acpigen_emit_byte(WHILE_OP);
	acpigen_write_len_f();
	acpigen_emit_byte(LLESS_OP);
	acpigen_emit_byte(LOCAL1_OP);
	acpigen_emit_byte(LOCAL0_OP);

	/* If (IDSP[Local1] == Arg0 && Arg1 == 1) { */
	acpigen_write_if();
	acpigen_emit_byte(LAND_OP);
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_byte(DEREF_OP);
	acpigen_emit_byte(INDEX_OP);
	acpigen_emit_namestring("IDSP");
	acpigen_emit_byte(LOCAL1_OP);
	acpigen_emit_byte(ZERO_OP); /* 3rd arg of index - unused */
	acpigen_emit_byte(ARG0_OP); /* end lequal */
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_byte(ARG1_OP);
	acpigen_write_integer(enabled ? 1 : 0);

	/* { Local2 = 1 } */
	acpigen_write_store();
	acpigen_write_one();
	acpigen_emit_byte(LOCAL2_OP);
	acpigen_pop_len(); /* If */

	/*
	 * Local1++
	 * } # End of While
	 */
	acpigen_emit_byte(INCREMENT_OP);
	acpigen_emit_byte(LOCAL1_OP);
	acpigen_pop_len(); /* While */

	/*
	 * If (Local2 == 1)
	 */
	acpigen_write_if();
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_byte(LOCAL2_OP);
	acpigen_write_one();

	/* caller must insert acpigen_pop_len() ! */
}

static void write_dptf_OSC(const struct device *ec)
{
	char name[16];
	int i;

	/*
	 * Arg0: Buffer containing UUID
	 * Arg1: "Integer containing Revision ID of buffer format", but Linux passes whether
	 *     it is enabling (1) or disabling (0) the policy in Arg1.
	 * Arg2: Integer containing count of entries in Arg3
	 * Arg3: Buffer containing list of DWORD capabilities
	 * Return: Buffer containing list of DWORD capabilities
	 */
	acpigen_write_method_serialized("_OSC", 4);

	/*
	 * If the Passive Policy is enabled:
	 * 1) Disable temperature sensor trip points in the EC (replaces TINI)
	 * 2) Disable the charge limit in the EC (replaces TCHG.INIT)
	 */
	write_is_policy_enabled(true);
	for (i = 0; i < DPTF_MAX_TSR; ++i) {
		snprintf(name, sizeof(name), "^TSR%1d.PATD", i);
		acpigen_emit_namestring(name);
	}

	acpigen_emit_namestring(acpi_device_path_join(ec, "CHGD"));
	acpigen_pop_len(); /* If (from write_is_policy_enabled) */

	/* If the Active Policy is disabled, disable DPTF fan control in the EC */
	write_is_policy_enabled(false);
	acpigen_write_store();
	acpigen_write_integer(EC_FAN_DUTY_AUTO);
	acpigen_emit_namestring(acpi_device_path_join(ec, "FAND"));
	acpigen_pop_len(); /* If (from write_is_policy_enabled) */

	acpigen_write_return_op(ARG3_OP);
	acpigen_pop_len(); /* Method _OSC */
}

static void write_dppm_methods(const struct device *ec)
{
	enum dptf_participant p;
	char name[16];
	int i;

	acpigen_write_scope("\\_SB.DPTF");
	write_dptf_OSC(ec);

	/* TEVT */
	if (CONFIG(EC_SUPPORTS_DPTF_TEVT)) {
		acpigen_write_method("TEVT", 1);

		/* Local0 = ToInteger(Arg0) */
		acpigen_write_to_integer(ARG0_OP, LOCAL0_OP);
		for (p = DPTF_TEMP_SENSOR_0, i = 0; p <= DPTF_TEMP_SENSOR_4; ++p, ++i) {
			snprintf(name, sizeof(name), "^TSR%1d", i);
			acpigen_write_if_lequal_op_int(LOCAL0_OP, i);
			acpigen_notify(name, THERMAL_EVENT);
			acpigen_pop_len(); /* If */
		}

		acpigen_pop_len(); /* Method */
	}

	/* TPET */
	acpigen_write_method("TPET", 0);
	for (p = DPTF_TEMP_SENSOR_0, i = 0; p <= DPTF_TEMP_SENSOR_4; ++p, ++i) {
		snprintf(name, sizeof(name), "^TSR%1d", i);
		acpigen_notify(name, TRIP_POINTS_CHANGED_EVENT);
	}

	acpigen_pop_len(); /* Method */
	acpigen_pop_len(); /* Scope */
}

static void write_charger_methods(const struct device *ec)
{
	dptf_write_scope(DPTF_CHARGER);
	write_charger_PPPC(ec);
	write_charger_SPPC(ec);
	acpigen_pop_len(); /* Scope */
}

static void write_fan_methods(const struct device *ec)
{
	dptf_write_scope(DPTF_FAN);
	write_fan_fsl(ec);
	write_fan_fst(ec);
	acpigen_pop_len(); /* Scope */
}

static void write_thermal_methods(const struct device *ec, enum dptf_participant participant,
				  int tsr_index)
{
	dptf_write_scope(participant);

	/*
	 * GTSH - Amount of hysteresis inherent in temperature reading (2 degrees, in units of
	 * 1/10th degree K)
	 */
	acpigen_write_name_integer("GTSH", 20);

	/* _TMP - read temperature from EC */
	acpigen_write_method_serialized("_TMP", 0);
	acpigen_emit_byte(RETURN_OP);
	acpigen_emit_namestring(acpi_device_path_join(ec, "TSRD"));
	acpigen_write_integer(tsr_index);
	acpigen_pop_len(); /* Method _TMP */

	/* PATC - Aux trip point count */
	acpigen_write_name_integer("PATC", 2);

	/* PAT0 - Set Aux trip point 0 */
	acpigen_write_method_serialized("PAT0", 1);
	acpigen_emit_namestring(acpi_device_path_join(ec, "PAT0"));
	acpigen_write_integer(tsr_index);
	acpigen_emit_byte(ARG0_OP);
	acpigen_pop_len(); /* Method PAT0 */

	/* PAT1 - Set Aux trip point 1 */
	acpigen_write_method_serialized("PAT1", 1);
	acpigen_emit_namestring(acpi_device_path_join(ec, "PAT1"));
	acpigen_write_integer(tsr_index);
	acpigen_emit_byte(ARG0_OP);
	acpigen_pop_len(); /* Method PAT0 */

	/* PATD - Disable Aux trip point */
	acpigen_write_method_serialized("PATD", 0);
	acpigen_emit_namestring(acpi_device_path_join(ec, "PATD"));
	acpigen_write_integer(tsr_index);
	acpigen_pop_len(); /* Method PAT0 */

	acpigen_pop_len(); /* Scope */
}

void ec_fill_dptf_helpers(const struct device *ec)
{
	enum dptf_participant p;
	int i;

	write_dppm_methods(ec);
	write_charger_methods(ec);
	write_fan_methods(ec);

	for (p = DPTF_TEMP_SENSOR_0, i = 0; p <= DPTF_TEMP_SENSOR_4; ++p, ++i)
		write_thermal_methods(ec, p, i);
}
