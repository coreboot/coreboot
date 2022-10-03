/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <device/pci_ops.h>
#include <device/pci_type.h>
#include <stdint.h>

#include "gpio.h"

#define MAX_GPIO_NUMBER 75 /* zero based */

/* LPC GPIO Base Address Register */
#define GPIO_BASE	0x48

static u16 get_gpio_base(void)
{
#ifdef __SIMPLE_DEVICE__
	/* Don't assume GPIO_BASE is still the same */
	return pci_read_config16(PCI_DEV(0, 0x1f, 0), GPIO_BASE) & 0xfffe;
#else
	static u16 gpiobase;

	if (gpiobase)
		return gpiobase;

	gpiobase = pci_read_config16(pcidev_on_root(0x1f, 0), GPIO_BASE) & 0xfffe;

	return gpiobase;
#endif
}

void setup_pch_gpios(const struct pch_gpio_map *gpio)
{
	u16 gpiobase = get_gpio_base();

	/* The order of these calls does matter on ICH9M and prior.
	 * The level has to be set on pins configured as gpio,
	 * but on newer platforms we want to change the level first
	 * to make sure there are no glitches on the lines !
	 * Write the gpio level twice to satisfy both requirements.
	 */

	/* GPIO Set 1 */
	if (gpio->set1.level)
		outl(*((u32 *)gpio->set1.level), gpiobase + GP_LVL);
	if (gpio->set1.mode)
		outl(*((u32 *)gpio->set1.mode), gpiobase + GPIO_USE_SEL);
	if (gpio->set1.direction)
		outl(*((u32 *)gpio->set1.direction), gpiobase + GP_IO_SEL);
	if (gpio->set1.level)
		outl(*((u32 *)gpio->set1.level), gpiobase + GP_LVL);
	if (gpio->set1.reset)
		outl(*((u32 *)gpio->set1.reset), gpiobase + GP_RST_SEL1);
	if (gpio->set1.invert)
		outl(*((u32 *)gpio->set1.invert), gpiobase + GPI_INV);
	if (gpio->set1.blink)
		outl(*((u32 *)gpio->set1.blink), gpiobase + GPO_BLINK);

	/* GPIO Set 2 */
	if (gpio->set2.level)
		outl(*((u32 *)gpio->set2.level), gpiobase + GP_LVL2);
	if (gpio->set2.mode)
		outl(*((u32 *)gpio->set2.mode), gpiobase + GPIO_USE_SEL2);
	if (gpio->set2.direction)
		outl(*((u32 *)gpio->set2.direction), gpiobase + GP_IO_SEL2);
	if (gpio->set2.level)
		outl(*((u32 *)gpio->set2.level), gpiobase + GP_LVL2);
	if (gpio->set2.reset)
		outl(*((u32 *)gpio->set2.reset), gpiobase + GP_RST_SEL2);

	/* GPIO Set 3 */
	if (gpio->set3.level)
		outl(*((u32 *)gpio->set3.level), gpiobase + GP_LVL3);
	if (gpio->set3.mode)
		outl(*((u32 *)gpio->set3.mode), gpiobase + GPIO_USE_SEL3);
	if (gpio->set3.direction)
		outl(*((u32 *)gpio->set3.direction), gpiobase + GP_IO_SEL3);
	if (gpio->set3.level)
		outl(*((u32 *)gpio->set3.level), gpiobase + GP_LVL3);
	if (gpio->set3.reset)
		outl(*((u32 *)gpio->set3.reset), gpiobase + GP_RST_SEL3);
}

/*
 * return current gpio level.
 */
int get_gpio(int gpio_num)
{
	static const int gpio_reg_offsets[] = {GP_LVL, GP_LVL2, GP_LVL3};
	u16 gpio_base = get_gpio_base();
	int index, bit;

	if (gpio_num > MAX_GPIO_NUMBER)
		return 0; /* Just ignore wrong gpio numbers. */

	index = gpio_num / 32;
	bit = gpio_num % 32;

	return (inl(gpio_base + gpio_reg_offsets[index]) >> bit) & 1;
}

/*
 * get a number comprised of multiple GPIO values. gpio_num_array points to
 * the array of gpio pin numbers to scan, terminated by -1.
 */
unsigned int get_gpios(const int *gpio_num_array)
{
	int gpio;
	unsigned int bitmask = 1;
	unsigned int vector = 0;

	while (bitmask &&
	       ((gpio = *gpio_num_array++) != -1)) {
		if (get_gpio(gpio))
			vector |= bitmask;
		bitmask <<= 1;
	}
	return vector;
}

/*
 * set gpio output to level.
 */
void set_gpio(int gpio_num, int value)
{
	static const int gpio_reg_offsets[] = {
		GP_LVL, GP_LVL2, GP_LVL3
	};
	u16 gpio_base = get_gpio_base();
	int index, bit;
	u32 config;

	if (gpio_num > MAX_GPIO_NUMBER)
		return; /* Just ignore wrong gpio numbers. */

	index = gpio_num / 32;
	bit = gpio_num % 32;

	config = inl(gpio_base + gpio_reg_offsets[index]);
	config &= ~(1 << bit);
	if (value != 0)
		config |= (1 << bit);
	outl(config, gpio_base + gpio_reg_offsets[index]);
}

int gpio_is_native(int gpio_num)
{
	static const int gpio_reg_offsets[] = {
		GPIO_USE_SEL, GPIO_USE_SEL2, GPIO_USE_SEL3
	};
	u16 gpio_base = get_gpio_base();
	int index, bit;
	u32 config;

	if (gpio_num > MAX_GPIO_NUMBER)
		return 0; /* Just ignore wrong gpio numbers. */

	index = gpio_num / 32;
	bit = gpio_num % 32;

	config = inl(gpio_base + gpio_reg_offsets[index]);
	return !(config & (1 << bit));
}
