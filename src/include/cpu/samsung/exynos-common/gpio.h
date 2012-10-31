/*
 * (C) Copyright 2010 Samsung Electronics
 * Minkyu Kang <mk7.kang@samsung.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __ASM_ARCH_COMMON_GPIO_H
#define __ASM_ARCH_COMMON_GPIO_H

#ifndef __ASSEMBLY__
struct s5p_gpio_bank {
	unsigned int	con;
	unsigned int	dat;
	unsigned int	pull;
	unsigned int	drv;
	unsigned int	pdn_con;
	unsigned int	pdn_pull;
	unsigned char	res1[8];
};

/* functions */
void s5p_gpio_cfg_pin(struct s5p_gpio_bank *bank, int gpio, int cfg);
void s5p_gpio_direction_output(struct s5p_gpio_bank *bank, int gpio, int en);
void s5p_gpio_direction_input(struct s5p_gpio_bank *bank, int gpio);
void s5p_gpio_set_value(struct s5p_gpio_bank *bank, int gpio, int en);
unsigned int s5p_gpio_get_value(struct s5p_gpio_bank *bank, int gpio);
void s5p_gpio_set_pull(struct s5p_gpio_bank *bank, int gpio, int mode);
void s5p_gpio_set_drv(struct s5p_gpio_bank *bank, int gpio, int mode);
void s5p_gpio_set_rate(struct s5p_gpio_bank *bank, int gpio, int mode);

/* GPIO pins per bank  */
#define GPIO_PER_BANK 8

#endif

/* Pin configurations */
#define EXYNOS_GPIO_INPUT	0x0
#define EXYNOS_GPIO_OUTPUT	0x1
#define EXYNOS_GPIO_IRQ	0xf
#define EXYNOS_GPIO_FUNC(x)	(x)

/* Pull mode */
#define EXYNOS_GPIO_PULL_NONE	0x0
#define EXYNOS_GPIO_PULL_DOWN	0x1
#define EXYNOS_GPIO_PULL_UP	0x3

/* Drive Strength level */
#define EXYNOS_GPIO_DRV_1X	0x0
#define EXYNOS_GPIO_DRV_3X	0x1
#define EXYNOS_GPIO_DRV_2X	0x2
#define EXYNOS_GPIO_DRV_4X	0x3
#define EXYNOS_GPIO_DRV_FAST	0x0
#define EXYNOS_GPIO_DRV_SLOW	0x1
#endif
