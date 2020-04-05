/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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
