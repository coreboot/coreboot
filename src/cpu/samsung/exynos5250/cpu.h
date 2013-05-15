/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Samsung Electronics
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

#ifndef CPU_SAMSUNG_EXYNOS5250_CPU_H
#define CPU_SAMSUNG_EXYNOS5250_CPU_H

#include <arch/io.h>

#define DEVICE_NOT_AVAILABLE		0

#define EXYNOS_PRO_ID			0x10000000

/* Address of address of function that copys data from SD or MMC */
#define EXYNOS_COPY_MMC_FNPTR_ADDR	0x02020030

/* Address of address of function that copys data from SPI */
#define EXYNOS_COPY_SPI_FNPTR_ADDR	0x02020058

/* Address of address of function that copys data through USB */
#define EXYNOS_COPY_USB_FNPTR_ADDR	0x02020070

/* Boot mode values */
#define EXYNOS_USB_SECONDARY_BOOT	0xfeed0002

#define EXYNOS_IRAM_SECONDARY_BASE	0x02020018

#define EXYNOS_I2C_SPACING		0x10000

/* EXYNOS5 */
#define EXYNOS5_GPIO_PART6_BASE		0x03860000	/* Z<6:0> */
#define EXYNOS5_PRO_ID			0x10000000
#define EXYNOS5_CLOCK_BASE		0x10010000
#define EXYNOS5_POWER_BASE		0x10040000
#define EXYNOS5_SWRESET			0x10040400
#define EXYNOS5_SYSREG_BASE		0x10050000
#define EXYNOS5_TZPC1_DECPROT1SET	0x10110810
#define EXYNOS5_MULTI_CORE_TIMER_BASE	0x101C0000
#define EXYNOS5_WATCHDOG_BASE		0x101D0000
#define EXYNOS5_ACE_SFR_BASE            0x10830000
#define EXYNOS5_DMC_PHY0_BASE		0x10C00000
#define EXYNOS5_DMC_PHY1_BASE		0x10C10000
#define EXYNOS5_GPIO_PART4_BASE		0x10D10000	/* V00..V37 */
#define EXYNOS5_GPIO_PART5_BASE		0x10D100C0	/* V40..V47 */
#define EXYNOS5_DMC_CTRL_BASE		0x10DD0000
#define EXYNOS5_GPIO_PART1_BASE		0x11400000	/* A00..Y67 */
#define EXYNOS5_GPIO_PART2_BASE		0x11400c00	/* X00..X37 */
#define EXYNOS5_USB_HOST_EHCI_BASE	0x12110000
#define EXYNOS5_USBPHY_BASE		0x12130000
#define EXYNOS5_USBOTG_BASE		0x12140000

#define EXYNOS5_MMC_BASE		0x12200000
#define EXYNOS5_MSHC_BASE		0x12240000

#define EXYNOS5_SROMC_BASE		0x12250000
#define EXYNOS5_UART_BASE		0x12C00000

#define EXYNOS5_SPI1_BASE		0x12D30000
#define EXYNOS5_I2C_BASE		0x12C60000
#define EXYNOS5_SPI_BASE		0x12D20000
#define EXYNOS5_PWMTIMER_BASE		0x12DD0000
#define EXYNOS5_SPI_ISP_BASE		0x131A0000
#define EXYNOS5_I2S_BASE		0x12D60000
#define EXYNOS5_GPIO_PART3_BASE		0x13400000	/* E00..H17 */
#define EXYNOS5_FIMD_BASE		0x14400000
#define EXYNOS5_DISP1_CTRL_BASE		0x14420000
#define EXYNOS5_MIPI_DSI1_BASE		0x14500000

#define EXYNOS5_ADC_BASE		DEVICE_NOT_AVAILABLE
#define EXYNOS5_MODEM_BASE		DEVICE_NOT_AVAILABLE

/* Compatibility defines */
#define EXYNOS_POWER_BASE		EXYNOS5_POWER_BASE

/* Marker values stored at the bottom of IRAM stack by SPL */
#define EXYNOS5_SPL_MARKER	0xb004f1a9	/* hexspeak word: bootflag */

/* Distance between each Trust Zone PC register set */
#define TZPC_BASE_OFFSET		0x10000

#define samsung_get_base_adc() ((struct exynos5_adc *)EXYNOS5_ADC_BASE)
#define samsung_get_base_clock() ((struct exynos5_clock *)EXYNOS5_CLOCK_BASE)
#define samsung_get_base_ace_sfr() ((struct exynos5_ace_sfr *)EXYNOS5_ACE_SFR_BASE)
#define samsung_get_base_dsim() ((struct exynos5_dsim *)EXYNOS5_MIPI_DSI1_BASE)
#define samsung_get_base_disp_ctrl() ((struct exynos5_disp_ctrl *)EXYNOS5_DISP1_CTRL_BASE)
#define samsung_get_base_fimd() ((struct exynos5_fimd *)EXYNOS5_FIMD_BASE)
#define samsung_get_base_pro_id() ((struct exynos5_pro_id *)EXYNOS5_PRO_ID)

#define samsung_get_base_mmc() ((struct exynos5_mmc *)EXYNOS5_MMC_BASE)
#define samsung_get_base_mshci() ((struct exynos5_mshci *)EXYNOS5_MSHC_BASE)

#define samsung_get_base_modem() ((struct exynos5_modem *)EXYNOS5_MODEM_BASE)
#define samsung_get_base_sromc() ((struct exynos5_sromc *)EXYNOS5_SROMC_BASE)
#define samsung_get_base_swreset() ((struct exynos5_swreset *)EXYNOS5_SWRESET)
#define samsung_get_base_sysreg() ((struct exynos5_sysreg *)EXYNOS5_SYSREG_BASE)
#define samsung_get_base_timer() ((struct s5p_timer *)EXYNOS5_PWMTIMER_BASE)
#define samsung_get_base_uart() ((struct exynos5_uart *)EXYNOS5_UART_BASE)
#define samsung_get_base_usb_phy() ((struct exynos5_usb_phy *)EXYNOS5_USBPHY_BASE)
#define samsung_get_base_usb_otg() ((struct exynos5_usb_otg *)EXYNOS5_USBOTG_BASE)
#define samsung_get_base_watchdog() ((struct exynos5_watchdog *)EXYNOS5_WATCHDOG_BASE)
#define samsung_get_base_power() ((struct exynos5_power *)EXYNOS5_POWER_BASE)
#define samsung_get_base_i2s() ((struct exynos5_i2s *)EXYNOS5_I2S_BASE)
#define samsung_get_base_spi1() ((struct exynos5_spi1 *)EXYNOS5_SPI1_BASE)
#define samsung_get_base_i2c() ((struct exynos5_i2c *)EXYNOS5_I2C_BASE)
#define samsung_get_base_spi() ((struct exynos5_spi *)EXYNOS5_SPI_BASE)
#define samsung_get_base_spi_isp() ((struct exynos5_spi_isp *)EXYNOS5_SPI_ISP_BASE)

#define EXYNOS5_SPI_NUM_CONTROLLERS	5
#define EXYNOS_I2C_MAX_CONTROLLERS	8

void exynos5250_config_l2_cache(void);

extern struct tmu_info exynos5250_tmu_info;

#endif	/* _EXYNOS5250_CPU_H */
