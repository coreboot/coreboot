/*
 * SMI handler -- mostly takes care of SMIs from the EC
 *
 * Copyright (C) 2014 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */
#include "ec.h"

#include <console/console.h>
#include <cpu/x86/smm.h>
#include <delay.h>
#include <ec/compal/ene932/ec.h>
#include <southbridge/amd/agesa/hudson/hudson.h>

enum ec_smi_event {
	EC_SMI_EVENT_IDLE = 0x80,
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
	/*
	 * Stub: We aren't processing any events yet, but reading the SMI source
	 * satisfies the EC in terms of responding to the event.
	 */

	printk(BIOS_DEBUG, "EC_SMI event 0x%x\n", src);
}

static void handle_ec_smi(void)
{
	uint8_t src;

	while ((src = ec_get_smi_event()) != EC_SMI_EVENT_IDLE)
		ec_process_smi(src);
}

int mainboard_smi_apmc(uint8_t data)
{
	switch (data) {
	case ACPI_SMI_CMD_ENABLE:
		printk(BIOS_DEBUG, "Enable ACPI mode\n");
		ec_enter_acpi_mode();
		break;
	case ACPI_SMI_CMD_DISABLE:
		printk(BIOS_DEBUG, "Disable ACPI mode\n");
		ec_enter_apm_mode();
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
}
