/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <amdblocks/lpc.h>
#include <soc/southbridge.h>
#include <variant/ec.h>

static void ramstage_ec_init(void)
{
	const struct google_chromeec_event_info info = {
		.log_events = MAINBOARD_EC_LOG_EVENTS,
		.sci_events = MAINBOARD_EC_SCI_EVENTS,
		.s3_wake_events = MAINBOARD_EC_S3_WAKE_EVENTS,
		.s5_wake_events = MAINBOARD_EC_S5_WAKE_EVENTS,
	};

	printk(BIOS_DEBUG, "mainboard: EC init\n");

	google_chromeec_events_init(&info, acpi_is_wakeup_s3());
}

static void early_ec_init(void)
{
	uint16_t ec_ioport_base;
	size_t ec_ioport_size;
	int status;

	/*
	 * Set up LPC decoding for the ChromeEC I/O port ranges:
	 * - Ports 62/66, 60/64, and 200->208
	 *  -- set by hudson_lpc_decode() in pre
	 * - ChromeEC specific communication I/O ports.
	 */
	google_chromeec_ioport_range(&ec_ioport_base, &ec_ioport_size);
	printk(BIOS_DEBUG,
		"LPC Setup google_chromeec_ioport_range: %04x, %08zx\n",
		ec_ioport_base, ec_ioport_size);
	status = lpc_set_wideio_range(ec_ioport_base, ec_ioport_size);
	if (status == WIDEIO_RANGE_ERROR)
		printk(BIOS_WARNING, "ERROR: Failed to assign a range\n");
	else
		printk(BIOS_DEBUG, "Range assigned to wide IO %d\n", status);
}

void mainboard_ec_init(void)
{
	if (ENV_RAMSTAGE)
		ramstage_ec_init();
	else
		early_ec_init();
}
