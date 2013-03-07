/*
 * (C) Copyright 2009-2010 Samsung Electronics
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

#ifndef __ASSEMBLER__	/* FIXME: not needed (i hope)? */

#include <cpu/samsung/exynos5-common/cpu.h>	/* FIXME: for S5PC110_GPIO_BASE */

struct s5p_gpio_bank {
	unsigned int	con;
	unsigned int	dat;
	unsigned int	pull;
	unsigned int	drv;
	unsigned int	pdn_con;
	unsigned int	pdn_pull;
	unsigned char	res1[8];
};

struct s5pc100_gpio {
	struct s5p_gpio_bank a0;
	struct s5p_gpio_bank a1;
	struct s5p_gpio_bank b;
	struct s5p_gpio_bank c;
	struct s5p_gpio_bank d;
	struct s5p_gpio_bank e0;
	struct s5p_gpio_bank e1;
	struct s5p_gpio_bank f0;
	struct s5p_gpio_bank f1;
	struct s5p_gpio_bank f2;
	struct s5p_gpio_bank f3;
	struct s5p_gpio_bank g0;
	struct s5p_gpio_bank g1;
	struct s5p_gpio_bank g2;
	struct s5p_gpio_bank g3;
	struct s5p_gpio_bank i;
	struct s5p_gpio_bank j0;
	struct s5p_gpio_bank j1;
	struct s5p_gpio_bank j2;
	struct s5p_gpio_bank j3;
	struct s5p_gpio_bank j4;
	struct s5p_gpio_bank k0;
	struct s5p_gpio_bank k1;
	struct s5p_gpio_bank k2;
	struct s5p_gpio_bank k3;
	struct s5p_gpio_bank l0;
	struct s5p_gpio_bank l1;
	struct s5p_gpio_bank l2;
	struct s5p_gpio_bank l3;
	struct s5p_gpio_bank l4;
	struct s5p_gpio_bank h0;
	struct s5p_gpio_bank h1;
	struct s5p_gpio_bank h2;
	struct s5p_gpio_bank h3;
};

struct s5pc110_gpio {
	struct s5p_gpio_bank a0;
	struct s5p_gpio_bank a1;
	struct s5p_gpio_bank b;
	struct s5p_gpio_bank c0;
	struct s5p_gpio_bank c1;
	struct s5p_gpio_bank d0;
	struct s5p_gpio_bank d1;
	struct s5p_gpio_bank e0;
	struct s5p_gpio_bank e1;
	struct s5p_gpio_bank f0;
	struct s5p_gpio_bank f1;
	struct s5p_gpio_bank f2;
	struct s5p_gpio_bank f3;
	struct s5p_gpio_bank g0;
	struct s5p_gpio_bank g1;
	struct s5p_gpio_bank g2;
	struct s5p_gpio_bank g3;
	struct s5p_gpio_bank i;
	struct s5p_gpio_bank j0;
	struct s5p_gpio_bank j1;
	struct s5p_gpio_bank j2;
	struct s5p_gpio_bank j3;
	struct s5p_gpio_bank j4;
	struct s5p_gpio_bank mp0_1;
	struct s5p_gpio_bank mp0_2;
	struct s5p_gpio_bank mp0_3;
	struct s5p_gpio_bank mp0_4;
	struct s5p_gpio_bank mp0_5;
	struct s5p_gpio_bank mp0_6;
	struct s5p_gpio_bank mp0_7;
	struct s5p_gpio_bank mp1_0;
	struct s5p_gpio_bank mp1_1;
	struct s5p_gpio_bank mp1_2;
	struct s5p_gpio_bank mp1_3;
	struct s5p_gpio_bank mp1_4;
	struct s5p_gpio_bank mp1_5;
	struct s5p_gpio_bank mp1_6;
	struct s5p_gpio_bank mp1_7;
	struct s5p_gpio_bank mp1_8;
	struct s5p_gpio_bank mp2_0;
	struct s5p_gpio_bank mp2_1;
	struct s5p_gpio_bank mp2_2;
	struct s5p_gpio_bank mp2_3;
	struct s5p_gpio_bank mp2_4;
	struct s5p_gpio_bank mp2_5;
	struct s5p_gpio_bank mp2_6;
	struct s5p_gpio_bank mp2_7;
	struct s5p_gpio_bank mp2_8;
	struct s5p_gpio_bank res1[48];
	struct s5p_gpio_bank h0;
	struct s5p_gpio_bank h1;
	struct s5p_gpio_bank h2;
	struct s5p_gpio_bank h3;
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

static inline unsigned int s5p_gpio_base(int nr)
{
	return S5PC110_GPIO_BASE;
}

#define s5pc110_gpio_get_nr(bank, pin) \
	((((((unsigned int)&(((struct s5pc110_gpio *)S5PC110_GPIO_BASE)->bank))\
	    - S5PC110_GPIO_BASE) / sizeof(struct s5p_gpio_bank)) \
	  * GPIO_PER_BANK) + pin)
#endif

/* Pin configurations */
#define GPIO_INPUT	0x0
#define GPIO_OUTPUT	0x1
#define GPIO_IRQ	0xf
#define GPIO_FUNC(x)	(x)

/* Pull mode */
#define GPIO_PULL_NONE	0x0
#define GPIO_PULL_DOWN	0x1
#define GPIO_PULL_UP	0x2

/* Drive Strength level */
#define GPIO_DRV_1X	0x0
#define GPIO_DRV_3X	0x1
#define GPIO_DRV_2X	0x2
#define GPIO_DRV_4X	0x3
#define GPIO_DRV_FAST	0x0
#define GPIO_DRV_SLOW	0x1

#if 0
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
#endif

/* GPIO pins per bank  */
#define GPIO_PER_BANK 8

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

#define EXYNOS5_GPIO_BASE0	0x11400000
#define EXYNOS5_GPIO_BASE1	0x13400000
#define EXYNOS5_GPIO_BASE2	0x10d10000
#define EXYNOS5_GPIO_BASE3	0x03860000

enum exynos5_gpio_port {
	/*
	 * Ordered by base address + offset.
	 * ETC registers are special, thus not included.
	 */

