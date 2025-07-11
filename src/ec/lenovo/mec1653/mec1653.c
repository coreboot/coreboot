/* SPDX-License-Identifier: GPL-2.0-only */

#include <ec/acpi/ec.h>
#include <arch/io.h>
#include "mec1653.h"
#include "uart.h"

void bootblock_ec_init(void)
{
	// Tell EC via BIOS Debug Port 1 that the world isn't on fire

	// Let the EC know that BIOS code is running
	outb(0x11, 0x86);
	outb(0x6e, 0x86);

	// Enable accesses to EC1 interface
	ec_set_ports(EC_SC, EC_DATA);
	ec_clear_out_queue();
	ec_write(ec_read(0), 0x20);

	// Reset LEDs to power on state
	// (Without this warm reboot leaves LEDs off)
	ec_write(0x0c, 0x80);
	ec_write(0x0c, 0x07);
	ec_write(0x0c, 0x8a);

	// Setup debug UART
	if (CONFIG(MEC1653_ENABLE_UART))
		mec1653_configure_uart();
}
