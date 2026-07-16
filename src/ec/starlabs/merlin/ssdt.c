/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <device/device.h>

#if CONFIG(SYSTEM_TYPE_DETACHABLE)
#include <soc/gpio.h>
#endif
#if CONFIG(STARLABS_ACPI_EFI_OPTION_SMI)
#include <starlabs/efi_option_smi.h>
#endif

#include "ec.h"

#define EC_ACPI_PATH           "\\_SB.PCI0.LPCB.EC"
#define EC_ACPI_METHOD(method) EC_ACPI_PATH "." method
#define EC_ACPI_FIELD(field)   EC_ACPI_PATH "." field

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

static void write_ec_read_store_op(const char *field, uint8_t destination)
{
	acpigen_emit_byte(STORE_OP);
	write_ec_read(field);
	acpigen_emit_byte(destination);
}

static void write_method_store_op(const char *method, uint8_t destination)
{
	acpigen_emit_byte(STORE_OP);
	acpigen_emit_namestring(method);
	acpigen_emit_byte(destination);
}

static void write_package_target(const char *package, unsigned int element)
{
	acpigen_emit_byte(INDEX_OP);
	acpigen_emit_namestring(package);
	acpigen_write_integer(element);
	acpigen_emit_byte(ZERO_OP);
}

static void write_package_store_ec_read(const char *package, unsigned int element,
					const char *field)
{
	acpigen_emit_byte(STORE_OP);
	write_ec_read(field);
	write_package_target(package, element);
}

static void write_package_store_op(const char *package, unsigned int element, uint8_t source)
{
	acpigen_emit_byte(STORE_OP);
	acpigen_emit_byte(source);
	write_package_target(package, element);
}

static void write_package_store_divide(const char *package, unsigned int element,
				       uint8_t source, uint64_t divisor)
{
	acpigen_emit_byte(DIVIDE_OP);
	acpigen_emit_byte(source);
	acpigen_write_integer(divisor);
	acpigen_emit_byte(ZERO_OP);
	write_package_target(package, element);
}

static void write_package_store_divide_add(const char *package, unsigned int element,
					   uint8_t source, uint64_t addend, uint64_t divisor)
{
	acpigen_emit_byte(DIVIDE_OP);
	acpigen_emit_byte(ADD_OP);
	acpigen_emit_byte(source);
	acpigen_write_integer(addend);
	acpigen_emit_byte(ZERO_OP);
	acpigen_write_integer(divisor);
	acpigen_emit_byte(ZERO_OP);
	write_package_target(package, element);
}

static void write_serialized_method_return_name(const char *method, int args, const char *value)
{
	acpigen_write_method_serialized(method, args);
	acpigen_write_return_namestr(value);
	acpigen_write_method_end();
}

static void write_serialized_method_return_integer(const char *method, uint64_t value)
{
	acpigen_write_method_serialized(method, 0);
	acpigen_write_return_integer(value);
	acpigen_write_method_end();
}

static void write_method_call(const char *method)
{
	acpigen_emit_namestring(method);
}

static void write_method_call_package_element(const char *method, unsigned int element)
{
	write_method_call(method);
	acpigen_emit_byte(DEREF_OP);
	acpigen_emit_byte(INDEX_OP);
	acpigen_emit_byte(ARG3_OP);
	acpigen_write_integer(element);
	acpigen_emit_byte(ZERO_OP);
}

static void write_ec_write_integer(uint64_t value, const char *field)
{
	write_method_call(EC_ACPI_METHOD("ECWR"));
	acpigen_write_integer(value);
	acpigen_emit_byte(REF_OF_OP);
	acpigen_emit_namestring(field);
}

static void write_hid_dsm_index(unsigned int index)
{
	acpigen_write_if();
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_byte(TO_INTEGER_OP);
	acpigen_emit_byte(ARG2_OP);
	acpigen_emit_byte(ZERO_OP);
	acpigen_write_integer(index);
}

