/*
 * (C) Copyright 2009 Samsung Electronics
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

/* FIXME(dhendrix): fix this up so it doesn't require a bunch of #ifdefs... */
#include <common.h>
#include <gpio.h>
//#include <arch/io.h>
#include <gpio.h>
#include <arch/gpio.h>
#include <console/console.h>
#include <cpu/samsung/s5p-common/gpio.h>
#include <cpu/samsung/exynos5-common/gpio.h>
#include <cpu/samsung/exynos5250/gpio.h>	/* FIXME: for gpio_decode_number prototype */

#define CON_MASK(x)		(0xf << ((x) << 2))
#define CON_SFR(x, v)		((v) << ((x) << 2))

#define DAT_MASK(x)		(0x1 << (x))
#define DAT_SET(x)		(0x1 << (x))

#define PULL_MASK(x)		(0x3 << ((x) << 1))
#define PULL_MODE(x, v)		((v) << ((x) << 1))

#define DRV_MASK(x)		(0x3 << ((x) << 1))
#define DRV_SET(x, m)		((m) << ((x) << 1))
#define RATE_MASK(x)		(0x1 << (x + 16))
#define RATE_SET(x)		(0x1 << (x + 16))

struct gpio_info {
	unsigned int reg_addr;	/* Address of register for this part */
	unsigned int max_gpio;	/* Maximum GPIO in this part */
};

#ifdef CONFIG_CPU_SAMSUNG_EXYNOS5

#include <cpu/samsung/exynos5250/cpu.h>
static const struct gpio_info gpio_data[EXYNOS_GPIO_NUM_PARTS] = {
	{ EXYNOS5_GPIO_PART1_BASE, GPIO_MAX_PORT_PART_1 },
	{ EXYNOS5_GPIO_PART2_BASE, GPIO_MAX_PORT_PART_2 },
	{ EXYNOS5_GPIO_PART3_BASE, GPIO_MAX_PORT_PART_3 },
	{ EXYNOS5_GPIO_PART4_BASE, GPIO_MAX_PORT_PART_4 },
	{ EXYNOS5_GPIO_PART5_BASE, GPIO_MAX_PORT_PART_5 },
	{ EXYNOS5_GPIO_PART6_BASE, GPIO_MAX_PORT },
};

#define HAVE_GENERIC_GPIO

#elif defined(CONFIG_CPU_SAMSUNG_EXYNOS4)

static const struct gpio_info gpio_data[EXYNOS_GPIO_NUM_PARTS] = {
	{ EXYNOS4_GPIO_PART1_BASE, GPIO_MAX_PORT_PART_1 },
	{ EXYNOS4_GPIO_PART2_BASE, GPIO_MAX_PORT_PART_2 },
	{ EXYNOS4_GPIO_PART3_BASE, GPIO_MAX_PORT_PART_3 },
};

#define HAVE_GENERIC_GPIO

#endif

/* This macro gets gpio pin offset from 0..7 */
#define GPIO_BIT(x)     ((x) & 0x7)

//#ifdef HAVE_GENERIC_GPIO
static struct s5p_gpio_bank *gpio_get_bank(unsigned int gpio)
{
	const struct gpio_info *data;
	unsigned int upto;
	int i;

	for (i = upto = 0, data = gpio_data; i < EXYNOS_GPIO_NUM_PARTS;
			i++, upto = data->max_gpio, data++) {
		if (gpio < data->max_gpio) {
			struct s5p_gpio_bank *bank;

			bank = (struct s5p_gpio_bank *)data->reg_addr;
			bank += (gpio - upto) / GPIO_PER_BANK;
			return bank;
		}
	}

#ifndef CONFIG_SPL_BUILD
	assert(gpio < GPIO_MAX_PORT);	/* ...which it will not be */
#endif
	return NULL;
}
//#endif

