/*
 * Copyright (c) 2012 Samsung Electronics.
 * Abhilash Kesavan <a.kesavan@samsung.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <arch/gpio.h>
#include <cpu/samsung/exynos5250/gpio.h>
#include <cpu/samsung/exynos5250/cpu.h>
#include <cpu/samsung/exynos5250/pinmux.h>
#include <cpu/samsung/exynos5-common/sromc.h>

int exynos_pinmux_config(enum periph_id peripheral, int flags)
{
	int i, start, count, start_ext, pin_ext, pin, drv;

	switch (peripheral) {
	case PERIPH_ID_UART0:
	case PERIPH_ID_UART1:
	case PERIPH_ID_UART2:
	case PERIPH_ID_UART3:
		switch (peripheral) {
		default:
		case PERIPH_ID_UART0:
			start = GPIO_A00; count = 4;
			break;
		case PERIPH_ID_UART1:
			start = GPIO_A04; count = 4;
			break;
		case PERIPH_ID_UART2:
			start = GPIO_A10; count = 4;
			break;
		case PERIPH_ID_UART3:
			start = GPIO_A14; count = 2;
			break;
		}
		for (i = start; i < start + count; i++) {
			gpio_set_pull(i, EXYNOS_GPIO_PULL_NONE);
			gpio_cfg_pin(i, EXYNOS_GPIO_FUNC(0x2));
		}
		break;
	case PERIPH_ID_SDMMC0:
	case PERIPH_ID_SDMMC1:
	case PERIPH_ID_SDMMC2:
	case PERIPH_ID_SDMMC3:
		pin = EXYNOS_GPIO_FUNC(0x2);
		pin_ext = EXYNOS_GPIO_FUNC(0x2);
		drv = EXYNOS_GPIO_DRV_4X;
		switch (peripheral) {
		default:
		case PERIPH_ID_SDMMC0:
			start = GPIO_C00;
			start_ext = GPIO_C10;
			break;
		case PERIPH_ID_SDMMC1:
			start = GPIO_C20;
			start_ext = 0;
			break;
		case PERIPH_ID_SDMMC2:
			start = GPIO_C30;
			/*
			 * TODO: (alim.akhtar@samsung.com)
			 * add support for 8 bit mode (needs to be a per-board
			 * option, so in the FDT).
			 */
			start_ext = 0;
			break;
		case PERIPH_ID_SDMMC3:
			/*
			 * TODO: Need to add definitions for GPC4 before
			 * enabling this.
			 */
			debug("SDMMC3 not supported yet");
			return -1;
		}
		if ((flags & PINMUX_FLAG_8BIT_MODE) && !start_ext) {
			debug("SDMMC device %d does not support 8bit mode",
					peripheral);
			return -1;
		}
		if (flags & PINMUX_FLAG_8BIT_MODE) {
			assert(peripheral == PERIPH_ID_SDMMC0);
			for (i = 0; i <= 3; i++) {
				gpio_cfg_pin(start_ext + i, pin_ext);
				gpio_set_pull(start_ext + i,
					      EXYNOS_GPIO_PULL_UP);
				gpio_set_drv(start_ext + i, drv);
			}
		}
		for (i = 0; i < 2; i++) {
			gpio_cfg_pin(start + i, pin);
			gpio_set_pull(start + i, EXYNOS_GPIO_PULL_NONE);
			gpio_set_drv(start + i, drv);
		}
		for (i = 2; i <= 6; i++) {
			gpio_cfg_pin(start + i, pin);
			gpio_set_pull(start + i, EXYNOS_GPIO_PULL_UP);
			gpio_set_drv(start + i, drv);
		}
		break;
	case PERIPH_ID_SROMC:
		/*
		 * SROM:CS1 and EBI
		 *
		 * GPY0[0]	SROM_CSn[0]
		 * GPY0[1]	SROM_CSn[1](2)
		 * GPY0[2]	SROM_CSn[2]
		 * GPY0[3]	SROM_CSn[3]
		 * GPY0[4]	EBI_OEn(2)
		 * GPY0[5]	EBI_EEn(2)
		 *
		 * GPY1[0]	EBI_BEn[0](2)
		 * GPY1[1]	EBI_BEn[1](2)
		 * GPY1[2]	SROM_WAIT(2)
		 * GPY1[3]	EBI_DATA_RDn(2)
		 */
		gpio_cfg_pin(GPIO_Y00 + (flags & PINMUX_FLAG_BANK),
				EXYNOS_GPIO_FUNC(2));
		gpio_cfg_pin(GPIO_Y04, EXYNOS_GPIO_FUNC(2));
		gpio_cfg_pin(GPIO_Y05, EXYNOS_GPIO_FUNC(2));

		for (i = 2; i < 4; i++)
			gpio_cfg_pin(GPIO_Y10 + i, EXYNOS_GPIO_FUNC(2));

		/*
		 * EBI: 8 Address Lines
		 *
		 * GPY3[0]	EBI_ADDR[0](2)
		 * GPY3[1]	EBI_ADDR[1](2)
		 * GPY3[2]	EBI_ADDR[2](2)
		 * GPY3[3]	EBI_ADDR[3](2)
		 * GPY3[4]	EBI_ADDR[4](2)
		 * GPY3[5]	EBI_ADDR[5](2)
		 * GPY3[6]	EBI_ADDR[6](2)
		 * GPY3[7]	EBI_ADDR[7](2)
		 *
		 * EBI: 16 Data Lines
		 *
		 * GPY5[0]	EBI_DATA[0](2)
		 * GPY5[1]	EBI_DATA[1](2)
		 * GPY5[2]	EBI_DATA[2](2)
		 * GPY5[3]	EBI_DATA[3](2)
		 * GPY5[4]	EBI_DATA[4](2)
		 * GPY5[5]	EBI_DATA[5](2)
		 * GPY5[6]	EBI_DATA[6](2)
		 * GPY5[7]	EBI_DATA[7](2)
		 *
		 * GPY6[0]	EBI_DATA[8](2)
		 * GPY6[1]	EBI_DATA[9](2)
		 * GPY6[2]	EBI_DATA[10](2)
		 * GPY6[3]	EBI_DATA[11](2)
		 * GPY6[4]	EBI_DATA[12](2)
		 * GPY6[5]	EBI_DATA[13](2)
		 * GPY6[6]	EBI_DATA[14](2)
		 * GPY6[7]	EBI_DATA[15](2)
		 */
		for (i = 0; i < 8; i++) {
			gpio_cfg_pin(GPIO_Y30 + i, EXYNOS_GPIO_FUNC(2));
			gpio_set_pull(GPIO_Y30 + i, EXYNOS_GPIO_PULL_UP);

			gpio_cfg_pin(GPIO_Y50 + i, EXYNOS_GPIO_FUNC(2));
			gpio_set_pull(GPIO_Y50 + i, EXYNOS_GPIO_PULL_UP);

			if (flags & PINMUX_FLAG_16BIT) {
				gpio_cfg_pin(GPIO_Y60 + i, EXYNOS_GPIO_FUNC(2));
				gpio_set_pull(GPIO_Y60 + i,
					      EXYNOS_GPIO_PULL_UP);
			}
		}
		break;
	case PERIPH_ID_SPI0:
	case PERIPH_ID_SPI1:
	case PERIPH_ID_SPI2:
	case PERIPH_ID_SPI3: {
		int cfg;

		switch (peripheral) {
		default:
		case PERIPH_ID_SPI0:
			start = GPIO_A20;
			cfg = 0x2;
			break;
		case PERIPH_ID_SPI1:
			start = GPIO_A24;
			cfg = 0x2;
			break;
		case PERIPH_ID_SPI2:
			start = GPIO_B11;
			cfg = 0x5;
			break;
		case PERIPH_ID_SPI3:
			start = GPIO_E00;
			cfg = 0x2;
			break;
		}

		for (i = 0; i < 4; i++)
			gpio_cfg_pin(start + i, EXYNOS_GPIO_FUNC(cfg));
		break;
	}
	case PERIPH_ID_SPI4:
		for (i = 0; i < 2; i++)
			gpio_cfg_pin(GPIO_F02 + i, EXYNOS_GPIO_FUNC(0x4));
		for (i = 2; i < 4; i++)
			gpio_cfg_pin(GPIO_E02 + i, EXYNOS_GPIO_FUNC(0x4));
		break;
	case PERIPH_ID_BACKLIGHT:
		gpio_cfg_pin(GPIO_B20, EXYNOS_GPIO_OUTPUT);
		gpio_set_value(GPIO_B20, 1);
		break;
	case PERIPH_ID_LCD:
		gpio_cfg_pin(GPIO_Y25, EXYNOS_GPIO_OUTPUT);
		gpio_set_value(GPIO_Y25, 1);
		gpio_cfg_pin(GPIO_X15, EXYNOS_GPIO_OUTPUT);
		gpio_set_value(GPIO_X15, 1);
		gpio_cfg_pin(GPIO_X30, EXYNOS_GPIO_OUTPUT);
		gpio_set_value(GPIO_X30, 1);
		break;
	case PERIPH_ID_I2C0:
		gpio_cfg_pin(GPIO_B30, EXYNOS_GPIO_FUNC(0x2));
		gpio_cfg_pin(GPIO_B31, EXYNOS_GPIO_FUNC(0x2));
		gpio_set_pull(GPIO_B30, EXYNOS_GPIO_PULL_NONE);
		gpio_set_pull(GPIO_B31, EXYNOS_GPIO_PULL_NONE);
		break;
	case PERIPH_ID_I2C1:
		gpio_cfg_pin(GPIO_B32, EXYNOS_GPIO_FUNC(0x2));
		gpio_cfg_pin(GPIO_B33, EXYNOS_GPIO_FUNC(0x2));
		gpio_set_pull(GPIO_B32, EXYNOS_GPIO_PULL_NONE);
		gpio_set_pull(GPIO_B33, EXYNOS_GPIO_PULL_NONE);
		break;
	case PERIPH_ID_I2C2:
		gpio_cfg_pin(GPIO_A06, EXYNOS_GPIO_FUNC(0x3));
		gpio_cfg_pin(GPIO_A07, EXYNOS_GPIO_FUNC(0x3));
		gpio_set_pull(GPIO_A06, EXYNOS_GPIO_PULL_NONE);
		gpio_set_pull(GPIO_A07, EXYNOS_GPIO_PULL_NONE);
		break;
	case PERIPH_ID_I2C3:
		gpio_cfg_pin(GPIO_A12, EXYNOS_GPIO_FUNC(0x3));
		gpio_cfg_pin(GPIO_A13, EXYNOS_GPIO_FUNC(0x3));
		gpio_set_pull(GPIO_A12, EXYNOS_GPIO_PULL_NONE);
		gpio_set_pull(GPIO_A13, EXYNOS_GPIO_PULL_NONE);
		break;
	case PERIPH_ID_I2C4:
		gpio_cfg_pin(GPIO_A20, EXYNOS_GPIO_FUNC(0x3));
		gpio_cfg_pin(GPIO_A21, EXYNOS_GPIO_FUNC(0x3));
		gpio_set_pull(GPIO_A20, EXYNOS_GPIO_PULL_NONE);
		gpio_set_pull(GPIO_A21, EXYNOS_GPIO_PULL_NONE);
		break;
	case PERIPH_ID_I2C5:
		gpio_cfg_pin(GPIO_A22, EXYNOS_GPIO_FUNC(0x3));
		gpio_cfg_pin(GPIO_A23, EXYNOS_GPIO_FUNC(0x3));
		gpio_set_pull(GPIO_A22, EXYNOS_GPIO_PULL_NONE);
		gpio_set_pull(GPIO_A23, EXYNOS_GPIO_PULL_NONE);
		break;
	case PERIPH_ID_I2C6:
		gpio_cfg_pin(GPIO_B13, EXYNOS_GPIO_FUNC(0x4));
		gpio_cfg_pin(GPIO_B14, EXYNOS_GPIO_FUNC(0x4));
		break;
	case PERIPH_ID_I2C7:
		gpio_cfg_pin(GPIO_B22, EXYNOS_GPIO_FUNC(0x3));
		gpio_cfg_pin(GPIO_B23, EXYNOS_GPIO_FUNC(0x3));
		gpio_set_pull(GPIO_B22, EXYNOS_GPIO_PULL_NONE);
		gpio_set_pull(GPIO_B23, EXYNOS_GPIO_PULL_NONE);
		break;
	case PERIPH_ID_DPHPD:
		/* Set Hotplug detect for DP */
		gpio_cfg_pin(GPIO_X07, EXYNOS_GPIO_FUNC(0x3));

		/*
		 * Hotplug detect should have an external pullup; disable the
		 * internal pulldown so they don't fight.
		 */
		gpio_set_pull(GPIO_X07, EXYNOS_GPIO_PULL_NONE);
		break;
	case PERIPH_ID_I2S1:
		for (i = 0; i < 5; i++)
			gpio_cfg_pin(GPIO_B00 + i, EXYNOS_GPIO_FUNC(0x02));
		break;
	default:
		debug("%s: invalid peripheral %d", __func__, peripheral);
		return -1;
	}

	return 0;
}