static void write_hid_dsm(void)
{
	static uint8_t supported_functions[] = {0xff, 0x03};

	acpigen_write_method_serialized("_DSM", 4);
	acpigen_write_if();
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_byte(ARG0_OP);
	acpigen_write_uuid("EEEC56B3-4442-408F-A792-4EDD4D758054");
	acpigen_write_if();
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_write_integer(1);
	acpigen_emit_byte(TO_INTEGER_OP);
	acpigen_emit_byte(ARG1_OP);
	acpigen_emit_byte(ZERO_OP);

	write_hid_dsm_index(0);
	acpigen_write_return_byte_buffer(supported_functions, ARRAY_SIZE(supported_functions));
	acpigen_write_if_end();

	write_hid_dsm_index(1);
	write_method_call("BTNL");
	acpigen_write_if_end();

	write_hid_dsm_index(2);
	acpigen_emit_byte(RETURN_OP);
	write_method_call("HDMM");
	acpigen_write_if_end();

	write_hid_dsm_index(3);
	write_method_call_package_element("HDSM", 0);
	acpigen_write_if_end();

	write_hid_dsm_index(4);
	acpigen_emit_byte(RETURN_OP);
	write_method_call("HDEM");
	acpigen_write_if_end();

	write_hid_dsm_index(5);
	acpigen_emit_byte(RETURN_OP);
	write_method_call("BTNS");
	acpigen_write_if_end();

	write_hid_dsm_index(6);
	write_method_call_package_element("BTNE", 0);
	acpigen_write_if_end();

	write_hid_dsm_index(7);
	acpigen_emit_byte(RETURN_OP);
	write_method_call("HEBC");
	acpigen_write_if_end();

	write_hid_dsm_index(8);
	if (CONFIG(SYSTEM_TYPE_DETACHABLE)) {
		acpigen_emit_byte(RETURN_OP);
		write_method_call("\\_SB.PCI0.LPCB.EC.VBTN.VGBS");
	} else {
		acpigen_write_return_integer(0);
	}
	acpigen_write_if_end();

	write_hid_dsm_index(9);
	acpigen_emit_byte(RETURN_OP);
	write_method_call("H2BC");
	acpigen_write_if_end();

	acpigen_write_if_end();
	acpigen_write_if_end();
	acpigen_write_return_singleton_buffer(0);
	acpigen_write_method_end();
}

static void write_hid_event_method(void)
{
	acpigen_write_method_serialized("HPEM", 1);
	acpigen_write_store_int_to_namestr(1, "HBSY");
	acpigen_write_store_op_to_namestr(ARG0_OP, "HIDX");
	acpigen_notify("\\_SB.HIDD", 0xc0);
	acpigen_write_store_int_to_op(0, LOCAL0_OP);

	acpigen_emit_byte(WHILE_OP);
	acpigen_write_len_f();
	acpigen_emit_byte(LAND_OP);
	acpigen_emit_byte(LLESS_OP);
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_write_integer(250);
	acpigen_emit_namestring("HBSY");
	acpigen_write_sleep(4);
	acpigen_emit_byte(INCREMENT_OP);
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_pop_len();

	acpigen_write_if_lequal_namestr_int("HBSY", 1);
	acpigen_write_store_int_to_namestr(0, "HBSY");
	acpigen_write_store_int_to_namestr(0, "HIDX");
	acpigen_write_return_integer(1);
	acpigen_write_else();
	acpigen_write_return_integer(0);
	acpigen_write_if_end();
	acpigen_write_method_end();
}

