/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <gpio.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/early_configs.h>
#include <soc/nvidia/tegra/i2c.h>

static struct clk_rst_ctlr *clk_rst = (void *)TEGRA_CLK_RST_BASE;

static void setup_pinmux(void)
{
	/* Write protect. */
	gpio_input_pullup(GPIO(R1));
	/* Recovery mode. */
	gpio_input_pullup(GPIO(Q7));
	/* Lid switch. */
	gpio_input_pullup(GPIO(R4));
	/* Power switch. */
	gpio_input_pullup(GPIO(Q0));
	/* Developer mode. */
	gpio_input_pullup(GPIO(Q6));
	/* EC in RW. */
	gpio_input_pullup(GPIO(U4));

	/* route PU4/5 to GMI to remove conflict w/PWM1/2. */
	pinmux_set_config(PINMUX_GPIO_PU4_INDEX,
			  PINMUX_GPIO_PU4_FUNC_NOR);        /* s/b GMI */
	pinmux_set_config(PINMUX_GPIO_PU5_INDEX,
			  PINMUX_GPIO_PU5_FUNC_NOR);        /* s/b GMI */

	/* SOC and TPM reset GPIO, active low. */
	gpio_output(GPIO(I5), 1);

	/* SPI1 MOSI */
	pinmux_set_config(PINMUX_ULPI_CLK_INDEX, PINMUX_ULPI_CLK_FUNC_SPI1 |
						 PINMUX_PULL_NONE |
						 PINMUX_INPUT_ENABLE);
	/* SPI1 MISO */
	pinmux_set_config(PINMUX_ULPI_DIR_INDEX, PINMUX_ULPI_DIR_FUNC_SPI1 |
						 PINMUX_PULL_NONE |
						 PINMUX_INPUT_ENABLE);
	/* SPI1 SCLK */
	pinmux_set_config(PINMUX_ULPI_NXT_INDEX, PINMUX_ULPI_NXT_FUNC_SPI1 |
						 PINMUX_PULL_NONE |
						 PINMUX_INPUT_ENABLE);
	/* SPI1 CS0 */
	pinmux_set_config(PINMUX_ULPI_STP_INDEX, PINMUX_ULPI_STP_FUNC_SPI1 |
						 PINMUX_PULL_NONE |
						 PINMUX_INPUT_ENABLE);

	/* I2C3 (cam) clock. */
	pinmux_set_config(PINMUX_CAM_I2C_SCL_INDEX,
			  PINMUX_CAM_I2C_SCL_FUNC_I2C3 | PINMUX_INPUT_ENABLE);
	/* I2C3 (cam) data. */
	pinmux_set_config(PINMUX_CAM_I2C_SDA_INDEX,
			  PINMUX_CAM_I2C_SDA_FUNC_I2C3 | PINMUX_INPUT_ENABLE);

	/* switch unused pin to GPIO */
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

void early_mainboard_init(void)
{
	clock_enable_clear_reset(0, CLK_H_SBC1, CLK_U_I2C3, 0, 0, 0);
	setup_pinmux();
	configure_ec_spi_bus();
	configure_tpm_i2c_bus();
}
