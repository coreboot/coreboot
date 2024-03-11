/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <console/console.h>
#include <intelblocks/lpc_lib.h>
#include <soc/intel/common/block/lpc/lpc_def.h>
#include <superio/aspeed/ast2400/ast2400.h>
#include <superio/aspeed/common/aspeed.h>

#define ASPEED_SIO_PORT 0x2E

void bootblock_mainboard_early_init(void)
{
	/* Enable eSPI decoding for com1 (0x3f8), com2 (02f8) and superio (0x2e) */
	lpc_io_setup_comm_a_b();
	lpc_enable_fixed_io_ranges(LPC_IOE_SUPERIO_2E_2F);

	if (CONFIG_UART_FOR_CONSOLE == 0) {
		/* Setup superio com1 */
		const pnp_devfn_t serial_dev = PNP_DEV(ASPEED_SIO_PORT, AST2400_SUART1);
		aspeed_enable_serial(serial_dev, CONFIG_TTYS0_BASE);
	} else
		die("COMs other than COM1 not supported\n");
}
