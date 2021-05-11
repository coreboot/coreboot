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

void acpigen_write_alib_dptc(uint8_t *default_param, size_t default_param_len,
	uint8_t *tablet_param, size_t tablet_param_len)
{
	/* Scope (\_SB) */
	acpigen_write_scope("\\_SB");

	/* Method(DPTC, 0, Serialized) */
	acpigen_write_method_serialized("DPTC", 0);

	/* TODO: The code assumes that if DPTC gets called the following object exists */
	/* If (LEqual ("\_SB.PCI0.LPCB.EC0.TBMD", 1)) */
	acpigen_write_if_lequal_namestr_int("\\_SB.PCI0.LPCB.EC0.TBMD", 1);

	acpigen_dptc_call_alib("TABB", tablet_param, tablet_param_len);

	/* Else */
	acpigen_write_else();

	acpigen_dptc_call_alib("DEFB", default_param, default_param_len);

	acpigen_pop_len(); /* Else */

	acpigen_pop_len(); /* Method DPTC */
	acpigen_pop_len(); /* Scope \_SB */
}
