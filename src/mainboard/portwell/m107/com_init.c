/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pnp_ops.h>

#define SERIAL_DEV PNP_DEV(0x6E, 1) /* ITE8528 UART1 */

void bootblock_mainboard_early_init(void)
{
	/* Enable the serial port inside the EC */
	pnp_set_logical_device(SERIAL_DEV);
	pnp_set_enable(SERIAL_DEV, 1);
}