static void write_hid_device(void)
{
	acpigen_write_device("HIDD");
	acpigen_write_name_string("_HID", "INTC1051");
	acpigen_write_name_integer("HBSY", 0);
	acpigen_write_name_integer("HIDX", 0);
	acpigen_write_name_integer("HMDE", 0);
	acpigen_write_name_integer("HRDY", 0);
	acpigen_write_name_integer("BTLD", 0);
	acpigen_write_name_integer("BTS1", 0);

	acpigen_write_method_serialized("_STA", 0);
	acpigen_write_return_integer(0x0f);
	acpigen_write_method_end();

	acpigen_write_name("DPKG");
	acpigen_write_package(4);
	acpigen_write_dword(0x11111111);
	acpigen_write_dword(0x22222222);
	acpigen_write_dword(0x33333333);
	acpigen_write_dword(0x44444444);
	acpigen_write_package_end();

	write_serialized_method_return_name("HDDM", 0, "DPKG");

	acpigen_write_method_serialized("HDEM", 0);
	acpigen_write_store_int_to_namestr(0, "HBSY");
	acpigen_write_if_lequal_namestr_int("HMDE", 0);
	acpigen_write_return_namestr("HIDX");
	acpigen_write_if_end();
	acpigen_write_return_namestr("HMDE");
	acpigen_write_method_end();

	write_serialized_method_return_name("HDMM", 0, "HMDE");

	acpigen_write_method_serialized("HDSM", 1);
	acpigen_write_store_op_to_namestr(ARG0_OP, "HRDY");
	acpigen_write_method_end();

	write_hid_event_method();

	acpigen_write_method_serialized("BTNL", 0);
	acpigen_write_store_int_to_namestr(0, "BTS1");
	acpigen_write_method_end();
	write_serialized_method_return_name("BTNE", 1, "BTS1");
	write_serialized_method_return_name("BTNS", 0, "BTS1");

	acpigen_write_method_serialized("BTNC", 0);
	acpigen_write_return_integer(0x1f);
	acpigen_write_method_end();

	acpigen_write_name_integer("HEB2", 0);
	write_serialized_method_return_integer("HEBC", 0);
	write_serialized_method_return_integer("H2BC", 0);
	write_serialized_method_return_integer("HEEC", 0);

	write_hid_dsm();
	acpigen_write_device_end();
}

static void write_hid_query_events(void)
{
	acpigen_write_method("_Q05", 0);
	write_method_call("\\_SB.HIDD.HPEM");
	acpigen_write_integer(20);
	acpigen_write_method_end();

	acpigen_write_method("_Q06", 0);
	write_method_call("\\_SB.HIDD.HPEM");
	acpigen_write_integer(19);
	acpigen_write_method_end();
}

#if CONFIG(SYSTEM_TYPE_DETACHABLE)
static void write_virtual_button_devices(void)
{
	acpigen_write_device("VBTN");
	acpigen_write_name_string("_HID", "INT33D6");
	acpigen_write_name_integer("_UID", 1);
	acpigen_write_name_string("_DDN", "Intel Virtual Button Driver");
	acpigen_write_STA(0x0f);

	acpigen_write_method("VBDL", 0);
	acpigen_write_method_end();

	acpigen_write_method_serialized("UPDK", 0);
	write_method_store_op("VGBS", LOCAL0_OP);
	acpigen_write_if_lequal_op_int(LOCAL0_OP, 0);
	acpigen_write_debug_string("Tablet Mode");
	acpigen_notify("\\_SB.HIDD", 0xcc);
	acpigen_write_else();
	acpigen_write_debug_string("Docked");
	acpigen_notify("\\_SB.HIDD", 0xcd);
	acpigen_write_if_end();
	acpigen_write_return_op(LOCAL0_OP);
	acpigen_write_method_end();

	acpigen_write_method("VGBS", 0);
	acpigen_write_if();
	acpigen_emit_byte(LNOT_OP);
	write_method_call("\\_SB.PCI0.GRXS");
	acpigen_write_integer(GPP_F15);
	acpigen_write_return_integer(0x40);
	acpigen_write_if_end();
	acpigen_write_return_integer(0);
	acpigen_write_method_end();
	acpigen_write_device_end();

	acpigen_write_device("VBTO");
	acpigen_write_name_string("_HID", "INT33D3");
	acpigen_write_name_string("_CID", "PNP0C60");
	acpigen_write_name_integer("_UID", 1);
	acpigen_write_name_string("_DDN", "Laptop/tablet mode indicator driver");
	acpigen_write_STA(0x0f);
	acpigen_write_device_end();
}
#endif

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

