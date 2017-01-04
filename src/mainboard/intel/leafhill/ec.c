/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/acpi.h>
#include <console/console.h>
#include <ec/ec.h>
#include <ec/google/chromeec/ec.h>
#include <rules.h>
#include <soc/lpc.h>
#include <variant/ec.h>

static void ramstage_ec_init(void)
{
	printk(BIOS_ERR, "mainboard: EC init\n");

	if (acpi_is_wakeup_s3()) {
		google_chromeec_log_events(MAINBOARD_EC_LOG_EVENTS |
					   MAINBOARD_EC_S3_WAKE_EVENTS);

		/* Disable SMI and wake events */
		google_chromeec_set_smi_mask(0);

		/* Clear pending events */
		while (google_chromeec_get_event() != 0)
			;

		/* Restore SCI event mask */
		google_chromeec_set_sci_mask(MAINBOARD_EC_SCI_EVENTS);
	} else {
		google_chromeec_log_events(MAINBOARD_EC_LOG_EVENTS |
					   MAINBOARD_EC_S5_WAKE_EVENTS);
	}

	/* Clear wake event mask */
	google_chromeec_set_wake_mask(0);
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
	lpc_enable_fixed_io_ranges(IOE_EC_62_66 | IOE_KBC_60_64 | IOE_LGE_200);
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
