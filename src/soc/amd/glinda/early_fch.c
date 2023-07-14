/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Update for Glinda */

#include <amdblocks/acpimmio.h>
#include <amdblocks/aoac.h>
#include <amdblocks/espi.h>
#include <amdblocks/i2c.h>
#include <amdblocks/lpc.h>
#include <amdblocks/pmlib.h>
#include <amdblocks/smbus.h>
#include <amdblocks/uart.h>
#include <soc/i2c.h>
#include <soc/southbridge.h>
#include <soc/uart.h>

#include "chip.h"

/* Before console init */
void fch_pre_init(void)
{
	/*
	 * PM_04_ACPIMMIO_DECODE_EN which enables the ACPIMMIO decode is already set after
	 * reset. Since the IO port based indirect PM register space access isn't implemented
	 * in Phoenix any more, don't call enable_acpimmio_decode_pm04() which uses the
	 * indirect PM register space access via the IO ports that aren't implemented any more.
	 */
	/* Setup SPI base by calling lpc_early_init before setting up eSPI. */
	lpc_early_init();

	/* Setup eSPI to enable port80 routing if the board is using eSPI and the eSPI
	   interface hasn't already been set up in verstage on PSP */
	if (CONFIG(SOC_AMD_COMMON_BLOCK_USE_ESPI) && !CONFIG(VBOOT_STARTS_BEFORE_BOOTBLOCK))
		configure_espi_with_mb_hook();

	fch_spi_early_init();
	fch_smbus_init();
	fch_enable_cf9_io();
	fch_enable_legacy_io();
	fch_disable_legacy_dma_io();
	enable_aoac_devices();

	/*
	 * On reset Range_0 defaults to enabled. We want to start with a clean
	 * slate to not have things unexpectedly enabled.
	 */
	clear_uart_legacy_config();

	if (CONFIG(AMD_SOC_CONSOLE_UART))
		set_uart_config(CONFIG_UART_FOR_CONSOLE);

	/* disable the keyboard reset function before mainboard GPIO setup */
	if (CONFIG(DISABLE_KEYBOARD_RESET_PIN))
		fch_disable_kb_rst();
}

/* After console init */
void fch_early_init(void)
{
	reset_i2c_peripherals();
	pm_set_power_failure_state();
	fch_print_pmxc0_status();
	i2c_soc_early_init();
	show_spi_speeds_and_modes();

	if (CONFIG(DISABLE_SPI_FLASH_ROM_SHARING))
		lpc_disable_spi_rom_sharing();
}
