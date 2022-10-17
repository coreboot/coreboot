/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <amdblocks/alib.h>
#include <types.h>

static void acpigen_dptc_call_alib(const char *buf_name, uint8_t *buffer, size_t size)
{
	/* Name (buf_name, Buffer(size) {...} */
	acpigen_write_name(buf_name);
	acpigen_write_byte_buffer(buffer, size);

	/* \_SB.ALIB(0xc, buf_name) */
	acpigen_emit_namestring("\\_SB.ALIB");
	acpigen_write_integer(ALIB_FUNCTION_DYNAMIC_POWER_THERMAL_CONFIG);
	acpigen_emit_namestring(buf_name);
}

void acpigen_write_alib_dptc_default(uint8_t *default_param, size_t default_param_len)
{
	/* Scope (\_SB) */
	acpigen_write_scope("\\_SB");

	/* Default (Unthrottled) Mode */
	/* Scope (\_SB)
	 * {
	 *     Method (DDEF, 0, Serialized)
	 *     {
	 *         Debug = "DPTC: Using normal SOC DPTC Settings."
	 *         Name (DEFB, Buffer (0x25)
	 *         {
	 *             ...
	 *         })
	 *         \_SB.ALIB
	 *         0x0C
	 *         DEFB
	 *     }
	 * }
	 */
	acpigen_write_method_serialized("DDEF", 0);
	acpigen_write_debug_string("DPTC: Using normal SOC DPTC Settings.");
	acpigen_dptc_call_alib("DEFB", default_param, default_param_len);
	acpigen_write_method_end();

	acpigen_write_scope_end();
}

void acpigen_write_alib_dptc_no_battery(uint8_t *no_battery_param, size_t no_battery_param_len)
{
	/* Scope (\_SB) */
	acpigen_write_scope("\\_SB");

	/* Low/No Battery Mode */
	/* Scope (\_SB)
	 * {
	 *     Method (DTHL, 0, Serialized)
	 *     {
	 *         Debug = "DPTC: Using low/no battery mode SOC DPTC settings."
	 *         Name (THTL, Buffer (0x25)
	 *         {
	 *             ...
	 *         })
	 *         \_SB.ALIB
	 *         0x0C
	 *         THTL
	 *     }
	 * }
	 */
	acpigen_write_method_serialized("DTHL", 0);
	acpigen_write_debug_string("DPTC: Using low/no battery mode SOC DPTC settings.");
	acpigen_dptc_call_alib("THTL", no_battery_param, no_battery_param_len);
	acpigen_write_method_end();

	acpigen_write_scope_end();
}

void acpigen_write_alib_dptc_tablet(uint8_t *tablet_param, size_t tablet_param_len)
{
	/* Scope (\_SB) */
	acpigen_write_scope("\\_SB");

	/* Tablet Mode */
	/* Scope (\_SB)
	 * {
	 *     Method (DTAB, 0, Serialized)
	 *     {
	 *         Debug = "DPTC: Using tablet mode SOC DPTC Settings."
	 *         Name (TABB, Buffer (0x25)
	 *         {
	 *             ...
	 *         })
	 *         \_SB.ALIB
	 *         0x0C
	 *         TABB
	 *     }
	 * }
	 */
	acpigen_write_method_serialized("DTAB", 0);
	acpigen_write_debug_string("DPTC: Using tablet mode SOC DPTC Settings.");
	acpigen_dptc_call_alib("TABB", tablet_param, tablet_param_len);
	acpigen_write_method_end();

	acpigen_write_scope_end();
}

void acpigen_write_alib_dptc_thermal_B(uint8_t *thermal_param_B, size_t thermal_param_B_len)
{
	/* Scope (\_SB) */
	acpigen_write_scope("\\_SB");

	/* Table B for dynamic DPTC*/
	/* Scope (\_SB)
	 * {
	 *     Method (DTTB, 0, Serialized)
	 *     {
	 *         Debug = "DPTC: TABLE B"
	 *         Name (THTB, Buffer (0x25)
	 *         {
	 *             ...
	 *         })
	 *         \_SB.ALIB
	 *         0x0C
	 *         THTB
	 *     }
	 * }
	 */
	acpigen_write_method_serialized("DTTB", 0);
	acpigen_write_debug_string("DPTC: TABLE B");
	acpigen_dptc_call_alib("THTB", thermal_param_B, thermal_param_B_len);
	acpigen_write_method_end();

	acpigen_write_scope_end();
}

