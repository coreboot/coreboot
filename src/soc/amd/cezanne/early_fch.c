/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <amdblocks/espi.h>
#include <amdblocks/i2c.h>
#include <amdblocks/lpc.h>
#include <amdblocks/pmlib.h>
#include <amdblocks/smbus.h>
#include <console/console.h>
#include <soc/i2c.h>
#include <soc/southbridge.h>
#include <soc/uart.h>

#include "chip.h"

/* Table to switch SCL pins to outputs to initially reset the I2C peripherals */
static const struct soc_i2c_scl_pin i2c_scl_pins[] = {
	{ PAD_GPO(I2C0_SCL_PIN, HIGH), GPIO_I2C0_SCL },
	{ PAD_GPO(I2C1_SCL_PIN, HIGH), GPIO_I2C1_SCL },
	{ PAD_GPO(I2C2_SCL_PIN, HIGH), GPIO_I2C2_SCL },
	{ PAD_GPO(I2C3_SCL_PIN, HIGH), GPIO_I2C3_SCL },
};

static void reset_i2c_peripherals(void)
{
	const struct soc_amd_cezanne_config *cfg = config_of_soc();
	struct soc_i2c_peripheral_reset_info reset_info;

	reset_info.i2c_scl_reset_mask = cfg->i2c_scl_reset & GPIO_I2C_MASK;
	reset_info.i2c_scl = i2c_scl_pins;
	reset_info.num_pins = ARRAY_SIZE(i2c_scl_pins);
	sb_reset_i2c_peripherals(&reset_info);
}

/* Initialize port80h routing early if needed */
void configure_port80_routing_early(void)
{
	mb_set_up_early_espi();
	if (CONFIG(SOC_AMD_COMMON_BLOCK_USE_ESPI))
		espi_setup();
}

/* Before console init */
void fch_pre_init(void)
{
	lpc_early_init();
	fch_spi_early_init();
	enable_acpimmio_decode_pm04();
	fch_smbus_init();
	fch_enable_cf9_io();
	fch_enable_legacy_io();
	fch_disable_legacy_dma_io();
	enable_aoac_devices();
	reset_i2c_peripherals();

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
	pm_set_power_failure_state();
	fch_print_pmxc0_status();
	i2c_soc_early_init();
	show_spi_speeds_and_modes();

	if (CONFIG(DISABLE_SPI_FLASH_ROM_SHARING))
		lpc_disable_spi_rom_sharing();

	if (CONFIG(SOC_AMD_COMMON_BLOCK_USE_ESPI) && !CONFIG(NO_EARLY_BOOTBLOCK_POSTCODES))
		espi_setup();
}
