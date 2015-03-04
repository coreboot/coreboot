/*
* Copyright (C) 2015 Broadcom Corporation
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation version 2.
*
* This program is distributed "as is" WITHOUT ANY WARRANTY of any
* kind, whether express or implied; without even the implied warranty
* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#include <arch/io.h>
#include <console/console.h>
#include <gpio.h>
#include <stdlib.h>
#include <delay.h>

#define dev_dbg(chip, fmt, args...) printk(BIOS_DEBUG, "[%s] " fmt, \
	chip->label, args)

#define CYGNUS_GPIO_DATA_IN_OFFSET   0x00
#define CYGNUS_GPIO_DATA_OUT_OFFSET  0x04
#define CYGNUS_GPIO_OUT_EN_OFFSET    0x08
#define CYGNUS_GPIO_IN_TYPE_OFFSET   0x0c
#define CYGNUS_GPIO_INT_DE_OFFSET    0x10
#define CYGNUS_GPIO_INT_EDGE_OFFSET  0x14
#define CYGNUS_GPIO_INT_MSK_OFFSET   0x18
#define CYGNUS_GPIO_INT_STAT_OFFSET  0x1c
#define CYGNUS_GPIO_INT_MSTAT_OFFSET 0x20
#define CYGNUS_GPIO_INT_CLR_OFFSET   0x24
#define CYGNUS_GPIO_PAD_RES_OFFSET   0x34
#define CYGNUS_GPIO_RES_EN_OFFSET    0x38

/* drive strength control for ASIU GPIO */
#define CYGNUS_GPIO_ASIU_DRV0_CTRL_OFFSET 0x58

/* drive strength control for CCM/CRMU (AON) GPIO */
#define CYGNUS_GPIO_DRV0_CTRL_OFFSET  0x00

#define GPIO_BANK_SIZE 0x200
#define NGPIOS_PER_BANK 32
#define GPIO_BANK(pin) ((pin) / NGPIOS_PER_BANK)

#define CYGNUS_GPIO_REG(pin, reg) (GPIO_BANK(pin) * GPIO_BANK_SIZE + (reg))
#define CYGNUS_GPIO_SHIFT(pin) ((pin) % NGPIOS_PER_BANK)

#define GPIO_DRV_STRENGTH_BIT_SHIFT  20
#define GPIO_DRV_STRENGTH_BITS       3
#define GPIO_DRV_STRENGTH_BIT_MASK   ((1 << GPIO_DRV_STRENGTH_BITS) - 1)

/*
 * Cygnus GPIO core
 *
 * @base: I/O register base for Cygnus GPIO controller
 * @io_ctrl: I/O register base for certain type of Cygnus GPIO controller that
 * has the PINCONF support implemented outside of the GPIO block
 * @num_banks: number of GPIO banks, each bank supports up to 32 GPIOs
 * @pinmux_is_supported: flag to indicate this GPIO controller contains pins
 * that can be individually muxed to GPIO
 * @pctl_priv: pointer to pinctrl handle
 */
struct cygnus_gpio {
	void		*base;
	void		*io_ctrl;
	const char	*label;
	int			gpio_base;
	u16			ngpio;
	unsigned	num_banks;
	int			pinmux_is_supported;
	void		*pctl_priv;
};

/*
 * GPIO cores table
 *
 * Cygnus has 3 gpio cores. The tables contains descriptors of those cores.
 */
struct cygnus_gpio cygnus_gpio_table[] = {
	{
		.base = (void *)0x03024800,
		.io_ctrl = (void *)0x03024008,
		.label = "gpio_crmu",
		.gpio_base = 170,
		.ngpio = 6,
	},
	{
		.base = (void *)0x1800a000,
		.io_ctrl = (void *)0x0301d164,
		.label = "gpio_ccm",
		.gpio_base = 0,
		.ngpio = 24,
	},
	{
		.base = (void *)0x180a5000,
		.label = "gpio_asiu",
		.gpio_base = 24,
		.ngpio = 146,
		.pinmux_is_supported = 1
	}
};

/*
 * Map a GPIO in the local gpio_chip pin space to a pin in the Cygnus IOMUX
 * pinctrl pin space
 */
struct cygnus_gpio_pin_range {
	unsigned offset;
	unsigned pin_base;
	unsigned num_pins;
};

#define CYGNUS_PINRANGE(o, p, n) { .offset = o, .pin_base = p, .num_pins = n }

/*
 * Pin mapping table for mapping local GPIO pins to Cygnus IOMUX pinctrl pins.
 * This is for ASIU gpio. The offset is based on ASIU gpios.
 */
