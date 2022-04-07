/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/pcr.h>
#include <soc/intel/common/block/lpc/lpc_def.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <superio/aspeed/ast2400/ast2400.h>
#include <superio/aspeed/common/aspeed.h>
#include <cpxsp_dl_gpio.h>

#define ASPEED_SIO_PORT 0x2E
#define PCR_DMI_LPCIOD 0x2770
#define PCR_DMI_LPCIOE 0x2774

static void enable_espi_lpc_io_windows(void)
{
	/*
	 * Set up decoding windows on PCH over PCR. The CPUs use two of AST2500 SIO ports,
	 * one is connected to debug header (SUART1) and another is used as SOL (SUART2).
	 * For that end it is wired into BMC virtual port.
	 */
	uint16_t lpciod = (LPC_IOD_COMB_RANGE | LPC_IOD_COMA_RANGE);
	uint16_t lpcioe = (LPC_IOE_SUPERIO_2E_2F | LPC_IOE_COMB_EN | LPC_IOE_COMA_EN);

	/* Open IO windows: 0x3f8 for com1 and 02e8 for com2 */
	pcr_or32(PID_DMI, PCR_DMI_LPCIOD, lpciod);
	/* LPC I/O enable: com1 and com2 */
	pcr_or32(PID_DMI, PCR_DMI_LPCIOE, lpcioe);

	/* Enable com1 (0x3f8), com2 (02f8) and superio (0x2e) */
	pci_write_config16(PCH_DEV_LPC, LPC_IO_DECODE, lpciod);
	pci_write_config16(PCH_DEV_LPC, LPC_IO_ENABLES, lpcioe);
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
	/* pre-configure Lewisburg PCH GPIO pads */
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));

	/* Open IO windows */
	enable_espi_lpc_io_windows();

	/* Configure appropriate physical port of SuperIO chip off BMC */
	const pnp_devfn_t serial_dev = PNP_DEV(ASPEED_SIO_PORT,
		com_to_ast_sio(CONFIG_UART_FOR_CONSOLE));
	aspeed_enable_serial(serial_dev, CONFIG_TTYS0_BASE);
}
