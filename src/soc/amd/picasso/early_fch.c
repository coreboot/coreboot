/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <amdblocks/espi.h>
#include <amdblocks/lpc.h>
#include <amdblocks/smbus.h>
#include <amdblocks/spi.h>
#include <console/console.h>
#include <soc/i2c.h>
#include <soc/southbridge.h>
#include <soc/uart.h>
#include <types.h>

static void lpc_configure_decodes(void)
{
	if (CONFIG(POST_IO) && (CONFIG_POST_IO_PORT == 0x80))
		lpc_enable_port80();
}

/* Before console init */
void fch_pre_init(void)
{
	lpc_early_init();

	if (!CONFIG(SOC_AMD_COMMON_BLOCK_USE_ESPI))
		lpc_configure_decodes();

	fch_spi_early_init();
	enable_acpimmio_decode_pm04();
	fch_smbus_init();
	fch_enable_cf9_io();
	fch_enable_legacy_io();
	enable_aoac_devices();
	sb_reset_i2c_slaves();

	/*
	 * On reset Range_0 defaults to enabled. We want to start with a clean
	 * slate to not have things unexpectedly enabled.
	 */
	clear_uart_legacy_config();

	if (CONFIG(AMD_SOC_CONSOLE_UART))
		set_uart_config(CONFIG_UART_FOR_CONSOLE);
}

/* After console init */
void fch_early_init(void)
{
	fch_print_pmxc0_status();
	i2c_soc_early_init();

	if (CONFIG(DISABLE_SPI_FLASH_ROM_SHARING))
		lpc_disable_spi_rom_sharing();

	if (CONFIG(SOC_AMD_COMMON_BLOCK_USE_ESPI)) {
		espi_setup();
		espi_configure_decodes();
	}
}
