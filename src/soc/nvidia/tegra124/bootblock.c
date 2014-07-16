/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <assert.h>
#include <arch/cache.h>
#include <arch/exception.h>
#include <bootblock_common.h>
#include <cbfs.h>
#include <console/console.h>
#include <soc/clock.h>
#include <soc/nvidia/tegra/apbmisc.h>
#include "pinmux.h"
#include "power.h"
#include "verstage.h"
#include <soc/addressmap.h>
#include <soc/nvidia/tegra/i2c.h>
#include <soc/nvidia/tegra124/gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>
static struct clk_rst_ctlr *clk_rst = (void *)TEGRA_CLK_RST_BASE;

static void setup_pinmux(void)
{
	// Write protect.
	gpio_input_pullup(GPIO(R1));
	// Recovery mode.
	gpio_input_pullup(GPIO(Q7));
	// Lid switch.
	gpio_input_pullup(GPIO(R4));
	// Power switch.
	gpio_input_pullup(GPIO(Q0));
	// Developer mode.
	gpio_input_pullup(GPIO(Q6));
	// EC in RW.
	gpio_input_pullup(GPIO(U4));

	// route PU4/5 to GMI to remove conflict w/PWM1/2.
	pinmux_set_config(PINMUX_GPIO_PU4_INDEX, PINMUX_GPIO_PU4_FUNC_NOR);
	pinmux_set_config(PINMUX_GPIO_PU5_INDEX, PINMUX_GPIO_PU5_FUNC_NOR);

	// SOC and TPM reset GPIO, active low.
	gpio_output(GPIO(I5), 1);

	// SPI1 MOSI
	pinmux_set_config(PINMUX_ULPI_CLK_INDEX, PINMUX_ULPI_CLK_FUNC_SPI1 |
						 PINMUX_PULL_NONE |
						 PINMUX_INPUT_ENABLE);
	// SPI1 MISO
	pinmux_set_config(PINMUX_ULPI_DIR_INDEX, PINMUX_ULPI_DIR_FUNC_SPI1 |
						 PINMUX_PULL_NONE |
						 PINMUX_INPUT_ENABLE);
	// SPI1 SCLK
	pinmux_set_config(PINMUX_ULPI_NXT_INDEX, PINMUX_ULPI_NXT_FUNC_SPI1 |
						 PINMUX_PULL_NONE |
						 PINMUX_INPUT_ENABLE);
	// SPI1 CS0
	pinmux_set_config(PINMUX_ULPI_STP_INDEX, PINMUX_ULPI_STP_FUNC_SPI1 |
						 PINMUX_PULL_NONE |
						 PINMUX_INPUT_ENABLE);

	// I2C3 (cam) clock.
	pinmux_set_config(PINMUX_CAM_I2C_SCL_INDEX,
			  PINMUX_CAM_I2C_SCL_FUNC_I2C3 | PINMUX_INPUT_ENABLE);
	// I2C3 (cam) data.
	pinmux_set_config(PINMUX_CAM_I2C_SDA_INDEX,
			  PINMUX_CAM_I2C_SDA_FUNC_I2C3 | PINMUX_INPUT_ENABLE);

	// switch unused pin to GPIO
	gpio_set_mode(GPIO(X3), GPIO_MODE_GPIO);
	gpio_set_mode(GPIO(X4), GPIO_MODE_GPIO);
	gpio_set_mode(GPIO(X5), GPIO_MODE_GPIO);
	gpio_set_mode(GPIO(X6), GPIO_MODE_GPIO);
	gpio_set_mode(GPIO(X7), GPIO_MODE_GPIO);
	gpio_set_mode(GPIO(W3), GPIO_MODE_GPIO);
}

static void configure_ec_spi_bus(void)
{
	clock_configure_source(sbc1, CLK_M, 3000);
}

static void configure_tpm_i2c_bus(void)
{
	clock_configure_i2c_scl_freq(i2c3, PLLP, 400);

	i2c_init(2);
}

void main(void)
{
	void *entry;

	// enable pinmux clamp inputs
	clamp_tristate_inputs();

	// enable JTAG at the earliest stage
	enable_jtag();

	clock_early_uart();

	// Serial out, tristate off.
	pinmux_set_config(PINMUX_KB_ROW9_INDEX, PINMUX_KB_ROW9_FUNC_UA3);
	// Serial in, tristate_on.
	pinmux_set_config(PINMUX_KB_ROW10_INDEX, PINMUX_KB_ROW10_FUNC_UA3 |
						 PINMUX_PULL_UP |
						 PINMUX_INPUT_ENABLE);
	// Mux some pins away from uart A.
	pinmux_set_config(PINMUX_UART2_CTS_N_INDEX,
			  PINMUX_UART2_CTS_N_FUNC_UB3 |
			  PINMUX_INPUT_ENABLE);
	pinmux_set_config(PINMUX_UART2_RTS_N_INDEX,
			  PINMUX_UART2_RTS_N_FUNC_UB3);

	if (CONFIG_BOOTBLOCK_CONSOLE) {
		console_init();
		exception_init();
	}

	clock_init();

	bootblock_mainboard_init();

	pinmux_set_config(PINMUX_CORE_PWR_REQ_INDEX,
			  PINMUX_CORE_PWR_REQ_FUNC_PWRON);
	pinmux_set_config(PINMUX_CPU_PWR_REQ_INDEX,
			  PINMUX_CPU_PWR_REQ_FUNC_CPU);
	pinmux_set_config(PINMUX_PWR_INT_N_INDEX,
			  PINMUX_PWR_INT_N_FUNC_PMICINTR |
			  PINMUX_INPUT_ENABLE);

	if (IS_ENABLED(CONFIG_VBOOT2_VERIFY_FIRMWARE)) {
		clock_enable_clear_reset(0, CLK_H_SBC1, CLK_U_I2C3, 0, 0, 0);
		setup_pinmux();
		configure_ec_spi_bus();
		configure_tpm_i2c_bus();
		entry = (void *)verstage_vboot_main;
	} else
		entry = cbfs_load_stage(CBFS_DEFAULT_MEDIA, "fallback/romstage");

	ASSERT(entry);
	clock_cpu0_config(entry);

	power_enable_and_ungate_cpu();

	/* Repair ram on cluster0 and cluster1 after CPU is powered on. */
	ram_repair();

	clock_cpu0_remove_reset();

	clock_halt_avp();
}
