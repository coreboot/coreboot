/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <console/console.h>
#include <ec/ec.h>
#include <ec/google/chromeec/ec.h>
#include <intelblocks/lpc_lib.h>
#include <variant/ec.h>

static void ramstage_ec_init(void)
{
	const struct google_chromeec_event_info info = {
		.log_events = MAINBOARD_EC_LOG_EVENTS,
		.sci_events = MAINBOARD_EC_SCI_EVENTS,
		.s3_wake_events = MAINBOARD_EC_S3_WAKE_EVENTS,
		.s5_wake_events = MAINBOARD_EC_S5_WAKE_EVENTS,
		.s0ix_wake_events = MAINBOARD_EC_S0IX_WAKE_EVENTS,
	};

	printk(BIOS_INFO, "mainboard: EC init\n");

	google_chromeec_events_init(&info, acpi_is_wakeup_s3());
}

static void bootblock_ec_init(void)
{
	uint16_t ec_ioport_base;
	size_t ec_ioport_size;

	/*
	* Set up LPC decoding for the ChromeEC I/O port ranges:
	* - Ports 62/66, 60/64, and 200->208
	* - ChromeEC specific communication I/O ports.
	*/
	lpc_enable_fixed_io_ranges(LPC_IOE_EC_62_66 | LPC_IOE_KBC_60_64
		| LPC_IOE_LGE_200);
	google_chromeec_ioport_range(&ec_ioport_base, &ec_ioport_size);
	lpc_open_pmio_window(ec_ioport_base, ec_ioport_size);
}

void mainboard_ec_init(void)
{
	if (ENV_RAMSTAGE)
		ramstage_ec_init();
	else if (ENV_BOOTBLOCK)
		bootblock_ec_init();
}
