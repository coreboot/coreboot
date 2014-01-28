/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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

#ifndef _BAYTRAIL_IRQ_H_
#define _BAYTRAIL_IRQ_H_

#define PIRQA_APIC_IRQ			16
#define PIRQB_APIC_IRQ			17
#define PIRQC_APIC_IRQ			18
#define PIRQD_APIC_IRQ			19
#define PIRQE_APIC_IRQ			20
#define PIRQF_APIC_IRQ			21
#define PIRQG_APIC_IRQ			22
#define PIRQH_APIC_IRQ			23

#define PIRQ_PIC_IRQDISABLE		0x0
#define PIRQ_PIC_IRQ3			0x3
#define PIRQ_PIC_IRQ4			0x4
#define PIRQ_PIC_IRQ5			0x5
#define PIRQ_PIC_IRQ6			0x6
#define PIRQ_PIC_IRQ7			0x7
#define PIRQ_PIC_IRQ9			0x9
#define PIRQ_PIC_IRQ10			0xa
#define PIRQ_PIC_IRQ11			0xb
#define PIRQ_PIC_IRQ12			0xc
#define PIRQ_PIC_IRQ14			0xe
#define PIRQ_PIC_IRQ15			0xf

/* Overloaded term, but these values determine the per device route. */
#define PIRQA				0
#define PIRQB				1
#define PIRQC				2
#define PIRQD				3
#define PIRQE				4
#define PIRQF				5
#define PIRQG				6
#define PIRQH				7

/* In each mainbaord directory there should exist a header file irqroute.h that
 * defines the PCI_DEV_PIRQ_ROUTES and PIRQ_PIC_ROUTES macros which
 * consist of PCI_DEV_PIRQ_ROUTE and PIRQ_PIC entries. */

#if !defined(__ASSEMBLER__) && !defined(__ACPI__)
#include <stdint.h>

#define NUM_IR_DEVS 32
#define NUM_PIRQS   8

struct baytrail_irq_route {
	/* Per device configuration. */
	uint16_t pcidev[NUM_IR_DEVS];
	/* Route path for each internal PIRQx in PIC mode. */
	uint8_t  pic[NUM_PIRQS];
};

extern const struct baytrail_irq_route global_baytrail_irq_route;

#define DEFINE_IRQ_ROUTES \
	const struct baytrail_irq_route global_baytrail_irq_route = { \
		.pcidev = { PCI_DEV_PIRQ_ROUTES, }, \
		.pic = { PIRQ_PIC_ROUTES, }, \
	}

#define PCI_DEV_PIRQ_ROUTE(dev_, a_, b_, c_, d_) \
	[dev_] = ((PIRQ ## d_) << 12) | ((PIRQ ## c_) << 8) | \
	         ((PIRQ ## b_) <<  4) | ((PIRQ ## a_) << 0)

#define PIRQ_PIC(pirq_, pic_irq_) \
	[PIRQ ## pirq_] = PIRQ_PIC_IRQ ## pic_irq_

#endif /* !defined(__ASSEMBLER__) && !defined(__ACPI__) */

#endif /* _BAYTRAIL_IRQ_H_ */