static void write_battery_packages(void)
{
	acpigen_write_name("SBIF");
	acpigen_write_package(13);
	acpigen_write_integer(1);
	acpigen_write_dword(0xffffffff);
	acpigen_write_dword(0xffffffff);
	acpigen_write_integer(1);
	acpigen_write_dword(0xffffffff);
	acpigen_write_integer(0);
	acpigen_write_integer(0);
	acpigen_write_dword(0xffffffff);
	acpigen_write_dword(0xffffffff);
	acpigen_write_string(CONFIG_EC_STARLABS_BATTERY_MODEL);
	acpigen_write_string("Unknown");
	acpigen_write_string(CONFIG_EC_STARLABS_BATTERY_TYPE);
	acpigen_write_string(CONFIG_EC_STARLABS_BATTERY_OEM);
	acpigen_write_package_end();

	acpigen_write_name("XBIF");
	acpigen_write_package(21);
	acpigen_write_integer(1);
	acpigen_write_integer(1);
	acpigen_write_dword(0xffffffff);
	acpigen_write_dword(0xffffffff);
	acpigen_write_integer(1);
	for (int i = 0; i < 4; i++)
		acpigen_write_dword(0xffffffff);
	acpigen_write_integer(2);
	acpigen_write_integer(5000);
	acpigen_write_integer(1000);
	acpigen_write_integer(5000);
	acpigen_write_integer(1000);
	acpigen_write_dword(0xffffffff);
	acpigen_write_dword(0xffffffff);
	acpigen_write_string(CONFIG_EC_STARLABS_BATTERY_MODEL);
	acpigen_write_string("Unknown");
	acpigen_write_string(CONFIG_EC_STARLABS_BATTERY_TYPE);
	acpigen_write_string(CONFIG_EC_STARLABS_BATTERY_OEM);
	acpigen_write_integer(1);
	acpigen_write_package_end();

	acpigen_write_name("PKG1");
	acpigen_write_package(4);
	for (int i = 0; i < 4; i++)
		acpigen_write_dword(0xffffffff);
	acpigen_write_package_end();
}

static void write_battery_full_capacity(void)
{
	acpigen_write_method("BFCX", 0);
	write_ec_read_store_op("B1FC", LOCAL0_OP);

	acpigen_write_if();
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_write_if_lnotequal_op_int(LOCAL0_OP, 0xffff);
	acpigen_write_return_op(LOCAL0_OP);
	acpigen_write_if_end();
	acpigen_write_if_end();

	acpigen_emit_byte(RETURN_OP);
	write_ec_read("B1DC");
	acpigen_write_method_end();
}

static void write_battery_info(void)
{
	acpigen_write_method("_BIF", 0);
	write_ec_read_store_op("B1DC", LOCAL0_OP);
	acpigen_write_if();
	acpigen_emit_byte(LOCAL0_OP);
	write_method_store_op("BFCX", LOCAL1_OP);
	write_package_store_op("SBIF", 1, LOCAL0_OP);
	write_package_store_op("SBIF", 2, LOCAL1_OP);
	write_package_store_ec_read("SBIF", 4, "B1DV");
	write_package_store_divide_add("SBIF", 5, LOCAL1_OP, 2, 5);
	write_package_store_divide_add("SBIF", 6, LOCAL1_OP, 5, 10);
	write_package_store_divide("SBIF", 7, LOCAL1_OP, 500);
	write_package_store_divide("SBIF", 8, LOCAL1_OP, 500);
	acpigen_write_if_end();
	acpigen_write_return_namestr("SBIF");
	acpigen_write_method_end();

	acpigen_write_method("_BIX", 0);
	write_ec_read_store_op("B1DC", LOCAL0_OP);
	acpigen_write_if();
	acpigen_emit_byte(LOCAL0_OP);
	write_method_store_op("BFCX", LOCAL1_OP);
	write_package_store_op("XBIF", 2, LOCAL0_OP);
	write_package_store_op("XBIF", 3, LOCAL1_OP);
	write_package_store_ec_read("XBIF", 5, "B1DV");
	write_package_store_divide_add("XBIF", 6, LOCAL1_OP, 2, 5);
	write_package_store_divide_add("XBIF", 7, LOCAL1_OP, 5, 10);
	acpigen_write_if_lnotequal_namestr_int("B1CC", 0xffff);
	write_package_store_ec_read("XBIF", 8, "B1CC");
	acpigen_write_if_end();
	write_package_store_divide("XBIF", 14, LOCAL1_OP, 500);
	write_package_store_divide("XBIF", 15, LOCAL1_OP, 500);
	acpigen_write_if_end();
	acpigen_write_return_namestr("XBIF");
	acpigen_write_method_end();
}