/* TODO: Deprecation this interface in favour of asm-generic/gpio.h */
void s5p_gpio_cfg_pin(struct s5p_gpio_bank *bank, int gpio, int cfg)
{
	unsigned int value;

	value = readl(&bank->con);
	value &= ~CON_MASK(gpio);
	value |= CON_SFR(gpio, cfg);
	writel(value, &bank->con);
}

void s5p_gpio_direction_output(struct s5p_gpio_bank *bank, int gpio, int en)
{
	unsigned int value;

	s5p_gpio_cfg_pin(bank, gpio, EXYNOS_GPIO_OUTPUT);

	value = readl(&bank->dat);
	value &= ~DAT_MASK(gpio);
	if (en)
		value |= DAT_SET(gpio);
	writel(value, &bank->dat);
}

void s5p_gpio_direction_input(struct s5p_gpio_bank *bank, int gpio)
{
	s5p_gpio_cfg_pin(bank, gpio, EXYNOS_GPIO_INPUT);
}

void s5p_gpio_set_value(struct s5p_gpio_bank *bank, int gpio, int en)
{
	unsigned int value;

	value = readl(&bank->dat);
	value &= ~DAT_MASK(gpio);
	if (en)
		value |= DAT_SET(gpio);
	writel(value, &bank->dat);
}

unsigned int s5p_gpio_get_value(struct s5p_gpio_bank *bank, int gpio)
{
	unsigned int value;

	value = readl(&bank->dat);
	return !!(value & DAT_MASK(gpio));
}

void s5p_gpio_set_pull(struct s5p_gpio_bank *bank, int gpio, int mode)
{
	unsigned int value;

	value = readl(&bank->pull);
	value &= ~PULL_MASK(gpio);

	switch (mode) {
	case EXYNOS_GPIO_PULL_DOWN:
	case EXYNOS_GPIO_PULL_UP:
		value |= PULL_MODE(gpio, mode);
		break;
	default:
		break;
	}

	writel(value, &bank->pull);
}

void s5p_gpio_set_drv(struct s5p_gpio_bank *bank, int gpio, int mode)
{
	unsigned int value;

	value = readl(&bank->drv);
	value &= ~DRV_MASK(gpio);

	switch (mode) {
	case EXYNOS_GPIO_DRV_1X:
	case EXYNOS_GPIO_DRV_2X:
	case EXYNOS_GPIO_DRV_3X:
	case EXYNOS_GPIO_DRV_4X:
		value |= DRV_SET(gpio, mode);
		break;
	default:
		return;
	}

	writel(value, &bank->drv);
}

void s5p_gpio_set_rate(struct s5p_gpio_bank *bank, int gpio, int mode)
{
	unsigned int value;

	value = readl(&bank->drv);
	value &= ~RATE_MASK(gpio);

	switch (mode) {
	case EXYNOS_GPIO_DRV_FAST:
	case EXYNOS_GPIO_DRV_SLOW:
		value |= RATE_SET(gpio);
		break;
	default:
		return;
	}

	writel(value, &bank->drv);
}

/* Common GPIO API - only available on Exynos5 */
/* FIXME(dhendrix): If this stuff is really only applicable to exynos5,
   move it to a more sensible location. */
#ifdef HAVE_GENERIC_GPIO

void gpio_cfg_pin(int gpio, int cfg)
{
	unsigned int value;
	struct s5p_gpio_bank *bank = gpio_get_bank(gpio);

	value = readl(&bank->con);
	value &= ~CON_MASK(GPIO_BIT(gpio));
	value |= CON_SFR(GPIO_BIT(gpio), cfg);
	writel(value, &bank->con);
}

static int gpio_get_cfg(int gpio)
{
	struct s5p_gpio_bank *bank = gpio_get_bank(gpio);
	int shift = GPIO_BIT(gpio) << 2;

	return (readl(&bank->con) & CON_MASK(GPIO_BIT(gpio))) >> shift;
}

