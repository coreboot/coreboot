/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef INTEL_LYNXPOINT_LP_GPIO_H
#define INTEL_LYNXPOINT_LP_GPIO_H

/* LynxPoint LP GPIOBASE Registers */
#define GPIO_OWNER(set)		(0x00 + ((set) * 4))
#define GPIO_PIRQ_APIC_EN	0x10
#define GPIO_BLINK		0x18
#define GPIO_SER_BLINK		0x1c
#define GPIO_SER_BLINK_CS	0x20
#define GPIO_SER_BLINK_DATA	0x24
#define GPIO_ROUTE(set)		(0x30 + ((set) * 4))
#define GPIO_RESET(set)		(0x60 + ((set) * 4))
#define GPIO_GLOBAL_CONFIG	0x7c
#define GPIO_IRQ_IS(set)	(0x80 + ((set) * 4))
#define GPIO_IRQ_IE(set)	(0x90 + ((set) * 4))
#define GPIO_CONFIG0(gpio)	(0x100 + ((gpio) * 8))
#define GPIO_CONFIG1(gpio)	(0x104 + ((gpio) * 8))

#define MAX_GPIO_NUMBER		94 /* zero based */
#define GPIO_LIST_END		0xffffffff

/* conf0 */

#define GPIO_MODE_NATIVE	(0 << 0)
#define GPIO_MODE_GPIO		(1 << 0)

#define GPIO_DIR_OUTPUT		(0 << 2)
#define GPIO_DIR_INPUT		(1 << 2)

#define GPIO_NO_INVERT		(0 << 3)
#define GPIO_INVERT		(1 << 3)

#define GPIO_IRQ_EDGE		(0 << 4)
#define GPIO_IRQ_LEVEL		(1 << 4)

#define GPI_LEVEL		(1 << 30)

#define GPO_LEVEL_SHIFT		31
#define GPO_LEVEL_MASK		(1 << GPO_LEVEL_SHIFT)
#define GPO_LEVEL_LOW		(0 << GPO_LEVEL_SHIFT)
#define GPO_LEVEL_HIGH		(1 << GPO_LEVEL_SHIFT)

/* conf1 */

#define GPIO_PULL_NONE		(0 << 0)
#define GPIO_PULL_DOWN		(1 << 0)
#define GPIO_PULL_UP		(2 << 0)

#define GPIO_SENSE_ENABLE	(0 << 2)
#define GPIO_SENSE_DISABLE	(1 << 2)

/* owner */

#define GPIO_OWNER_ACPI		0
#define GPIO_OWNER_GPIO		1

/* route */

#define GPIO_ROUTE_SCI		0
#define GPIO_ROUTE_SMI		1

/* irqen */

#define GPIO_IRQ_DISABLE	0
#define GPIO_IRQ_ENABLE		1

/* blink */

#define GPO_NO_BLINK		0
#define GPO_BLINK		1

/* reset */

#define GPIO_RESET_PWROK	0
#define GPIO_RESET_RSMRST	1

/* pirq route to io-apic */

#define GPIO_PIRQ_APIC_MASK	0
#define GPIO_PIRQ_APIC_ROUTE	1

#define LP_GPIO_END \
	{ .conf0 = GPIO_LIST_END }

#define LP_GPIO_NATIVE \
	{ .conf0 = GPIO_MODE_NATIVE }

#define LP_GPIO_UNUSED \
	{ .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT, \
	  .owner = GPIO_OWNER_GPIO, \
	  .conf1 = GPIO_SENSE_DISABLE }

#define LP_GPIO_ACPI_SCI \
	{ .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_INVERT, \
	  .owner = GPIO_OWNER_ACPI, \
	  .route = GPIO_ROUTE_SCI }

#define LP_GPIO_ACPI_SMI \
	{ .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_INVERT, \
	  .owner = GPIO_OWNER_ACPI, \
	  .route = GPIO_ROUTE_SMI }

#define LP_GPIO_INPUT \
	{ .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT, \
	  .owner = GPIO_OWNER_GPIO }

#define LP_GPIO_INPUT_INVERT \
	{ .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_INVERT, \
	  .owner = GPIO_OWNER_GPIO }

#define LP_GPIO_IRQ_EDGE \
	{ .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_IRQ_EDGE, \
	  .owner = GPIO_OWNER_GPIO, \
	  .irqen = GPIO_IRQ_ENABLE }

#define LP_GPIO_IRQ_LEVEL \
	{ .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_IRQ_LEVEL, \
	  .owner = GPIO_OWNER_GPIO, \
	  .irqen = GPIO_IRQ_ENABLE }

#define LP_GPIO_PIRQ \
	{ .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT, \
	  .owner = GPIO_OWNER_GPIO, \
	  .pirq  = GPIO_PIRQ_APIC_ROUTE }

#define LP_GPIO_OUT_HIGH \
	{ .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_HIGH, \
	  .owner = GPIO_OWNER_GPIO, \
	  .conf1 = GPIO_SENSE_DISABLE }

#define LP_GPIO_OUT_LOW \
	{ .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_LOW, \
	  .owner = GPIO_OWNER_GPIO, \
	  .conf1 = GPIO_SENSE_DISABLE }

struct pch_lp_gpio_map {
	u8 gpio;
	u32 conf0;
	u32 conf1;
	u8 owner;
	u8 route;
	u8 irqen;
	u8 reset;
	u8 blink;
	u8 pirq;
} __attribute__ ((packed));

/* Configure GPIOs with mainboard provided settings */
void setup_pch_lp_gpios(const struct pch_lp_gpio_map map[]);

#endif
