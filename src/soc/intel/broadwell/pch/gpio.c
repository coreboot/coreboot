/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <soc/gpio.h>
#include <soc/iomap.h>
#include <soc/pm.h>

/*
 * This function will return a number that indicates which PIRQ
 * this GPIO maps to.  If this is not a PIRQ capable GPIO then
 * it will return -1.  The GPIO to PIRQ mapping is not linear.
 */
static int gpio_to_pirq(int gpio)
{
	switch (gpio) {
	case 8:  return 0;	/* PIRQI */
	case 9:  return 1;	/* PIRQJ */
	case 10: return 2;	/* PIRQK */
	case 13: return 3;	/* PIRQL */
	case 14: return 4;	/* PIRQM */
	case 45: return 5;	/* PIRQN */
	case 46: return 6;	/* PIRQO */
	case 47: return 7;	/* PIRQP */
	case 48: return 8;	/* PIRQQ */
	case 49: return 9;	/* PIRQR */
	case 50: return 10;	/* PIRQS */
	case 51: return 11;	/* PIRQT */
	case 52: return 12;	/* PIRQU */
	case 53: return 13;	/* PIRQV */
	case 54: return 14;	/* PIRQW */
	case 55: return 15;	/* PIRQX */
	default: return -1;
	};
}

void setup_pch_lp_gpios(const struct pch_lp_gpio_map map[])
{
	const struct pch_lp_gpio_map *config;
	u32 owner[3] = {0};
	u32 route[3] = {0};
	u32 irqen[3] = {0};
	u32 reset[3] = {0};
	u32 blink = 0;
	u16 pirq2apic = 0;
	int set, bit, gpio = 0;

	for (config = map; config->conf0 != GPIO_LIST_END; config++, gpio++) {
		if (gpio > MAX_GPIO_NUMBER)
			break;

		/* Setup Configuration registers 1 and 2 */
		outl(config->conf0, GPIO_BASE_ADDRESS + GPIO_CONFIG0(gpio));
		outl(config->conf1, GPIO_BASE_ADDRESS + GPIO_CONFIG1(gpio));

		/* Determine set and bit based on GPIO number */
		set = gpio >> 5;
		bit = gpio % 32;

		/* Apply settings to set specific bits */
		owner[set] |= config->owner << bit;
		route[set] |= config->route << bit;
		irqen[set] |= config->irqen << bit;
		reset[set] |= config->reset << bit;

		if (set == 0)
			blink |= config->blink << bit;

		/* PIRQ to IO-APIC map */
		if (config->pirq == GPIO_PIRQ_APIC_ROUTE) {
			set = gpio_to_pirq(gpio);
			if (set >= 0)
				pirq2apic |= 1 << set;
		}
	}

	for (set = 0; set <= 2; set++) {
		outl(owner[set], GPIO_BASE_ADDRESS + GPIO_OWNER(set));
		outl(route[set], GPIO_BASE_ADDRESS + GPIO_ROUTE(set));
		outl(irqen[set], GPIO_BASE_ADDRESS + GPIO_IRQ_IE(set));
		outl(reset[set], GPIO_BASE_ADDRESS + GPIO_RESET(set));
	}

	outl(blink, GPIO_BASE_ADDRESS + GPIO_BLINK);
	outl(pirq2apic, GPIO_BASE_ADDRESS + GPIO_PIRQ_APIC_EN);
}

int get_gpio(int gpio_num)
{
	if (gpio_num > MAX_GPIO_NUMBER)
		return 0;

	return !!(inl(GPIO_BASE_ADDRESS + GPIO_CONFIG0(gpio_num)) & GPI_LEVEL);
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

void set_gpio(int gpio_num, int value)
{
	u32 conf0;

	if (gpio_num > MAX_GPIO_NUMBER)
		return;

	conf0 = inl(GPIO_BASE_ADDRESS + GPIO_CONFIG0(gpio_num));
	conf0 &= ~GPO_LEVEL_MASK;
	conf0 |= value << GPO_LEVEL_SHIFT;
	outl(conf0, GPIO_BASE_ADDRESS + GPIO_CONFIG0(gpio_num));
}

int gpio_is_native(int gpio_num)
{
	return !(inl(GPIO_BASE_ADDRESS + GPIO_CONFIG0(gpio_num)) & 1);
}