void gpio_set_pull(int gpio, int mode)
{
	unsigned int value;
	struct s5p_gpio_bank *bank = gpio_get_bank(gpio);

	value = readl(&bank->pull);
	value &= ~PULL_MASK(GPIO_BIT(gpio));

	switch (mode) {
	case EXYNOS_GPIO_PULL_DOWN:
	case EXYNOS_GPIO_PULL_UP:
		value |= PULL_MODE(GPIO_BIT(gpio), mode);
		break;
	default:
		break;
	}

	writel(value, &bank->pull);
}

void gpio_set_drv(int gpio, int mode)
{
	unsigned int value;
	struct s5p_gpio_bank *bank = gpio_get_bank(gpio);

	value = readl(&bank->drv);
	value &= ~DRV_MASK(GPIO_BIT(gpio));

	switch (mode) {
	case EXYNOS_GPIO_DRV_1X:
	case EXYNOS_GPIO_DRV_2X:
	case EXYNOS_GPIO_DRV_3X:
	case EXYNOS_GPIO_DRV_4X:
		value |= DRV_SET(GPIO_BIT(gpio), mode);
		break;
	default:
		return;
	}

	writel(value, &bank->drv);
}

void gpio_set_rate(int gpio, int mode)
{
	unsigned int value;
	struct s5p_gpio_bank *bank = gpio_get_bank(gpio);

	value = readl(&bank->drv);
	value &= ~RATE_MASK(GPIO_BIT(gpio));

	switch (mode) {
	case EXYNOS_GPIO_DRV_FAST:
	case EXYNOS_GPIO_DRV_SLOW:
		value |= RATE_SET(GPIO_BIT(gpio));
		break;
	default:
		return;
	}

	writel(value, &bank->drv);
}

int gpio_request(unsigned gpio, const char *label)
{
	return 0;
}

int gpio_free(unsigned gpio)
{
	return 0;
}

int gpio_direction_input(unsigned gpio)
{
	gpio_cfg_pin(gpio, EXYNOS_GPIO_INPUT);

	return 0;
}

int gpio_direction_output(unsigned gpio, int value)
{
	unsigned int val;
	struct s5p_gpio_bank *bank = gpio_get_bank(gpio);

	gpio_cfg_pin(gpio, EXYNOS_GPIO_OUTPUT);

	val = readl(&bank->dat);
	val &= ~DAT_MASK(GPIO_BIT(gpio));
	if (value)
		val |= DAT_SET(GPIO_BIT(gpio));
	writel(val, &bank->dat);

	return 0;
}

int gpio_get_value(unsigned gpio)
{
	unsigned int value;
	struct s5p_gpio_bank *bank = gpio_get_bank(gpio);

	value = readl(&bank->dat);
	return !!(value & DAT_MASK(GPIO_BIT(gpio)));
}

int gpio_set_value(unsigned gpio, int value)
{
	unsigned int val;
	struct s5p_gpio_bank *bank = gpio_get_bank(gpio);

	val = readl(&bank->dat);
	val &= ~DAT_MASK(GPIO_BIT(gpio));
	if (value)
		val |= DAT_SET(GPIO_BIT(gpio));
	writel(val, &bank->dat);

	return 0;
}
#else

static int s5p_gpio_get_pin(unsigned gpio)
{
	return gpio % GPIO_PER_BANK;
}

/*
 * If we have the old-style GPIO numbering setup, use these functions
 * which don't necessary provide sequentially increasing GPIO numbers.
 */
static struct s5p_gpio_bank *s5p_gpio_get_bank(unsigned gpio)
{
	int bank = gpio / GPIO_PER_BANK;
	bank *= sizeof(struct s5p_gpio_bank);

	return (struct s5p_gpio_bank *) (s5p_gpio_base(gpio) + bank);
}

int gpio_request(unsigned gpio, const char *label)
{
	return 0;
}