static const struct cygnus_gpio_pin_range cygnus_gpio_pintable[] = {
	CYGNUS_PINRANGE(0, 42, 1),
	CYGNUS_PINRANGE(1, 44, 3),
	CYGNUS_PINRANGE(4, 48, 1),
	CYGNUS_PINRANGE(5, 50, 3),
	CYGNUS_PINRANGE(8, 126, 1),
	CYGNUS_PINRANGE(9, 155, 1),
	CYGNUS_PINRANGE(10, 152, 1),
	CYGNUS_PINRANGE(11, 154, 1),
	CYGNUS_PINRANGE(12, 153, 1),
	CYGNUS_PINRANGE(13, 127, 3),
	CYGNUS_PINRANGE(16, 140, 1),
	CYGNUS_PINRANGE(17, 145, 7),
	CYGNUS_PINRANGE(24, 130, 10),
	CYGNUS_PINRANGE(34, 141, 4),
	CYGNUS_PINRANGE(38, 54, 1),
	CYGNUS_PINRANGE(39, 56, 3),
	CYGNUS_PINRANGE(42, 60, 3),
	CYGNUS_PINRANGE(45, 64, 3),
	CYGNUS_PINRANGE(48, 68, 2),
	CYGNUS_PINRANGE(50, 84, 6),
	CYGNUS_PINRANGE(56, 94, 6),
	CYGNUS_PINRANGE(62, 72, 1),
	CYGNUS_PINRANGE(63, 70, 1),
	CYGNUS_PINRANGE(64, 80, 1),
	CYGNUS_PINRANGE(65, 74, 3),
	CYGNUS_PINRANGE(68, 78, 1),
	CYGNUS_PINRANGE(69, 82, 1),
	CYGNUS_PINRANGE(70, 156, 17),
	CYGNUS_PINRANGE(87, 104, 12),
	CYGNUS_PINRANGE(99, 102, 2),
	CYGNUS_PINRANGE(101, 90, 4),
	CYGNUS_PINRANGE(105, 116, 10),
	CYGNUS_PINRANGE(123, 11, 1),
	CYGNUS_PINRANGE(124, 38, 4),
	CYGNUS_PINRANGE(128, 43, 1),
	CYGNUS_PINRANGE(129, 47, 1),
	CYGNUS_PINRANGE(130, 49, 1),
	CYGNUS_PINRANGE(131, 53, 1),
	CYGNUS_PINRANGE(132, 55, 1),
	CYGNUS_PINRANGE(133, 59, 1),
	CYGNUS_PINRANGE(134, 63, 1),
	CYGNUS_PINRANGE(135, 67, 1),
	CYGNUS_PINRANGE(136, 71, 1),
	CYGNUS_PINRANGE(137, 73, 1),
	CYGNUS_PINRANGE(138, 77, 1),
	CYGNUS_PINRANGE(139, 79, 1),
	CYGNUS_PINRANGE(140, 81, 1),
	CYGNUS_PINRANGE(141, 83, 1),
	CYGNUS_PINRANGE(142, 10, 1)
};

static unsigned cygnus_gpio_to_pin(unsigned gpio)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(cygnus_gpio_pintable); i++) {
		const struct cygnus_gpio_pin_range *range = cygnus_gpio_pintable
			+ i;

		if ((gpio < range->offset) ||
			(gpio >= (range->offset + range->num_pins)))
			continue;

		return range->pin_base + (gpio - range->offset);
	}
	return -1;
}

static struct cygnus_gpio *cygnus_get_gpio_core(unsigned gpio,
	unsigned *gpio_offset)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(cygnus_gpio_table); i++) {
		struct cygnus_gpio *chip = cygnus_gpio_table + i;

		if ((gpio < chip->gpio_base) ||
			(gpio >= (chip->gpio_base + chip->ngpio)))
			continue;

		*gpio_offset = gpio - chip->gpio_base;
		return chip;
	}

	return NULL;
}

static u32 cygnus_readl(struct cygnus_gpio *chip, unsigned int offset)
{
	return read32(chip->base + offset);
}

static void cygnus_writel(struct cygnus_gpio *chip, unsigned int offset,
			  u32 val)
{
	write32(chip->base + offset, val);
}

/**
 *  cygnus_set_bit - set or clear one bit (corresponding to the GPIO pin) in a
 *  Cygnus GPIO register
 *
 *  @cygnus_gpio: Cygnus GPIO device
 *  @reg: register offset
 *  @gpio: GPIO pin
 *  @set: set or clear. 1 - set; 0 -clear
 */
static void cygnus_set_bit(struct cygnus_gpio *chip, unsigned int reg,
			   unsigned gpio, int set)
{
	unsigned int offset = CYGNUS_GPIO_REG(gpio, reg);
	unsigned int shift = CYGNUS_GPIO_SHIFT(gpio);
	u32 val;

	val = cygnus_readl(chip, offset);
	if (set)
		val |= BIT(shift);
	else
		val &= ~BIT(shift);
	cygnus_writel(chip, offset, val);
}

