/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * SMI handler -- mostly takes care of SMIs from the EC
 */
#include "ec.h"

#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <delay.h>
#include <ec/compal/ene932/ec.h>
#include <southbridge/amd/agesa/hudson/hudson.h>
#include <southbridge/amd/agesa/hudson/smi.h>

#define ACPI_PM1_CNT_SLEEP(state) ((1 << 13) | (state & 0x7) << 10)

enum sleep_states {
	S0 = 0,
	S1 = 1,
	S3 = 3,
	S4 = 4,
	S5 = 5,
};

enum ec_smi_event {
	EC_SMI_EVENT_IDLE = 0x80,
	EC_SMI_BATTERY_LOW = 0xb3,
};

/* Tell EC to operate in APM mode. Events generate SMIs instead of SCIs */
static void ec_enter_apm_mode(void)
{
	ec_kbc_write_cmd(0x59);
	ec_kbc_write_ib(0xE9);
}
/* Tell EC to operate in ACPI mode, thus generating SCIs on events, not SMIs */
static void ec_enter_acpi_mode(void)
{
	ec_kbc_write_cmd(0x59);
	ec_kbc_write_ib(0xE8);
}

static uint8_t ec_get_smi_event(void)
{
	ec_kbc_write_cmd(0x56);
	return ec_kbc_read_ob();
}

static void ec_process_smi(uint8_t src)
{
	/* Reading the SMI source satisfies the EC in terms of responding to
	 * the event, regardless of whether we take an action or not.
	 */

	switch (src) {
	case EC_SMI_BATTERY_LOW:
		printk(BIOS_DEBUG, "Battery low. Shutting down\n");
		outl(ACPI_PM1_CNT_SLEEP(S5), ACPI_PM1_CNT_BLK);
		break;
	default:
		printk(BIOS_DEBUG, "EC_SMI event 0x%x\n", src);
	}
}

static void handle_ec_smi(void)
{
	uint8_t src;

	while ((src = ec_get_smi_event()) != EC_SMI_EVENT_IDLE)
		ec_process_smi(src);
}

static void handle_lid_smi(void)
{
	/* Only triggered in non-ACPI mode on lid close. */
	outl(ACPI_PM1_CNT_SLEEP(S4), ACPI_PM1_CNT_BLK);
}

int mainboard_smi_apmc(uint8_t data)
{
	switch (data) {
	case ACPI_SMI_CMD_ENABLE:
		printk(BIOS_DEBUG, "Enable ACPI mode\n");
		ec_enter_acpi_mode();
		hudson_disable_gevent_smi(EC_LID_GEVENT);
		break;
	case ACPI_SMI_CMD_DISABLE:
		printk(BIOS_DEBUG, "Disable ACPI mode\n");
		ec_enter_apm_mode();
		hudson_configure_gevent_smi(EC_LID_GEVENT, SMI_MODE_SMI,
					    SMI_LVL_LOW);
		break;
	default:
		printk(BIOS_DEBUG, "Unhandled ACPI command: 0x%x\n", data);
	}
	return 0;
}

void mainboard_smi_gpi(uint32_t gpi_sts)
{
	if (gpi_sts & (1 << EC_SMI_GEVENT))
		handle_ec_smi();
	if (gpi_sts & (1 << EC_LID_GEVENT))
		handle_lid_smi();
}
