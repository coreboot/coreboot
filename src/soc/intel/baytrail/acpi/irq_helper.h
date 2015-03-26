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
 * Foundation, Inc.
 */

#undef PCI_DEV_PIRQ_ROUTES
#undef ACPI_DEV_APIC_IRQ
#undef PCI_DEV_PIRQ_ROUTE
#undef PIRQ_PIC_ROUTES
#undef PIRQ_PIC

#if defined(PIC_MODE)

#define ACPI_DEV_APIC_IRQ(dev_, pin_, pin_name_) \
	Package() { ## dev_ ## ffff, pin_, \_SB.PCI0.LPCB.LNK ## pin_name_, 0 }

#else /* defined(PIC_MODE) */

#define ACPI_DEV_APIC_IRQ(dev_, pin_, pin_name_) \
	Package() { ## dev_ ## ffff, pin_, 0, PIRQ ## pin_name_ ## _APIC_IRQ }

#endif

#define PCI_DEV_PIRQ_ROUTE(dev_, a_, b_, c_, d_) \
	ACPI_DEV_APIC_IRQ(dev_, 0, a_), \
	ACPI_DEV_APIC_IRQ(dev_, 1, b_), \
	ACPI_DEV_APIC_IRQ(dev_, 2, c_), \
	ACPI_DEV_APIC_IRQ(dev_, 3, d_)

/* Empty PIRQ_PIC definition. */
#define PIRQ_PIC(pirq_, pic_irq_)

/* Include the mainboard irq route definition. */
#include "irqroute.h"