static void write_battery_status(void)
{
	acpigen_write_method("_BST", 0);

	acpigen_emit_byte(AND_OP);
	write_ec_read("B1ST");
	acpigen_write_integer(0x0f);
	write_package_target("PKG1", 0);
	write_package_store_ec_read("PKG1", 1, "B1PR");
	acpigen_set_package_element_int("PKG1", 2, 0xffffffff);
	acpigen_write_store_int_to_op(0, LOCAL2_OP);

	write_ec_read_store_op("B1RP", LOCAL0_OP);
	acpigen_write_if();
	acpigen_emit_byte(LNOT_OP);
	acpigen_emit_byte(LGREATER_OP);
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_write_integer(100);
	write_method_store_op("BFCX", LOCAL1_OP);
	acpigen_write_if();
	acpigen_emit_byte(LOCAL1_OP);
	acpigen_write_if_lnotequal_op_int(LOCAL1_OP, 0xffff);
	acpigen_emit_byte(DIVIDE_OP);
	acpigen_emit_byte(ADD_OP);
	acpigen_emit_byte(MULTIPLY_OP);
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_emit_byte(LOCAL1_OP);
	acpigen_emit_byte(ZERO_OP);
	acpigen_write_integer(50);
	acpigen_emit_byte(ZERO_OP);
	acpigen_write_integer(100);
	acpigen_emit_byte(ZERO_OP);
	write_package_target("PKG1", 2);
	acpigen_write_store_int_to_op(1, LOCAL2_OP);
	acpigen_write_if_end();
	acpigen_write_if_end();
	acpigen_write_if_end();

	acpigen_write_if_lequal_op_int(LOCAL2_OP, 0);
	write_ec_read_store_op("B1RC", LOCAL0_OP);
	acpigen_write_if_lnotequal_op_int(LOCAL0_OP, 0xffff);
	write_method_store_op("BFCX", LOCAL1_OP);
	acpigen_write_if();
	acpigen_emit_byte(LOCAL1_OP);
	acpigen_write_if_lnotequal_op_int(LOCAL1_OP, 0xffff);
	acpigen_write_if_lgreater_op_op(LOCAL0_OP, LOCAL1_OP);
	acpigen_write_store_ops(LOCAL1_OP, LOCAL0_OP);
	acpigen_write_if_end();
	acpigen_write_if_end();
	acpigen_write_if_end();
	write_package_store_op("PKG1", 2, LOCAL0_OP);
	acpigen_write_if_end();
	acpigen_write_if_end();

	write_package_store_ec_read("PKG1", 3, "B1PV");
	acpigen_write_return_namestr("PKG1");
	acpigen_write_method_end();
}

