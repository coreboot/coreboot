/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <arch/io.h>
#include <device/pci_ops.h>
#include <device/device.h>
#include <device/pci.h>
#include <gpio.h>

#include "pch.h"
#include "lp_gpio.h"

static u16 get_gpio_base(void)
{
#ifdef __SIMPLE_DEVICE__
	return pci_read_config16(PCH_LPC_DEV, GPIO_BASE) & 0xfffc;
#else
	return pci_read_config16(pcidev_on_root(0x1f, 0),
				 GPIO_BASE) & 0xfffc;
#endif
}

/*
 * This function will return a number that indicates which PIRQ
 * this GPIO maps to.  If this is not a PIRQ capable GPIO then
 * it will return -1.  The GPIO to PIRQ mapping is not linear.
 */
static int lp_gpio_to_pirq(int gpio)
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
	u16 gpio_base = get_gpio_base();
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
		outl(config->conf0, gpio_base + GPIO_CONFIG0(gpio));
		outl(config->conf1, gpio_base + GPIO_CONFIG1(gpio));

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
			set = lp_gpio_to_pirq(gpio);
			if (set >= 0)
				pirq2apic |= 1 << set;
		}
	}

	for (set = 0; set <= 2; set++) {
		outl(owner[set], gpio_base + GPIO_OWNER(set));
		outl(route[set], gpio_base + GPIO_ROUTE(set));
		outl(irqen[set], gpio_base + GPIO_IRQ_IE(set));
		outl(reset[set], gpio_base + GPIO_RESET(set));
	}

	outl(blink, gpio_base + GPIO_BLINK);
	outl(pirq2apic, gpio_base + GPIO_PIRQ_APIC_EN);
}

int gpio_get(gpio_t gpio_num)
{
	u16 gpio_base = get_gpio_base();

	if (gpio_num > MAX_GPIO_NUMBER)
		return 0;

	return !!(inl(gpio_base + GPIO_CONFIG0(gpio_num)) & GPI_LEVEL);
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
		if (gpio_get(gpio))
			vector |= bitmask;
		bitmask <<= 1;
	}
	return vector;
}

void set_gpio(int gpio_num, int value)
{
	u16 gpio_base = get_gpio_base();
	u32 conf0;

	if (gpio_num > MAX_GPIO_NUMBER)
		return;

	conf0 = inl(gpio_base + GPIO_CONFIG0(gpio_num));
	conf0 &= ~GPO_LEVEL_MASK;
	conf0 |= value << GPO_LEVEL_SHIFT;
	outl(conf0, gpio_base + GPIO_CONFIG0(gpio_num));
}

int gpio_is_native(int gpio_num)
{
	u16 gpio_base = get_gpio_base();

	return !(inl(gpio_base + GPIO_CONFIG0(gpio_num)) & 1);
}
