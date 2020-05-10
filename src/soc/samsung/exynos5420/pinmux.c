/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/gpio.h>
#include <soc/pinmux.h>

static void exynos_pinmux_uart(int start, int count)
{
	int i;

	for (i = start; i < start + count; i++) {
		gpio_set_pull(i, GPIO_PULL_NONE);
		gpio_cfg_pin(i, GPIO_FUNC(0x2));
	}
}

void exynos_pinmux_uart0(void)
{
	exynos_pinmux_uart(GPIO_A00, 4);
}

void exynos_pinmux_uart1(void)
{
	exynos_pinmux_uart(GPIO_A04, 4);
}

void exynos_pinmux_uart2(void)
{
	exynos_pinmux_uart(GPIO_A10, 4);
}

void exynos_pinmux_uart3(void)
{
	exynos_pinmux_uart(GPIO_A14, 2);
}

struct gpio {
	enum exynos5_gpio_pin pin;
	unsigned int func;
	unsigned int pull;
	unsigned int drv;
};

static void exynos_pinmux_sdmmc(struct gpio *gpios, int num_gpios)
{
	int i;

	for (i = 0; i < num_gpios; i++) {
		gpio_set_drv(gpios[i].pin, gpios[i].drv);
		gpio_set_pull(gpios[i].pin, gpios[i].pull);
		gpio_cfg_pin(gpios[i].pin, GPIO_FUNC(gpios[i].func));
	}
}

void exynos_pinmux_sdmmc0(void)
{
	struct gpio gpios[] = {
		{ GPIO_C00, 0x2, GPIO_PULL_NONE, GPIO_DRV_4X },	/* CLK */
		{ GPIO_C01, 0x2, GPIO_PULL_NONE, GPIO_DRV_4X },	/* CMD */
		/*
		 * MMC0 is intended to be used for eMMC. The card detect
		 * pin is used as a VDDEN signal to power on the eMMC. The
		 * 5420 iROM makes this same assumption.
		 */
		{ GPIO_C02, GPIO_OUTPUT, GPIO_PULL_NONE, GPIO_DRV_4X },
		{ GPIO_C03, 0x2, GPIO_PULL_UP, GPIO_DRV_4X },	/* DATA[0] */
		{ GPIO_C04, 0x2, GPIO_PULL_UP, GPIO_DRV_4X },	/* DATA[1] */
		{ GPIO_C05, 0x2, GPIO_PULL_UP, GPIO_DRV_4X },	/* DATA[2] */
		{ GPIO_C06, 0x2, GPIO_PULL_UP, GPIO_DRV_4X },	/* DATA[3] */

		{ GPIO_C30, 0x2, GPIO_PULL_UP, GPIO_DRV_4X },	/* DATA[4] */
		{ GPIO_C31, 0x2, GPIO_PULL_UP, GPIO_DRV_4X },	/* DATA[5] */
		{ GPIO_C32, 0x2, GPIO_PULL_UP, GPIO_DRV_4X },	/* DATA[6] */
		{ GPIO_C33, 0x2, GPIO_PULL_UP, GPIO_DRV_4X },	/* DATA[7] */
	};

	exynos_pinmux_sdmmc(&gpios[0], ARRAY_SIZE(gpios));

	/* set VDDEN */
	gpio_set_value(GPIO_C02, 1);
}

void exynos_pinmux_sdmmc1(void)
{
	struct gpio gpios[] = {
		{ GPIO_C10, 0x2, GPIO_PULL_NONE, GPIO_DRV_4X },	/* CLK */
		{ GPIO_C11, 0x2, GPIO_PULL_NONE, GPIO_DRV_4X },	/* CMD */
		{ GPIO_C12, 0x2, GPIO_PULL_NONE, GPIO_DRV_4X },	/* CDn */
		{ GPIO_C13, 0x2, GPIO_PULL_NONE, GPIO_DRV_4X },	/* DATA[0] */
		{ GPIO_C14, 0x2, GPIO_PULL_NONE, GPIO_DRV_4X },	/* DATA[1] */
		{ GPIO_C15, 0x2, GPIO_PULL_NONE, GPIO_DRV_4X },	/* DATA[2] */
		{ GPIO_C16, 0x2, GPIO_PULL_NONE, GPIO_DRV_4X },	/* DATA[3] */

		{ GPIO_D14, 0x2, GPIO_PULL_UP, GPIO_DRV_4X },	/* DATA[4] */
		{ GPIO_D15, 0x2, GPIO_PULL_UP, GPIO_DRV_4X },	/* DATA[5] */
		{ GPIO_D16, 0x2, GPIO_PULL_UP, GPIO_DRV_4X },	/* DATA[6] */
		{ GPIO_D17, 0x2, GPIO_PULL_UP, GPIO_DRV_4X },	/* DATA[7] */
	};

	exynos_pinmux_sdmmc(&gpios[0], ARRAY_SIZE(gpios));
}

