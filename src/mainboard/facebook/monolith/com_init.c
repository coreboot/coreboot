/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pnp_ops.h>
#include <device/pnp.h>
#include "onboard.h"

#define SERIAL_DEV1 PNP_DEV(ITE8528_CMD_PORT, 1) /* ITE8528 UART1 */
#define SERIAL_DEV2 PNP_DEV(ITE8528_CMD_PORT, 2) /* ITE8528 UART2 */

void bootblock_mainboard_early_init(void)
{
	/* Enable the serial ports inside the EC */
	pnp_set_logical_device(SERIAL_DEV1);
	pnp_set_enable(SERIAL_DEV1, 1);
	pnp_set_logical_device(SERIAL_DEV2);
	pnp_set_iobase(SERIAL_DEV2, PNP_IDX_IO0, 0x3e8);
	pnp_set_irq(SERIAL_DEV2, PNP_IDX_IRQ0, 3);
	pnp_set_enable(SERIAL_DEV2, 1);
}