static int cygnus_get_bit(struct cygnus_gpio *chip, unsigned int reg,
			  unsigned gpio)
{
	unsigned int offset = CYGNUS_GPIO_REG(gpio, reg);
	unsigned int shift = CYGNUS_GPIO_SHIFT(gpio);
	u32 val;

	val = cygnus_readl(chip, offset) & BIT(shift);
	if (val)
		return 1;
	else
		return 0;
}

/*
 * Request the Cygnus IOMUX pinmux controller to mux individual pins to GPIO
 */
static int cygnus_gpio_request(struct cygnus_gpio *chip, unsigned offset)
{
	/* not all Cygnus GPIO pins can be muxed individually */
	if (!chip->pinmux_is_supported || (chip->pctl_priv == NULL))
		return 0;

	return cygnus_gpio_request_enable(chip->pctl_priv,
		cygnus_gpio_to_pin(offset));
}

static void cygnus_gpio_free(struct cygnus_gpio *chip, unsigned offset)
{
	if (!chip->pinmux_is_supported || (chip->pctl_priv == NULL))
		return;

	cygnus_gpio_disable_free(chip->pctl_priv, cygnus_gpio_to_pin(offset));
}

static int cygnus_gpio_direction_input(struct cygnus_gpio *chip, unsigned gpio)
{
	cygnus_set_bit(chip, CYGNUS_GPIO_OUT_EN_OFFSET, gpio, 0);

	dev_dbg(chip, "gpio:%u set input\n", gpio);

	return 0;
}

static int cygnus_gpio_direction_output(struct cygnus_gpio *chip, unsigned gpio,
					int value)
{
	cygnus_set_bit(chip, CYGNUS_GPIO_OUT_EN_OFFSET, gpio, 1);
	cygnus_set_bit(chip, CYGNUS_GPIO_DATA_OUT_OFFSET, gpio, value);

	dev_dbg(chip, "gpio:%u set output, value:%d\n", gpio, value);

	return 0;
}

static void cygnus_gpio_set(struct cygnus_gpio *chip, unsigned gpio, int value)
{
	cygnus_set_bit(chip, CYGNUS_GPIO_DATA_OUT_OFFSET, gpio, value);

	dev_dbg(chip, "gpio:%u set, value:%d\n", gpio, value);
}

static int cygnus_gpio_get(struct cygnus_gpio *chip, unsigned gpio)
{
	return cygnus_get_bit(chip, CYGNUS_GPIO_DATA_IN_OFFSET, gpio);
}

static int cygnus_gpio_set_pull(struct cygnus_gpio *chip, unsigned gpio,
				int disable, int pull_up)
{
	if (disable) {
		cygnus_set_bit(chip, CYGNUS_GPIO_RES_EN_OFFSET, gpio, 0);
	} else {
		cygnus_set_bit(chip, CYGNUS_GPIO_PAD_RES_OFFSET, gpio, pull_up);
		cygnus_set_bit(chip, CYGNUS_GPIO_RES_EN_OFFSET, gpio, 1);
	}

	dev_dbg(chip, "gpio:%u set pullup:%d\n", gpio, pull_up);

	return 0;
}

#define CYGNUS_GPIO_TEST_AON_GPIO0	170
#define CYGNUS_GPIO_TEST_SPI2_MISO	69
#define CYGNUS_GPIO_TEST_DELAY_S	3

static void cygnus_gpio_test(void)
{
	gpio_t gpio_in;
	gpio_t gpio_out;
	unsigned val;

	printk(BIOS_INFO, "Start gpio test...\n");

	gpio_in = CYGNUS_GPIO_TEST_AON_GPIO0;  /* AON_GPIO0 */
	gpio_input(gpio_in);
	gpio_input_pulldown(gpio_in);
	printk(BIOS_INFO, "GPIO get %d=%d\n", gpio_in, gpio_get(gpio_in));

	gpio_in = CYGNUS_GPIO_TEST_SPI2_MISO;  /* SPI2_MISO */
	gpio_input(gpio_in);
	gpio_input_pullup(gpio_in);
	printk(BIOS_INFO, "GPIO get %d=%d\n", gpio_in, gpio_get(gpio_in));
	val = 0;
	gpio_out = CYGNUS_GPIO_TEST_SPI2_MISO;

	gpio_output(gpio_out, val);
	printk(BIOS_INFO, "GPIO set %d=%d\n", gpio_out, val);
	delay(CYGNUS_GPIO_TEST_DELAY_S);

	val = 1;
	gpio_set(gpio_out, val);
	printk(BIOS_INFO, "GPIO set %d=%d\n", gpio_out, val);
	delay(CYGNUS_GPIO_TEST_DELAY_S);

	val = 0;
	gpio_set(gpio_out, val);
	printk(BIOS_INFO, "GPIO set %d=%d\n", gpio_out, val);
	delay(CYGNUS_GPIO_TEST_DELAY_S);

	val = 1;
	gpio_set(gpio_out, val);
	printk(BIOS_INFO, "GPIO set %d=%d\n", gpio_out, val);
	delay(CYGNUS_GPIO_TEST_DELAY_S);

	val = 0;
	gpio_set(gpio_out, val);
	printk(BIOS_INFO, "GPIO set %d=%d\n", gpio_out, val);
	delay(CYGNUS_GPIO_TEST_DELAY_S);

	gpio_free(CYGNUS_GPIO_TEST_AON_GPIO0);
	gpio_free(CYGNUS_GPIO_TEST_SPI2_MISO);
	printk(BIOS_INFO, "Gpio test completed...\n");
}

