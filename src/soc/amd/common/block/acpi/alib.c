/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <amdblocks/alib.h>
#include <types.h>

void acpigen_dptc_call_alib(const char *buf_name, uint8_t *buffer, size_t size)
{
	/* Name (buf_name, Buffer(size) {...} */
	acpigen_write_name(buf_name);
	acpigen_write_byte_buffer(buffer, size);

	/* \_SB.ALIB(0xc, buf_name) */
	acpigen_emit_namestring("\\_SB.ALIB");
	acpigen_write_integer(ALIB_FUNCTION_DYNAMIC_POWER_THERMAL_CONFIG);
	acpigen_emit_namestring(buf_name);
}