int gpio_free(unsigned gpio)
{
	return 0;
}

int gpio_direction_input(unsigned gpio)
{
	s5p_gpio_direction_input(s5p_gpio_get_bank(gpio),
				s5p_gpio_get_pin(gpio));
	return 0;
}

int gpio_direction_output(unsigned gpio, int value)
{
	s5p_gpio_direction_output(s5p_gpio_get_bank(gpio),
				 s5p_gpio_get_pin(gpio), value);
	return 0;
}

int gpio_get_value(unsigned gpio)
{
	return (int) s5p_gpio_get_value(s5p_gpio_get_bank(gpio),
				       s5p_gpio_get_pin(gpio));
}

int gpio_set_value(unsigned gpio, int value)
{
	s5p_gpio_set_value(s5p_gpio_get_bank(gpio),
			  s5p_gpio_get_pin(gpio), value);

	return 0;
}

#endif /* HAVE_GENERIC_GPIO */

/*
 * Add a delay here to give the lines time to settle
 * TODO(sjg): 1us does not always work, 2 is stable, so use 5 to be safe
 * Come back to this and sort out what the datasheet says
 */
#define GPIO_DELAY_US 5

int gpio_read_mvl3(unsigned gpio)
{
	int high, low;
	enum mvl3 value;

	if (gpio >= GPIO_MAX_PORT)
		return -1;

	gpio_direction_input(gpio);
	gpio_set_pull(gpio, EXYNOS_GPIO_PULL_UP);
	udelay(GPIO_DELAY_US);
	high = gpio_get_value(gpio);
	gpio_set_pull(gpio, EXYNOS_GPIO_PULL_DOWN);
	udelay(GPIO_DELAY_US);
	low = gpio_get_value(gpio);

	if (high && low) /* external pullup */
		value = LOGIC_1;
	else if (!high && !low) /* external pulldown */
		value = LOGIC_0;
	else /* floating */
		value = LOGIC_Z;

	/*
	 * Check if line is externally pulled high and
	 * configure the internal pullup to match.  For
	 * floating and pulldowns, the GPIO is already
	 * configured with an internal pulldown from the
	 * above test.
	 */
	if (value == LOGIC_1)
		gpio_set_pull(gpio, EXYNOS_GPIO_PULL_UP);

	return value;
}

int gpio_decode_number(unsigned gpio_list[], int count)
{
	int result = 0;
	int multiplier = 1;
	int gpio, i, value;
	enum mvl3 mvl3;

	for (i = 0; i < count; i++) {
		gpio = gpio_list[i];

		mvl3 = gpio_read_mvl3(gpio);
		if (mvl3 == LOGIC_1)
			value = 2;
		else if (mvl3 == LOGIC_0)
			value = 1;
		else if (mvl3 == LOGIC_Z)
			value = 0;
		else
			return -1;

		result += value * multiplier;
		multiplier *= 3;
	}

	return result;
}

static const char *get_cfg_name(int cfg)
{
	static char name[8];

	if (cfg == EXYNOS_GPIO_INPUT)
		return "input";
	else if (cfg == EXYNOS_GPIO_OUTPUT)
		return "output";
	printk(BIOS_INFO, "func %d", cfg);
//	sprintf(name, "func %d", cfg);

	return name;
}

/*
 * Display Exynos GPIO information
 */
void gpio_info(void)
{
	unsigned gpio;

	for (gpio = 0; gpio < GPIO_MAX_PORT; gpio++) {
		int cfg = gpio_get_cfg(gpio);

		printk(BIOS_INFO, "GPIO_%-3d: %s", gpio, get_cfg_name(cfg));
		if (cfg == EXYNOS_GPIO_INPUT || cfg == EXYNOS_GPIO_OUTPUT)
			printk(BIOS_INFO, ", value = %d", gpio_get_value(gpio));
		printk(BIOS_INFO, "\n");
	}
}
