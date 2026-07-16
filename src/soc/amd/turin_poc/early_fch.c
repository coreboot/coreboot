/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <amdblocks/espi.h>
#include <amdblocks/lpc.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/aoac.h>
#include <amdblocks/i2c.h>
#include <amdblocks/pmlib.h>
#include <amdblocks/smbus.h>
#include <amdblocks/smn.h>
#include <amdblocks/spi.h>
#include <amdblocks/uart.h>
#include <device/pci.h>
#include <soc/southbridge.h>
#include <soc/uart.h>
#include <soc/i2c.h>

/* Before console init */
void fch_pre_init(void)
{
	/*
	 * Enable_acpimmio_decode_pm04 to enable the ACPIMMIO decode which is
	 * needed to access the GPIO registers.
	 */
	enable_acpimmio_decode_pm04();
	/* Setup SPI base by calling lpc_early_init before setting up eSPI. */
	lpc_early_init();
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

	/*
	 * Skip eSPI configuration. The EFS/PSP DIR already contains eSPI configuration
	 * so x86 does not need to configure it. Running configure_espi_with_mb_hook
	 * and resetting eSPI causes the slave/AST2600 to stop printing output on serial
	 * port. Boards may still configure I/O decodes early, if needed.
	 *
	 * configure_espi_with_mb_hook();
	 */
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