void exynos_pinmux_sdmmc2(void)
{
	struct gpio gpios[] = {
		{ GPIO_C20, 0x2, GPIO_PULL_NONE, GPIO_DRV_4X },	/* CLK */
		{ GPIO_C21, 0x2, GPIO_PULL_NONE, GPIO_DRV_4X },	/* CMD */
		{ GPIO_C22, 0x2, GPIO_PULL_NONE, GPIO_DRV_4X },	/* CDn */
		{ GPIO_C23, 0x2, GPIO_PULL_NONE, GPIO_DRV_4X },	/* DATA[0] */
		{ GPIO_C24, 0x2, GPIO_PULL_NONE, GPIO_DRV_4X },	/* DATA[1] */
		{ GPIO_C25, 0x2, GPIO_PULL_NONE, GPIO_DRV_4X },	/* DATA[2] */
		{ GPIO_C26, 0x2, GPIO_PULL_NONE, GPIO_DRV_4X },	/* DATA[3] */
	};

	exynos_pinmux_sdmmc(&gpios[0], ARRAY_SIZE(gpios));
}

static void exynos_pinmux_spi(int start, int cfg)
{
	int i;

	for (i = start; i < start + 4; i++) {
		gpio_cfg_pin(i, cfg);
		gpio_set_pull(i, GPIO_PULL_NONE);
		gpio_set_drv(i, GPIO_DRV_3X);
	}
}

void exynos_pinmux_spi0(void)
{
	exynos_pinmux_spi(GPIO_A20, 0x2);
}

void exynos_pinmux_spi1(void)
{
	exynos_pinmux_spi(GPIO_A24, 0x2);
}

void exynos_pinmux_spi2(void)
{
	exynos_pinmux_spi(GPIO_B11, 0x5);
}

void exynos_pinmux_spi3(void)
{
	exynos_pinmux_spi(GPIO_F10, 0x2);
}

void exynos_pinmux_spi4(void)
{
	int i;

	for (i = 0; i < 2; i++) {
		gpio_cfg_pin(GPIO_F02 + i, GPIO_FUNC(0x4));
		gpio_cfg_pin(GPIO_E04 + i, GPIO_FUNC(0x4));
	}
}

static void exynos_pinmux_i2c(int start, int func)
{
	gpio_cfg_pin(start, GPIO_FUNC(func));
	gpio_cfg_pin(start + 1, GPIO_FUNC(func));
	gpio_set_pull(start, GPIO_PULL_NONE);
	gpio_set_pull(start + 1, GPIO_PULL_NONE);
}

void exynos_pinmux_i2c0(void)
{
	exynos_pinmux_i2c(GPIO_B30, 0x2);
}

void exynos_pinmux_i2c1(void)
{
	exynos_pinmux_i2c(GPIO_B32, 0x2);
}

void exynos_pinmux_i2c2(void)
{
	exynos_pinmux_i2c(GPIO_A06, 0x3);
}

void exynos_pinmux_i2c3(void)
{
	exynos_pinmux_i2c(GPIO_A12, 0x3);
}

void exynos_pinmux_i2c4(void)
{
	exynos_pinmux_i2c(GPIO_A20, 0x3);
}

void exynos_pinmux_i2c5(void)
{
	exynos_pinmux_i2c(GPIO_A22, 0x3);
}

void exynos_pinmux_i2c6(void)
{
	exynos_pinmux_i2c(GPIO_B13, 0x4);
}

void exynos_pinmux_i2c7(void)
{
	exynos_pinmux_i2c(GPIO_B22, 0x3);
}

void exynos_pinmux_i2c8(void)
{
	exynos_pinmux_i2c(GPIO_B34, 0x2);
}

void exynos_pinmux_i2c9(void)
{
	exynos_pinmux_i2c(GPIO_B36, 0x2);
}

void exynos_pinmux_i2c10(void)
{
	exynos_pinmux_i2c(GPIO_B40, 0x2);
}

void exynos_pinmux_dphpd(void)
{
	gpio_cfg_pin(GPIO_X07, GPIO_FUNC(0x3));
	gpio_set_pull(GPIO_X07, GPIO_PULL_NONE);
}
