/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/smm.h>
#include <ec/acpi/ec.h>
#include <soc/nvs.h>

/*
 * TODO: Perform RE of protocols in vendor firmware:
 * - gEfiSmmSxDispatch2ProtocolGuid
 * - gEfiSmmPowerButtonDispatch2ProtocolGuid
 *
 * However, note that first glance suggests that no handlers
 * will be very interesting and that gEfiSmmGpiDispatch2ProtocolGuid
 * was unused (as I recall).
 *
 * Also, consider gEfiSmmIoTrapDispatch2ProtocolGuid, but
 * this is less likely.
 */

/* Keep in sync with dsdt.asl; could insert into SSDT at runtime */
#define APM_CNT_BOARD_SMI	0xDD

/* Toggle TURBO_MODE_DISABLE bit in IA32_MISC_ENABLE MSR
   when requested by EC. */
static void toggle_turbo_disable(uint8_t function_parameter_0)
{
	if (function_parameter_0 == 1) {
		printk(BIOS_DEBUG, "EC: Enabling Intel Turbo Mode\n");
		msr_unset(IA32_MISC_ENABLE, 0x4000000000);
	} else if (function_parameter_0 == 0) {
		printk(BIOS_DEBUG, "EC: Disabling Intel Turbo Mode\n");
		msr_set(IA32_MISC_ENABLE, 0x4000000000);
	}
}

/* Set WiFi and BT enable bits in EC RAM. */
static void enable_rf_by_capability(void)
{
	/* FIXME: We're not tracking (driver) 'capabilities' at the moment (must we?),
	   so we just enable WiFi and BT here. If this was tracked, then
	   bits may be cleared here */
	uint8_t rf_register = ec_read(0x71);
	ec_write(0x71, rf_register | 0x03);
}

/* Set OS capability bits in EC RAM. */
static void handle_acpi_osys(void)
{
	uint8_t os_support;

	/* TODO: Add _OSI method support to coreboot and make this work */
	printk(BIOS_DEBUG, "GNVS.OSYS = %d\n", gnvs->unused_was_osys);
	switch (gnvs->unused_was_osys) {
	/* Linux */
	case 1000:
		os_support = 64;
		break;
	/* Windows versions by year */
	case 2009:
		os_support = 3;
		break;
	case 2012:
		os_support = 4;
		break;
	case 2013:
		os_support = 5;
		break;
	case 2015:
		os_support = 6;
		break;
	/* Operating system unknown */
	default:
		printk(BIOS_DEBUG, "GNVS.OSYS not supported!\n");
		printk(BIOS_DEBUG, "No capabilities!\n");
		os_support = 0;
		break;
	}

	ec_write(0x5C, os_support);
}

/* Handles EC's _REG, _PTS and _WAK methods.
   Partially involves setting EC RAM offsets based on GNVS.OSYS - OS capabilities? */
static void handle_acpi_wake_event(
	uint8_t function_parameter_0, uint8_t function_parameter_1)
{
	switch (function_parameter_0) {
	case 1:
		printk(BIOS_DEBUG, "EC: Called for _REG method - OS initialise\n");
		enable_rf_by_capability();
		handle_acpi_osys();
		// NOTE: Not handling (driver) 'capabilities'
		break;
	case 2:
		printk(BIOS_DEBUG, "EC: Called for _PTS method - Entering sleep\n");
		// NOTE: Not saving (driver) 'capabilities'
		// NOTE: Not saving and restoring EC RAM offset 0x4F
		break;
	case 3:
		printk(BIOS_DEBUG, "EC: Called for _WAK method - Sleep resume\n");
		enable_rf_by_capability();
		handle_acpi_osys();
		// NOTE: Not saving and restoring EC RAM offset 0x4F
		break;
	default:
		printk(BIOS_DEBUG, "function_parameter_0 is invalid!\n");
		break;
	}
}

/* TODO: Reverse engineer 0x80 function and implement if necessary */
static void ec_smi_handler(uint8_t smif)
{
	uint8_t smm_data_port;
	uint8_t function_parameter_0;
	uint8_t function_parameter_1;

	/* Parameters encoded onto SMI data port because PRMx NVS are not present
	   - Callers must only use 4 bits per argument
	   - _PTS and _WAK are required to call in spec-compliant way */
	smm_data_port = inb(APM_STS);
	function_parameter_0 = smm_data_port & ~0xF0;
	function_parameter_1 = smm_data_port >> 4;

	printk(BIOS_DEBUG, "Function 0x%x(0x%x, 0x%x) called\n",
			smif, function_parameter_0, function_parameter_1);
	switch (smif) {
	case 0x80:
		printk(BIOS_WARNING, "Function 0x80 is unimplemented!\n");
		printk(BIOS_DEBUG, "Function calls offset 0 in ACER_BOOT_DEVICE_SERVICE_PROTOCOL_GUID\n");
		break;
	case 0x81:
		toggle_turbo_disable(function_parameter_0);
		break;
	case 0x82:
		handle_acpi_wake_event(function_parameter_0, function_parameter_1);
		break;
	default:
		/* Not handled */
		printk(BIOS_DEBUG, "Requested function is unknown!\n");
		return;
	}

	/*
	 * gnvs->smif:
	 * - On success, the handler returns 0
	 * - On failure, the handler returns a value != 0
	 */
	gnvs->smif = 0;
}

int mainboard_smi_apmc(u8 data)
{
	/* TODO: Continue SmmKbcDriver RE of common service registration and confirm */
	switch (data) {
	case APM_CNT_BOARD_SMI:
		if (gnvs) {
			ec_smi_handler(gnvs->smif);
		}
		break;
	case APM_CNT_ACPI_ENABLE:	/* Events generate SCIs for OS */
		/* use 0x68/0x6C to prevent races with userspace */
		ec_set_ports(0x6C, 0x68);
		/* discard all events */
		ec_clear_out_queue();
		/* Tests at runtime show this re-enables charging and battery reporting */
		send_ec_command(0xE9);	/* Vendor implements using ACPI "CMDB" register" */
		send_ec_data(0x81);
		/* TODO: Set touchpad GPP owner to ACPI? */
		break;
	case APM_CNT_ACPI_DISABLE:	/* Events generate SMIs for SMM */
		/* use 0x68/0x6C to prevent races with userspace */
		ec_set_ports(0x6C, 0x68);
		/* discard all events */
		ec_clear_out_queue();
		/* Tests at runtime show this disables charging and battery reporting */
		send_ec_command(0xE9);	/* Vendor implements using ACPI "CMDB" register" */
		send_ec_data(0x80);
		/* TODO: Set touchpad GPP owner to GPIO? */
		break;
	default:
		break;
	}
	return 0;
}
