/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <soc/nvs.h>
#include <southbridge/intel/common/pmutil.h>
#include <northbridge/intel/ironlake/ironlake.h>
#include <ec/acpi/ec.h>
#include <ec/lenovo/h8/h8.h>
#include <delay.h>
#include "dock.h"

#define GPE_EC_SCI	1
#define GPE_EC_WAKE	13

static void mainboard_smi_handle_ec_sci(void)
{
	u8 status = inb(EC_SC);
	u8 event;

	if (!(status & EC_SCI_EVT))
		return;

	event = ec_query();
	printk(BIOS_DEBUG, "EC event %#02x\n", event);

	switch (event) {
	case 0x18:
		/* Fn-F9 key */
	case 0x27:
		/* Power loss */
	case 0x50:
		/* Undock Key */
		ec_clr_bit(0x03, 2);
		dock_disconnect();
		break;
	case 0x37:
	case 0x58:
		/* Dock Event */
		ec_clr_bit(0x03, 2);
		mdelay(250);
		dock_connect();
		ec_set_bit(0x03, 2);
		/* set dock LED to indicate status */
		ec_write(0x0c, 0x09);
		ec_write(0x0c, 0x88);
		break;
	default:
		break;
	}
}

void mainboard_smi_gpi(u32 gpi_sts)
{
	if (gpi_sts & (1 << GPE_EC_SCI))
		mainboard_smi_handle_ec_sci();
}

int mainboard_smi_apmc(u8 data)
{
	switch (data) {
	case APM_CNT_ACPI_ENABLE:
		/* use 0x1600/0x1604 to prevent races with userspace */
		ec_set_ports(0x1604, 0x1600);
		/* route H8SCI to SCI */
		gpi_route_interrupt(GPE_EC_SCI, GPI_IS_SCI);
		/* discard all events, and enable attention */
		ec_write(0x80, 0x01);
		break;
	case APM_CNT_ACPI_DISABLE:
		/* we have to use port 0x62/0x66, as 0x1600/0x1604 doesn't
		   provide a EC query function */
		ec_set_ports(0x66, 0x62);
		/* route H8SCI# to SMI */
		gpi_route_interrupt(GPE_EC_SCI, GPI_IS_SMI);
		/* discard all events, and enable attention */
		ec_write(0x80, 0x01);
		break;
	default:
		break;
	}
	return 0;
}

void mainboard_smi_sleep(u8 slp_typ)
{
	if (slp_typ == 3) {
		u8 ec_wake = ec_read(0x32);
		/* If EC wake events are enabled, enable wake on EC WAKE GPE. */
		if (ec_wake & 0x14) {
			/* Redirect EC WAKE GPE to SCI. */
			gpi_route_interrupt(GPE_EC_WAKE, GPI_IS_SCI);
		}
	}
}
