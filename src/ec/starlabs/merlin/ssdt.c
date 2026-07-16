/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <device/device.h>

#include "ec.h"

#define EC_ACPI_PATH "\\_SB.PCI0.LPCB.EC"

static void write_ec_read(const char *field)
{
	acpigen_emit_namestring("ECRD");
	acpigen_emit_byte(REF_OF_OP);
	acpigen_emit_namestring(field);
}

static void write_ec_read_store(const char *field, const char *destination)
{
	acpigen_emit_byte(STORE_OP);
	write_ec_read(field);
	acpigen_emit_namestring(destination);
}

static void write_ac_adapter(void)
{
	if (CONFIG(EC_STARLABS_MERLIN)) {
		acpigen_write_method("_Q0A", 0);
		acpigen_notify("ADP1", 0x80);
		acpigen_write_method_end();
	}

	acpigen_write_device("ADP1");
	acpigen_write_name_string("_HID", "ACPI0003");
	acpigen_write_STA(0x0f);

	acpigen_write_method("_PSR", 0);
	acpigen_emit_byte(AND_OP);
	write_ec_read("ECPS");
	acpigen_write_one();
	acpigen_emit_namestring("\\PWRS");
	acpigen_write_return_namestr("\\PWRS");
	acpigen_write_method_end();

	acpigen_write_method("_PCL", 0);
	acpigen_emit_byte(RETURN_OP);
	acpigen_write_package(1);
	acpigen_emit_namestring("\\_SB");
	acpigen_write_package_end();
	acpigen_write_method_end();
	acpigen_write_device_end();
}

static void write_lid(void)
{
	if (CONFIG(EC_STARLABS_MERLIN)) {
		acpigen_write_method("_Q0C", 0);
		write_ec_read_store("LSTE", "\\LIDS");
		acpigen_notify("LID0", 0x80);
		acpigen_write_method_end();
	}

	acpigen_write_device("LID0");
	acpigen_write_name("_HID");
	acpigen_emit_eisaid("PNP0C0D");
	acpigen_write_STA(0x0f);

	acpigen_write_method("_LID", 0);
	acpigen_emit_byte(RETURN_OP);
	write_ec_read("LSTE");
	acpigen_write_method_end();
	acpigen_write_device_end();
}

static void write_shutdown_event(void)
{
	acpigen_write_method("_Q3F", 0);
	acpigen_notify("\\_SB", 0x81);
	acpigen_write_method_end();
}

void merlin_fill_ssdt(const struct device *dev)
{
	(void)dev;

	acpigen_write_scope(EC_ACPI_PATH);
	write_ac_adapter();
	if (CONFIG(EC_STARLABS_MERLIN))
		write_shutdown_event();
	if (CONFIG(SYSTEM_TYPE_LAPTOP) || CONFIG(SYSTEM_TYPE_DETACHABLE))
		write_lid();
	acpigen_write_scope_end();
}
