/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/x86/smm.h>
#include <ec/acpi/ec.h>
#include <ec/compal/ene932/ec.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/pmutil.h>

#include "ec.h"

#define GPE_PALMDET1	2
#define GPE_PALMDET2	4
#define GPE_EC_SCI	7
#define GPE_EC_SMI	8
/* FIXME: check this */
#define GPE_EC_WAKE	13

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

/* Tell EC to operate in APM mode. Events generate SMIs instead of SCIs. */
static void ec_enter_apm_mode(void)
{
	ec_kbc_write_cmd(0x59);
	ec_kbc_write_ib(0xE9);
}
/* Tell EC to operate in ACPI mode, thus generating SCIs on events, not SMIs. */
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
	 * Reading the SMI source satisfies the EC in terms of responding to
	 * the event, regardless of whether we take an action or not.
	 */

	printk(BIOS_DEBUG, "Unhandled EC_SMI event 0x%x\n", src);
}

static void handle_ec_smi(void)
{
	uint8_t src;

	while ((src = ec_get_smi_event()) != EC_SMI_EVENT_IDLE)
		ec_process_smi(src);
}

void mainboard_smi_gpi(u32 gpi_sts)
{
	if (gpi_sts & (1 << GPE_EC_SMI))
		handle_ec_smi();
}

int mainboard_smi_apmc(u8 data)
{
	switch (data) {
	case APM_CNT_ACPI_ENABLE:
		ec_enter_acpi_mode();
		gpi_route_interrupt(GPE_EC_SCI, GPI_IS_SCI);
		gpi_route_interrupt(GPE_PALMDET1, GPI_IS_SCI);
		gpi_route_interrupt(GPE_PALMDET2, GPI_IS_SCI);
		break;
	case APM_CNT_ACPI_DISABLE:
		ec_enter_apm_mode();
		gpi_route_interrupt(GPE_EC_SCI, GPI_IS_SMI);
		break;
	default:
		printk(BIOS_DEBUG, "Unhandled ACPI command: 0x%x\n", data);
	}
	return 0;
}

void mainboard_smi_sleep(u8 slp_typ)
{
	if (slp_typ == S3) {
		u8 ec_wake = ec_read(0x32);
		/* If EC wake events are enabled, enable wake on EC WAKE GPE. */
		if (ec_wake & 0x14)
			gpi_route_interrupt(GPE_EC_WAKE, GPI_IS_SCI);
	}
}