	/* base == EXYNOS_GPIO_BASE0 */
	EXYNOS5_GPA0 = EXYNOS5_GPIO_BASE0 + 0x0000,
	EXYNOS5_GPA1 = EXYNOS5_GPIO_BASE0 + 0x0020,
	EXYNOS5_GPA2 = EXYNOS5_GPIO_BASE0 + 0x0040,

	EXYNOS5_GPB0 = EXYNOS5_GPIO_BASE0 + 0x0060,
	EXYNOS5_GPB1 = EXYNOS5_GPIO_BASE0 + 0x0080,
	EXYNOS5_GPB2 = EXYNOS5_GPIO_BASE0 + 0x00a0,
	EXYNOS5_GPB3 = EXYNOS5_GPIO_BASE0 + 0x00c0,

	EXYNOS5_GPC0 = EXYNOS5_GPIO_BASE0 + 0x00e0,
	EXYNOS5_GPC1 = EXYNOS5_GPIO_BASE0 + 0x0100,
	EXYNOS5_GPC2 = EXYNOS5_GPIO_BASE0 + 0x0120,
	EXYNOS5_GPC3 = EXYNOS5_GPIO_BASE0 + 0x0140,

	EXYNOS5_GPD0 = EXYNOS5_GPIO_BASE0 + 0x0160,
	EXYNOS5_GPD1 = EXYNOS5_GPIO_BASE0 + 0x0180,

	EXYNOS5_GPY0 = EXYNOS5_GPIO_BASE0 + 0x01a0,
	EXYNOS5_GPY1 = EXYNOS5_GPIO_BASE0 + 0x01c0,
	EXYNOS5_GPY2 = EXYNOS5_GPIO_BASE0 + 0x01e0,
	EXYNOS5_GPY3 = EXYNOS5_GPIO_BASE0 + 0x0200,
	EXYNOS5_GPY4 = EXYNOS5_GPIO_BASE0 + 0x0220,
	EXYNOS5_GPY5 = EXYNOS5_GPIO_BASE0 + 0x0240,
	EXYNOS5_GPY6 = EXYNOS5_GPIO_BASE0 + 0x0260,

	EXYNOS5_GPX0 = EXYNOS5_GPIO_BASE0 + 0x0c00,
	EXYNOS5_GPX1 = EXYNOS5_GPIO_BASE0 + 0x0c20,
	EXYNOS5_GPX2 = EXYNOS5_GPIO_BASE0 + 0x0c40,
	EXYNOS5_GPX3 = EXYNOS5_GPIO_BASE0 + 0x0c60,

	/* base == EXYNOS_GPIO_BASE1 */
	EXYNOS5_GPE0 = EXYNOS5_GPIO_BASE1 + 0x0000,
	EXYNOS5_GPE1 = EXYNOS5_GPIO_BASE1 + 0x0020,

	EXYNOS5_GPF0 = EXYNOS5_GPIO_BASE1 + 0x0040,
	EXYNOS5_GPF1 = EXYNOS5_GPIO_BASE1 + 0x0060,

	EXYNOS5_GPG0 = EXYNOS5_GPIO_BASE1 + 0x0080,
	EXYNOS5_GPG1 = EXYNOS5_GPIO_BASE1 + 0x00a0,
	EXYNOS5_GPG2 = EXYNOS5_GPIO_BASE1 + 0x00c0,

	EXYNOS5_GPH0 = EXYNOS5_GPIO_BASE1 + 0x00e0,
	EXYNOS5_GPH1 = EXYNOS5_GPIO_BASE1 + 0x0100,

	/* base == EXYNOS_GPIO_BASE2 */
	EXYNOS5_GPV0 = EXYNOS5_GPIO_BASE2 + 0x0000,
	EXYNOS5_GPV1 = EXYNOS5_GPIO_BASE2 + 0x0020,
	EXYNOS5_GPV2 = EXYNOS5_GPIO_BASE2 + 0x0060,
	EXYNOS5_GPV3 = EXYNOS5_GPIO_BASE2 + 0x0080,
	EXYNOS5_GPV4 = EXYNOS5_GPIO_BASE2 + 0x00c0,

	/* base == EXYNOS_GPIO_BASE3 */
	EXYNOS5_GPZ = EXYNOS5_GPIO_BASE3 + 0x0000,
};

#endif