static void write_battery(void)
{
	if (CONFIG(EC_STARLABS_MERLIN)) {
		acpigen_write_method("_Q09", 0);
		acpigen_notify("BAT0", 0x81);
		acpigen_write_method_end();
		acpigen_write_method("_Q0B", 0);
		acpigen_notify("BAT0", 0x80);
		acpigen_write_method_end();
	}

	acpigen_write_device("BAT0");
	acpigen_write_name("_HID");
	acpigen_emit_eisaid("PNP0C0A");
	acpigen_write_name_integer("_UID", 0);

	acpigen_write_method("_STA", 0);
	acpigen_write_if();
	acpigen_emit_byte(AND_OP);
	write_ec_read("ECPS");
	acpigen_write_integer(2);
	acpigen_emit_byte(ZERO_OP);
	acpigen_write_return_integer(0x1f);
	acpigen_write_if_end();
	acpigen_write_return_integer(0x0f);
	acpigen_write_method_end();

	write_battery_packages();
	write_battery_full_capacity();
	write_battery_info();
	write_battery_status();

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

#if CONFIG(STARLABS_ACPI_EFI_OPTION_SMI)
static void write_ec_read_path(const char *field)
{
	write_method_call(EC_ACPI_METHOD("ECRD"));
	acpigen_emit_byte(REF_OF_OP);
	acpigen_emit_namestring(field);
}

static void write_efi_option_get(const char *option)
{
	write_method_call("EOGT");
	acpigen_emit_namestring(option);
}

static void write_efi_option_set_ec_read(const char *option, const char *field)
{
	write_method_call("EOSV");
	acpigen_emit_namestring(option);
	write_ec_read_path(field);
}

static void write_efi_option_field(void)
{
	static const struct fieldlist fields[] = {
		FIELDLIST_NAMESTR("EOCM", 32),
		FIELDLIST_NAMESTR("EOID", 32),
		FIELDLIST_NAMESTR("EOVL", 32),
		FIELDLIST_NAMESTR("EORS", 32),
	};

	acpigen_emit_ext_op(FIELD_OP);
	acpigen_write_len_f();
	acpigen_emit_namestring("\\DNVS");
	acpigen_emit_byte(FIELD_BYTEACC | FIELD_NOLOCK | FIELD_PRESERVE);
	for (size_t i = 0; i < ARRAY_SIZE(fields); i++)
		acpigen_write_field_name(fields[i].name, fields[i].bits);
	acpigen_pop_len();
}

static void write_efi_option_methods(void)
{
	write_efi_option_field();
	acpigen_write_name_integer("EOAP", STARLABS_APMC_CMD_EFI_OPTION);
	acpigen_write_name_integer("EOFL", STARLABS_EFIOPT_ID_FN_LOCK_STATE);
	acpigen_write_name_integer("EOTP", STARLABS_EFIOPT_ID_TRACKPAD_STATE);
	acpigen_write_name_integer("EOKB", STARLABS_EFIOPT_ID_KBL_BRIGHTNESS);
	acpigen_write_name_integer("EOKS", STARLABS_EFIOPT_ID_KBL_STATE);
	acpigen_write_mutex("EOMX", 0);

	acpigen_write_method_serialized("EOGT", 1);
	acpigen_write_if();
	acpigen_write_acquire("EOMX", 1000);
	acpigen_write_return_integer(0xffffffff);
	acpigen_write_if_end();
	acpigen_write_store_int_to_namestr(1, "EOCM");
	acpigen_write_store_op_to_namestr(ARG0_OP, "EOID");
	acpigen_write_store_int_to_namestr(0, "EORS");
	acpigen_write_store_namestr_to_namestr("EOAP", EC_ACPI_FIELD("SMB2"));
	acpigen_write_store_namestr_to_op("EOVL", LOCAL0_OP);
	acpigen_write_release("EOMX");
	acpigen_write_return_op(LOCAL0_OP);
	acpigen_write_method_end();

	acpigen_write_method_serialized("EOSV", 2);
	acpigen_write_if();
	acpigen_write_acquire("EOMX", 1000);
	acpigen_write_return_integer(1);
	acpigen_write_if_end();
	acpigen_write_store_int_to_namestr(2, "EOCM");
	acpigen_write_store_op_to_namestr(ARG0_OP, "EOID");
	acpigen_write_store_op_to_namestr(ARG1_OP, "EOVL");
	acpigen_write_store_int_to_namestr(0, "EORS");
	acpigen_write_store_namestr_to_namestr("EOAP", EC_ACPI_FIELD("SMB2"));
	acpigen_write_store_namestr_to_op("EORS", LOCAL0_OP);
	acpigen_write_release("EOMX");
	acpigen_write_return_op(LOCAL0_OP);
	acpigen_write_method_end();
}

static void write_efi_option_suspend(void)
{
	acpigen_emit_byte(STORE_OP);
	write_ec_read_path(EC_ACPI_FIELD("TPLE"));
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_write_if_lequal_op_int(LOCAL0_OP, 0x11);
	acpigen_write_store_int_to_op(0, LOCAL0_OP);
	acpigen_write_if_end();
	write_method_call("EOSV");
	acpigen_emit_namestring("EOTP");
	acpigen_emit_byte(LOCAL0_OP);
	write_efi_option_set_ec_read("EOFL", EC_ACPI_FIELD("FLKE"));
	write_efi_option_set_ec_read("EOKS", EC_ACPI_FIELD("KLSE"));
	write_efi_option_set_ec_read("EOKB", EC_ACPI_FIELD("KLBE"));
}

static void write_efi_option_restore_integer(const char *field, uint64_t valid_value)
{
	acpigen_write_if_lequal_op_int(LOCAL0_OP, valid_value);
	write_ec_write_integer(valid_value, field);
	acpigen_write_else();
	write_ec_write_integer(0, field);
	acpigen_write_if_end();
}

static void write_efi_option_resume(void)
{
	static const uint8_t brightness_values[] = {0xdd, 0xcc, 0xbb, 0xaa};

	acpigen_emit_byte(STORE_OP);
	write_efi_option_get("EOTP");
	acpigen_emit_byte(LOCAL0_OP);
	write_efi_option_restore_integer(EC_ACPI_FIELD("TPLE"), 0x22);

	write_method_call(EC_ACPI_METHOD("ECWR"));
	write_efi_option_get("EOFL");
	acpigen_emit_byte(REF_OF_OP);
	acpigen_emit_namestring(EC_ACPI_FIELD("FLKE"));

	acpigen_emit_byte(STORE_OP);
	write_efi_option_get("EOKS");
	acpigen_emit_byte(LOCAL0_OP);
	write_efi_option_restore_integer(EC_ACPI_FIELD("KLSE"), 0xdd);

	acpigen_emit_byte(STORE_OP);
	write_efi_option_get("EOKB");
	acpigen_emit_byte(LOCAL0_OP);
	for (size_t i = 0; i < ARRAY_SIZE(brightness_values); i++) {
		acpigen_write_if_lequal_op_int(LOCAL0_OP, brightness_values[i]);
		write_ec_write_integer(brightness_values[i], EC_ACPI_FIELD("KLBE"));
		acpigen_write_if_end();
	}
}
#endif

static void write_sleep_methods(void)
{
#if CONFIG(STARLABS_ACPI_EFI_OPTION_SMI)
	write_efi_option_methods();
#endif

	acpigen_write_method_serialized("RPTS", 1);
#if CONFIG(STARLABS_ACPI_EFI_OPTION_SMI)
	write_efi_option_suspend();
#endif
	write_ec_write_integer(0, EC_ACPI_FIELD("OSFG"));
	acpigen_write_return_op(ARG0_OP);
	acpigen_write_method_end();

	acpigen_write_method_serialized("RWAK", 1);
	write_ec_write_integer(1, EC_ACPI_FIELD("OSFG"));
#if CONFIG(STARLABS_ACPI_EFI_OPTION_SMI)
	write_efi_option_resume();
#endif
	acpigen_write_return_op(ARG0_OP);
	acpigen_write_method_end();
}

void merlin_fill_ssdt(const struct device *dev)
{
	(void)dev;

	acpigen_write_scope(EC_ACPI_PATH);
	if (CONFIG(EC_STARLABS_MERLIN))
		write_hid_query_events();
#if CONFIG(SYSTEM_TYPE_DETACHABLE)
	write_virtual_button_devices();
#endif
	write_ac_adapter();
	if (CONFIG(EC_STARLABS_MERLIN))
		write_shutdown_event();
	if (CONFIG(SYSTEM_TYPE_LAPTOP) || CONFIG(SYSTEM_TYPE_DETACHABLE)) {
		write_battery();
		write_lid();
	}
	acpigen_write_scope_end();

	acpigen_write_scope("\\_SB");
	write_hid_device();
	write_sleep_methods();
	acpigen_write_scope_end();
}
