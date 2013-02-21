/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/acpi.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <types.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include "ec.h"

void link_ec_init(void)
{
	printk(BIOS_DEBUG, "link_ec_init\n");
	post_code(0xf0);

	/* Restore SCI event mask on resume. */
	if (acpi_slp_type == 3) {
		google_chromeec_log_events(LINK_EC_LOG_EVENTS |
					   LINK_EC_S3_WAKE_EVENTS);

		/* Disable SMI and wake events */
		google_chromeec_set_smi_mask(0);

		/* Clear pending events */
		while (google_chromeec_get_event() != 0);
		google_chromeec_set_sci_mask(LINK_EC_SCI_EVENTS);
	} else {
		google_chromeec_log_events(LINK_EC_LOG_EVENTS |
					   LINK_EC_S5_WAKE_EVENTS);
	}

	/* Clear wake events, these are enabled on entry to sleep */
	google_chromeec_set_wake_mask(0);

	post_code(0xf1);
}