void acpigen_write_alib_dptc_thermal_C(uint8_t *thermal_param_C, size_t thermal_param_C_len)
{
	/* Scope (\_SB) */
	acpigen_write_scope("\\_SB");

	/* Table C for dynamic DPTC*/
	/* Scope (\_SB)
	 * {
	 *     Method (DTTC, 0, Serialized)
	 *     {
	 *         Debug = "DPTC: TABLE C"
	 *         Name (THTC, Buffer (0x25)
	 *         {
	 *             ...
	 *         })
	 *         \_SB.ALIB
	 *         0x0C
	 *         THTC
	 *     }
	 * }
	 */
	acpigen_write_method_serialized("DTTC", 0);
	acpigen_write_debug_string("DPTC: TABLE C");
	acpigen_dptc_call_alib("THTC", thermal_param_C, thermal_param_C_len);
	acpigen_write_method_end();

	acpigen_write_scope_end();
}

void acpigen_write_alib_dptc_thermal_D(uint8_t *thermal_param_D, size_t thermal_param_D_len)
{
	/* Scope (\_SB) */
	acpigen_write_scope("\\_SB");

	/* Table D for dynamic DPTC*/
	/* Scope (\_SB)
	 * {
	 *     Method (DTTD, 0, Serialized)
	 *     {
	 *         Debug = "DPTC: Using normal SOC DPTC Settings."
	 *         Name (THTD, Buffer (0x25)
	 *         {
	 *             ...
	 *         })
	 *         \_SB.ALIB
	 *         0x0C
	 *         THTD
	 *     }
	 * }
	 */
	acpigen_write_method_serialized("DTTD", 0);
	acpigen_write_debug_string("DPTC: TABLE D");
	acpigen_dptc_call_alib("THTD", thermal_param_D, thermal_param_D_len);
	acpigen_write_method_end();

	acpigen_write_scope_end();
}

void acpigen_write_alib_dptc_thermal_E(uint8_t *thermal_param_E, size_t thermal_param_E_len)
{
	/* Scope (\_SB) */
	acpigen_write_scope("\\_SB");

	/* Table E for dynamic DPTC*/
	/* Scope (\_SB)
	 * {
	 *     Method (DTTE, 0, Serialized)
	 *     {
	 *         Debug = "DPTC: TABLE E"
	 *         Name (THTE, Buffer (0x25)
	 *         {
	 *             ...
	 *         })
	 *         \_SB.ALIB
	 *         0x0C
	 *         THTE
	 *     }
	 * }
	 */
	acpigen_write_method_serialized("DTTE", 0);
	acpigen_write_debug_string("DPTC: TABLE E");
	acpigen_dptc_call_alib("THTE", thermal_param_E, thermal_param_E_len);
	acpigen_write_method_end();

	acpigen_write_scope_end();
}

void acpigen_write_alib_dptc_thermal_F(uint8_t *thermal_param_F, size_t thermal_param_F_len)
{
	/* Scope (\_SB) */
	acpigen_write_scope("\\_SB");

	/* Table F for dynamic DPTC*/
	/* Scope (\_SB)
	 * {
	 *     Method (DTTF, 0, Serialized)
	 *     {
	 *         Debug = "DPTC: TABLE F"
	 *         Name (THTF, Buffer (0x25)
	 *         {
	 *             ...
	 *         })
	 *         \_SB.ALIB
	 *         0x0C
	 *         THTF
	 *     }
	 * }
	 */
	acpigen_write_method_serialized("DTTF", 0);
	acpigen_write_debug_string("DPTC: TABLE F");
	acpigen_dptc_call_alib("THTF", thermal_param_F, thermal_param_F_len);
	acpigen_write_method_end();

	acpigen_write_scope_end();
}
