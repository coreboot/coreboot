/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <delay.h>
#include <ec/acpi/ec.h>
#include <ec/lenovo/h8/h8.h>
#include <southbridge/intel/common/pmutil.h>
#include "dock.h"
#include "smi.h"

/*
 * GPIO2 is H8SCI# (the Embedded Controller SCI signal).
 * In ICH8, GPI[n] maps to GPE0_STS bit (n + 16), so GPI2 -> GPE0 bit 18
 * (0x12), matching the EC device _GPE value in the DSDT.
 */
#define GPE_EC_SCI	2

static void mainboard_smi_dock_connect(void)
{
	ec_clr_bit(0x03, 2);
	mdelay(250);
	if (!dock_connect()) {
		ec_set_bit(0x03, 2);
		/* set dock LED to indicate success */
		ec_write(0x0c, 0x09);
		ec_write(0x0c, 0x88);
	} else {
		/* blink dock LED to indicate failure */
		ec_write(0x0c, 0x08);
		ec_write(0x0c, 0xc9);
	}
}

static void mainboard_smi_dock_disconnect(void)
{
	ec_clr_bit(0x03, 2);
	dock_disconnect();
}

int mainboard_io_trap_handler(int smif)
{
	switch (smif) {
	case SMI_DOCK_CONNECT:
		mainboard_smi_dock_connect();
		break;
	case SMI_DOCK_DISCONNECT:
		mainboard_smi_dock_disconnect();
		break;
	default:
		return 0;
	}

	/* On success, the IO Trap Handler returns 1
	 * On failure, the IO Trap Handler returns a value != 1 */
	return 1;
}

static void mainboard_smi_handle_ec_sci(void)
{
	u8 status = inb(EC_SC);
	u8 event;

	if (!(status & EC_SCI_EVT))
		return;

	event = ec_query();
	printk(BIOS_DEBUG, "EC event %#02x\n", event);

	switch (event) {
	/* Undock key / power loss / Fn-F9 */
	case 0x18:
	case 0x27:
	case 0x50:
		mainboard_smi_dock_disconnect();
		break;
	/* Dock event */
	case 0x37:
	case 0x58:
		mainboard_smi_dock_connect();
		break;
	default:
		break;
	}
}

void mainboard_smi_gpi(u32 gpi)
{
	if (gpi & (1 << GPE_EC_SCI))
		mainboard_smi_handle_ec_sci();
}

int mainboard_smi_apmc(u8 data)
{
	switch (data) {
	case APM_CNT_ACPI_ENABLE:
		/* use 0x1600/0x1604 to prevent races with userspace */
		ec_set_ports(0x1604, 0x1600);
		/* route H8SCI# to SCI */
		gpi_route_interrupt(GPE_EC_SCI, GPI_IS_SCI);
		/* discard all pending events, enable attention */
		ec_write(0x80, 0x01);
		break;
	case APM_CNT_ACPI_DISABLE:
		/* fall back to port 0x62/0x66 for EC query in SMI */
		ec_set_ports(0x66, 0x62);
		/* route H8SCI# to SMI */
		gpi_route_interrupt(GPE_EC_SCI, GPI_IS_SMI);
		/* discard all pending events, enable attention */
		ec_write(0x80, 0x01);
		break;
	default:
		break;
	}
	return 0;
}
