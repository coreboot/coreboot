/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/espi.h>
#include <amdblocks/lpc.h>
#include <amdblocks/spi.h>
#include <bootblock_common.h>
#include <device/pnp_type.h>
#include <gpio.h>
#include <superio/aspeed/ast2400/ast2400.h>
#include <superio/aspeed/common/aspeed.h>

/*
 * The board can use two UARTs:
 *
 * - BMC UART1 (SOL/COM1) exposed as DB9 connector on rear panel
 * - FCH UART0 on internal P0_UART header
 *
 * P0_UART works on 1.8V levels and it needs voltage level shifter to 3.3V.
 * Pinout:
 *
 *  ---
 *  | |-- VCC (1.8V)
 *  | |-- RXD
 *  | |-- TXD
 *  | |-- GND
 *  ---
 */
void bootblock_mainboard_early_init(void)
{
	/* Configure appropriate physical port of SuperIO chip off BMC */
	const pnp_devfn_t serial1_dev = PNP_DEV(0x2e, AST2400_SUART1);

	/*
	 * APCBs are configured to enable 0xca2 and 0x3f8 ranges already.
	 * Eable only SIO and post codes here.
	 */
	espi_open_io_window(0x2e, 2);
	espi_open_io_window(0x80, 1);

	aspeed_enable_serial(serial1_dev, 0x3f8);
	/* Enable UART function pin */
	aspeed_enable_uart_pin(serial1_dev);
}


static const struct soc_amd_gpio gpio_table[] = {
	PAD_GPO(GPIO_3, LOW),
	PAD_NFO(GPIO_4, SATA_ACT_L, LOW),
	PAD_GPI(GPIO_5, PULL_DOWN),
	PAD_GPI(GPIO_6, PULL_DOWN),
	PAD_NF(GPIO_76, SPI_TPM_CS_L, PULL_UP),
	PAD_GPI(GPIO_89, PULL_UP),
	PAD_NF(GPIO_89, PM_INTR_L, PULL_NONE),
	PAD_GPO(GPIO_115, LOW),
	PAD_GPO(GPIO_116, LOW),
	PAD_NF(GPIO_129, KBRST_L, PULL_UP),
	PAD_NF(GPIO_135, UART0_CTS_L, PULL_NONE),
	PAD_NF(GPIO_136, UART0_RXD, PULL_NONE),
	PAD_NF(GPIO_137, UART0_RTS_L, PULL_NONE),
	PAD_NF(GPIO_138, UART0_TXD, PULL_NONE),
	PAD_NF(GPIO_139, UART0_INTR, PULL_NONE),
	PAD_NF(GPIO_141, UART1_RXD, PULL_NONE),
	PAD_NF(GPIO_142, UART1_TXD, PULL_NONE)
};

void bootblock_mainboard_init(void)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
	lpc_tpm_decode_spi();
}
