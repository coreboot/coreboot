/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef CPU_SAMSUNG_EXYNOS5250_PINMUX_H
#define CPU_SAMSUNG_EXYNOS5250_PINMUX_H

void exynos_pinmux_uart0(void);
void exynos_pinmux_uart1(void);
void exynos_pinmux_uart2(void);
void exynos_pinmux_uart3(void);

void exynos_pinmux_sdmmc0(void);
void exynos_pinmux_sdmmc1(void);
void exynos_pinmux_sdmmc2(void);
void exynos_pinmux_sdmmc3(void);

void exynos_pinmux_sromc(int bank, int sixteen_bit);

void exynos_pinmux_spi0(void);
void exynos_pinmux_spi1(void);
void exynos_pinmux_spi2(void);
void exynos_pinmux_spi3(void);
void exynos_pinmux_spi4(void);

void exynos_pinmux_backlight(void);
void exynos_pinmux_lcd(void);

void exynos_pinmux_i2c0(void);
void exynos_pinmux_i2c1(void);
void exynos_pinmux_i2c2(void);
void exynos_pinmux_i2c3(void);
void exynos_pinmux_i2c4(void);
void exynos_pinmux_i2c5(void);
void exynos_pinmux_i2c6(void);
void exynos_pinmux_i2c7(void);

void exynos_pinmux_dphpd(void);

void exynos_pinmux_i2s0(void);
void exynos_pinmux_i2s1(void);

#endif
