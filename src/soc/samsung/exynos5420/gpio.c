/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <device/mmio.h>
#include <delay.h>
#include <soc/cpu.h>
#include <soc/gpio.h>

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

static const struct gpio_info gpio_data[EXYNOS_GPIO_NUM_PARTS] = {
	{ EXYNOS5420_GPIO_PART1_BASE, GPIO_MAX_PORT_PART_1 },
	{ EXYNOS5420_GPIO_PART2_BASE, GPIO_MAX_PORT_PART_2 },
	{ EXYNOS5420_GPIO_PART3_BASE, GPIO_MAX_PORT_PART_3 },
	{ EXYNOS5420_GPIO_PART4_BASE, GPIO_MAX_PORT_PART_4 },
	{ EXYNOS5420_GPIO_PART5_BASE, GPIO_MAX_PORT_PART_5 },
	{ EXYNOS5420_GPIO_PART6_BASE, GPIO_MAX_PORT },
};

/* This macro gets gpio pin offset from 0..7 */
#define GPIO_BIT(x)     ((x) & 0x7)

static struct gpio_bank *gpio_get_bank(unsigned int gpio)
{
	const struct gpio_info *data;
	unsigned int upto;
	int i;

	for (i = upto = 0, data = gpio_data; i < EXYNOS_GPIO_NUM_PARTS;
			i++, upto = data->max_gpio, data++) {
		if (gpio < data->max_gpio) {
			struct gpio_bank *bank;

			bank = (struct gpio_bank *)data->reg_addr;
			bank += (gpio - upto) / GPIO_PER_BANK;
			return bank;
		}
	}

	ASSERT(gpio < GPIO_MAX_PORT);	/* ...which it will not be */
	return NULL;
}

/* Common GPIO API - only available on Exynos5 */
void gpio_cfg_pin(int gpio, int cfg)
{
	unsigned int value;
	struct gpio_bank *bank = gpio_get_bank(gpio);

	value = read32(&bank->con);
	value &= ~CON_MASK(GPIO_BIT(gpio));
	value |= CON_SFR(GPIO_BIT(gpio), cfg);
	write32(&bank->con, value);
}

static int gpio_get_cfg(int gpio)
{
	struct gpio_bank *bank = gpio_get_bank(gpio);
	int shift = GPIO_BIT(gpio) << 2;

	return (read32(&bank->con) & CON_MASK(GPIO_BIT(gpio))) >> shift;
}

void gpio_set_pull(int gpio, int mode)
{
	unsigned int value;
	struct gpio_bank *bank = gpio_get_bank(gpio);

	value = read32(&bank->pull);
	value &= ~PULL_MASK(GPIO_BIT(gpio));

	switch (mode) {
	case GPIO_PULL_DOWN:
	case GPIO_PULL_UP:
		value |= PULL_MODE(GPIO_BIT(gpio), mode);
		break;
	default:
		break;
	}

	write32(&bank->pull, value);
}

void gpio_set_drv(int gpio, int mode)
{
	unsigned int value;
	struct gpio_bank *bank = gpio_get_bank(gpio);

	value = read32(&bank->drv);
	value &= ~DRV_MASK(GPIO_BIT(gpio));

	switch (mode) {
	case GPIO_DRV_1X:
	case GPIO_DRV_2X:
	case GPIO_DRV_3X:
	case GPIO_DRV_4X:
		value |= DRV_SET(GPIO_BIT(gpio), mode);
		break;
	default:
		return;
	}

	write32(&bank->drv, value);
}

void gpio_set_rate(int gpio, int mode)
{
	unsigned int value;
	struct gpio_bank *bank = gpio_get_bank(gpio);

	value = read32(&bank->drv);
	value &= ~RATE_MASK(GPIO_BIT(gpio));

	switch (mode) {
	case GPIO_DRV_FAST:
	case GPIO_DRV_SLOW:
		value |= RATE_SET(GPIO_BIT(gpio));
		break;
	default:
		return;
	}

	write32(&bank->drv, value);
}

int gpio_direction_input(unsigned int gpio)
{
	gpio_cfg_pin(gpio, GPIO_INPUT);

	return 0;
}

int gpio_direction_output(unsigned int gpio, int value)
{
	unsigned int val;
	struct gpio_bank *bank = gpio_get_bank(gpio);

	val = read32(&bank->dat);
	val &= ~DAT_MASK(GPIO_BIT(gpio));
	if (value)
		val |= DAT_SET(GPIO_BIT(gpio));
	write32(&bank->dat, val);

	gpio_cfg_pin(gpio, GPIO_OUTPUT);

	return 0;
}

int gpio_get_value(unsigned int gpio)
{
	unsigned int value;
	struct gpio_bank *bank = gpio_get_bank(gpio);

	value = read32(&bank->dat);
	return !!(value & DAT_MASK(GPIO_BIT(gpio)));
}

int gpio_set_value(unsigned int gpio, int value)
{
	unsigned int val;
	struct gpio_bank *bank = gpio_get_bank(gpio);

	val = read32(&bank->dat);
	val &= ~DAT_MASK(GPIO_BIT(gpio));
	if (value)
		val |= DAT_SET(GPIO_BIT(gpio));
	write32(&bank->dat, val);

	return 0;
}

/*
 * Add a delay here to give the lines time to settle
 * TODO(dianders): 5us does not always work, 10 is stable, so use 15 to be safe
 * Come back to this and sort out what the datasheet says
 */
#define GPIO_DELAY_US	15

int gpio_read_mvl3(unsigned int gpio)
{
	int high, low;
	enum mvl3 value;

	if (gpio >= GPIO_MAX_PORT)
		return -1;

	gpio_direction_input(gpio);
	gpio_set_pull(gpio, GPIO_PULL_UP);
	udelay(GPIO_DELAY_US);
	high = gpio_get_value(gpio);
	gpio_set_pull(gpio, GPIO_PULL_DOWN);
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
		gpio_set_pull(gpio, GPIO_PULL_UP);

	return value;
}

/*
 * Display Exynos GPIO information
 */
void gpio_info(void)
{
	unsigned int gpio;

	for (gpio = 0; gpio < GPIO_MAX_PORT; gpio++) {
		int cfg = gpio_get_cfg(gpio);

		printk(BIOS_INFO, "GPIO_%-3d: ", gpio);
		if (cfg == GPIO_INPUT)
			printk(BIOS_INFO, "input");
		else if (cfg == GPIO_OUTPUT)
			printk(BIOS_INFO, "output");
		else
			printk(BIOS_INFO, "func %d", cfg);

		if (cfg == GPIO_INPUT || cfg == GPIO_OUTPUT)
			printk(BIOS_INFO, ", value = %d", gpio_get_value(gpio));
		printk(BIOS_INFO, "\n");
	}
}