void gpio_init(void)
{
	int i;

	printk(BIOS_INFO, "Setting up the gpio...\n");

	for (i = 0; i < ARRAY_SIZE(cygnus_gpio_table); i++) {
		struct cygnus_gpio *chip = cygnus_gpio_table + i;

		chip->num_banks = (chip->ngpio+NGPIOS_PER_BANK - 1)
			/ NGPIOS_PER_BANK;
		if (chip->pinmux_is_supported)
			chip->pctl_priv = cygnus_pinmux_init();
	}

	if (IS_ENABLED(CONFIG_CYGNUS_GPIO_TEST))
		cygnus_gpio_test();
}

void gpio_free(gpio_t gpio)
{
	struct cygnus_gpio *chip;
	unsigned gpio_num;

	chip = cygnus_get_gpio_core(gpio, &gpio_num);
	if (chip == NULL) {
		dev_dbg(chip, "unable to find chip for gpio %d", gpio);
		return;
	}

	cygnus_gpio_free(chip, gpio_num);
}

void gpio_input(gpio_t gpio)
{
	struct cygnus_gpio *chip;
	unsigned gpio_num;

	chip = cygnus_get_gpio_core(gpio, &gpio_num);
	if (chip == NULL) {
		dev_dbg(chip, "unable to find chip for gpio %d", gpio);
		return;
	}

	if (cygnus_gpio_request(chip, gpio_num) != 0) {
		printk(BIOS_ERR, "Cannot mux GPIO %d\n", gpio);
		return;
	}
	cygnus_gpio_direction_input(chip, gpio_num);
}

void gpio_input_pulldown(gpio_t gpio)
{
	struct cygnus_gpio *chip;
	unsigned gpio_num;

	chip = cygnus_get_gpio_core(gpio, &gpio_num);
	if (chip == NULL) {
		dev_dbg(chip, "unable to find chip for gpio %d", gpio);
		return;
	}

	cygnus_gpio_set_pull(chip, gpio_num, 0, 0);
}

void gpio_input_pullup(gpio_t gpio)
{
	struct cygnus_gpio *chip;
	unsigned gpio_num;

	chip = cygnus_get_gpio_core(gpio, &gpio_num);
	if (chip == NULL) {
		dev_dbg(chip, "unable to find chip for gpio %d", gpio);
		return;
	}

	cygnus_gpio_set_pull(chip, gpio_num, 0, 1);
}

int gpio_get(gpio_t gpio)
{
	struct cygnus_gpio *chip;
	unsigned gpio_num;

	chip = cygnus_get_gpio_core(gpio, &gpio_num);
	if (chip == NULL) {
		dev_dbg(chip, "unable to find chip for gpio %d", gpio);
		return -1;
	}

	return cygnus_gpio_get(chip, gpio_num);
}

void gpio_set(gpio_t gpio, int value)
{
	struct cygnus_gpio *chip;
	unsigned gpio_num;

	chip = cygnus_get_gpio_core(gpio, &gpio_num);
	if (chip == NULL) {
		dev_dbg(chip, "unable to find chip for gpio %d", gpio);
		return;
	}

	cygnus_gpio_set(chip, gpio_num, value);
}

void gpio_output(gpio_t gpio, int value)
{
	struct cygnus_gpio *chip;
	unsigned gpio_num;

	chip = cygnus_get_gpio_core(gpio, &gpio_num);
	if (chip == NULL) {
		dev_dbg(chip, "unable to find chip for gpio %d", gpio);
		return;
	}

	if (cygnus_gpio_request(chip, gpio_num) != 0) {
		printk(BIOS_ERR, "Cannot mux GPIO %d\n", gpio);
		return;
	}
	cygnus_gpio_direction_output(chip, gpio_num, value);
}
