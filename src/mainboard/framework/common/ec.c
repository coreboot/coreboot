/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <bootstate.h>
#include <console/console.h>
#include <ec/ec.h>
#include <ec/google/chromeec/ec.h>
#include <mainboard/framework/common/ec.h>

/*
 * Signal to the EC that BIOS has finished POST.
 */
#define EC_CMD_DIAGNOSIS	0x3E0B
#define EC_DIAGNOSIS_PORT80_COMPLETE	0xFF

void mainboard_ec_init(void)
{
	static const struct google_chromeec_event_info info = {
		.log_events = MAINBOARD_EC_LOG_EVENTS,
		.sci_events = MAINBOARD_EC_SCI_EVENTS,
		.s3_wake_events = MAINBOARD_EC_S3_WAKE_EVENTS,
		.s5_wake_events = MAINBOARD_EC_S5_WAKE_EVENTS,
		.s0ix_wake_events = MAINBOARD_EC_S0IX_WAKE_EVENTS,
	};

	printk(BIOS_DEBUG, "mainboard: EC init\n");

	google_chromeec_events_init(&info, acpi_is_wakeup_s3());
}

static void framework_ec_signal_bios_complete(void *unused)
{
	const uint8_t diagnosis_code = EC_DIAGNOSIS_PORT80_COMPLETE;
	struct chromeec_command cmd = {
		.cmd_code = EC_CMD_DIAGNOSIS,
		.cmd_version = 0,
		.cmd_data_in = &diagnosis_code,
		.cmd_size_in = sizeof(diagnosis_code),
		.cmd_data_out = NULL,
		.cmd_size_out = 0,
		.cmd_dev_index = 0,
	};

	if (google_chromeec_command(&cmd))
		printk(BIOS_ERR, "mainboard: Failed to signal BIOS complete to EC\n");
	else
		printk(BIOS_DEBUG, "mainboard: Signalled BIOS complete to EC\n");
}
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT,
		      framework_ec_signal_bios_complete, NULL);
