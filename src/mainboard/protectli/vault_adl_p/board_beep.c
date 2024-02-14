/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <delay.h>

#include "board_beep.h"

#define ITE_GPIO_BASE		0xa00
#define ITE_GPIO_PIN(x)		(1 << ((x) % 10))
#define ITE_GPIO_SET(x)		(((x) / 10) - 1)
#define ITE_GPIO_IO_ADDR(x)	(ITE_GPIO_BASE + ITE_GPIO_SET(x))

void do_beep(uint32_t frequency, uint32_t duration_msec)
{
	uint32_t timer_delay = 1000000 / frequency / 2;
	uint32_t count = (duration_msec * 1000) / (timer_delay * 2);
	uint8_t val = inb(ITE_GPIO_IO_ADDR(41)); /* GP41 drives a MOSFET for PC Speaker */

	for (uint32_t i = 0; i < count; i++) {
		outb(val | ITE_GPIO_PIN(41), ITE_GPIO_IO_ADDR(41));
		udelay(timer_delay);
		outb(val & ~ITE_GPIO_PIN(41), ITE_GPIO_IO_ADDR(41));
		udelay(timer_delay);
	}
}
