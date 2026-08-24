/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/espi.h>
#include <amdblocks/lpc.h>
#include <amdblocks/spi.h>
#include <bootblock_common.h>
#include <device/mmio.h>
#include <device/pnp_ops.h>
#include <device/pnp_type.h>
#include <gpio.h>
#include <soc/iomap.h>
#include <superio/aspeed/ast2400/ast2400.h>
#include <superio/aspeed/common/aspeed.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6796d/nct6796d.h>

#define ESPI1_BASE (SPI_BASE_ADDRESS + ESPI1_OFFSET_FROM_BAR)

void bootblock_mainboard_early_init(void)
{
	/* Configure appropriate physical port of SuperIO chip off BMC */
	const pnp_devfn_t serial1_dev = PNP_DEV(0x4e, AST2400_SUART1);
	const pnp_devfn_t serial2_dev = PNP_DEV(0x2e, NCT6796D_SP1);

	/* Unconfigure 0x3e8 and 0x2e8 ranges (IO ranges 4 and 5) on eSPI 1 */
	clrbits32p(ESPI1_BASE + ESPI_DECODE, BIT(16) | BIT(17));	/* IO_RANGE_EXT_EN */
	write16p(ESPI1_BASE + 0x80, 0x0);	/* ESPI_IO_BASE_REG2, range 4 */
	write16p(ESPI1_BASE + 0x82, 0x0);	/* ESPI_IO_BASE_REG2, range 5 */
	write8p(ESPI1_BASE + 0x88, 0x0);	/* ESPI_IO_SIZE1, range 4 */
	write8p(ESPI1_BASE + 0x89, 0x0);	/* ESPI_IO_SIZE1, range 5 */

	/*
	 * APCBs are configured to enable 0x3f8 range already.
	 * Eable only post codes and SIO UART1 here.
	 */
	espi_open_io_window(0x2e, 2);
	espi_open_io_window(0x80, 1);
	espi_open_io_window(0x3e8, 8);

	/* Enable AST2600 SuperIO UART1 (COM1 header) */
	aspeed_enable_serial(serial1_dev, 0x3f8);
	/* Enable UART function pin */
	aspeed_enable_uart_pin(serial1_dev);

	/* Enable serial port on UART1 header */
	nuvoton_enable_serial(serial2_dev, 0x3e8);
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
};

void bootblock_mainboard_init(void)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
	lpc_tpm_decode_spi();
}
