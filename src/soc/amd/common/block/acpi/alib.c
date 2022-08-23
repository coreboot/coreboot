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
