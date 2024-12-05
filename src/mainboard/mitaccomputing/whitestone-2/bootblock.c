/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/pcr.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <soc/intel/common/block/lpc/lpc_def.h>
#include <superio/aspeed/ast2400/ast2400.h>
#include <superio/aspeed/common/aspeed.h>

#define ASPEED_SIO_PORT 0x2E

static void enable_espi_lpc_io_windows(void)
{
	/*
	 * Set up decoding windows on PCH over PCR. The CPUs use two of AST2600 SIO ports,
	 * one is connected to debug header (SUART1) and another is used as SOL (SUART2).
	 * For Whitestone-2, only SUART1 is used.
	 * Enable com1 (0x3f8), com2 (0x2f8) and superio (0x2e)
	 */
	lpc_open_pmio_window(0x3f8, 8);
	lpc_open_pmio_window(0x2f8, 8);
	lpc_open_pmio_window(0x2e, 2);
	lpc_io_setup_comm_a_b();
}

static uint8_t com_to_ast_sio(uint8_t com)
{
	switch (com) {
	case 0:
		return AST2400_SUART1;
	case 1:
		return AST2400_SUART2;
	case 2:
		return AST2400_SUART3;
	case 4:
		return AST2400_SUART4;
	default:
		return AST2400_SUART1;
	}
}

void bootblock_mainboard_early_init(void)
{
	/* Open IO windows */
	enable_espi_lpc_io_windows();

	/* Configure appropriate physical port of SuperIO chip off BMC */
	const pnp_devfn_t serial_dev = PNP_DEV(ASPEED_SIO_PORT,
		com_to_ast_sio(CONFIG_UART_FOR_CONSOLE));
	aspeed_enable_serial(serial_dev, CONFIG_TTYS0_BASE);
}
