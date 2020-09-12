/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <intelblocks/pcr.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <superio/aspeed/ast2400/ast2400.h>
#include <superio/aspeed/common/aspeed.h>
#include <tp_pch_gpio.h>

/* these are defined in intelblocks/lpc_lib.h but we can't use them yet */
#define PCR_DMI_LPCIOD 0x2770
#define PCR_DMI_LPCIOE 0x2774
#define ASPEED_CONFIG_INDEX 0x2E
#define ASPEED_CONFIG_DATA 0x2F

static void enable_espi_lpc_io_windows(void)
{
	/*
	 * Set up decoding windows on PCH over PCR. The CPUs use two of AST2500 SIO ports,
	 * one is connected to debug header (SUART1) and another is used as SOL (SUART2).
	 * For that end it is wired into BMC virtual port.
	 */

	/* Open IO windows: 0x3f8 for com1 and 02e8 for com2 */
	pcr_or32(PID_DMI, PCR_DMI_LPCIOD, (0 << 0) | (1 << 4));
	/* LPC I/O enable: com1 and com2 */
	pcr_or32(PID_DMI, PCR_DMI_LPCIOE, (1 << 0) | (1 << 1));

	/* Enable com1 (0x3f8), com2 (02f8) and superio (0x2e) */
	pci_mmio_write_config32(PCH_DEV_LPC, 0x80,
		(1 << 28) | (1 << 16) | (1 << 17) | (0 << 0) | (1 << 4));
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
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));

	/* Open IO windows */
	enable_espi_lpc_io_windows();

	/* Configure appropriate physical port of SuperIO chip off BMC */
	const pnp_devfn_t serial_dev = PNP_DEV(ASPEED_CONFIG_INDEX,
					com_to_ast_sio(CONFIG_UART_FOR_CONSOLE));
	aspeed_enable_serial(serial_dev, CONFIG_TTYS0_BASE);

	/* Port 80h direct to GPIO for LED display */
	const pnp_devfn_t gpio_dev = PNP_DEV(ASPEED_CONFIG_INDEX, AST2400_GPIO);
	aspeed_enable_port80_direct_gpio(gpio_dev, GPIOH);

	/* Enable UART function pin */
	aspeed_enable_uart_pin(serial_dev);
}
