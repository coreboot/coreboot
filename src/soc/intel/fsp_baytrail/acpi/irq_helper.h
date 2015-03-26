/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2014 Sage Electronics Engineering, LLC.
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


/*
 * This file intentionally gets included multiple times, to set pic and apic
 * modes, so should not have guard statements added.
 */

/*
 * This file will use arch/x86/acpi/irqroute.asl and mainboard/irqroute.h
 * to generate the ACPI IRQ routing for the mainboard being compiled.
 * This method uses #defines in irqroute.h along with the macros contained
 * in this file to generate an IRQ routing for each PCI device in the system.
 */

#undef PCI_DEV_PIRQ_ROUTES
#undef ACPI_DEV_IRQ
#undef PCI_DEV_PIRQ_ROUTE
#undef PIRQ_PIC_ROUTES
#undef PIRQ_PIC
#undef IRQROUTE_H
#undef ROOTPORT_METHODS
#undef RP_METHOD
#undef ROOTPORT_IRQ_ROUTES
#undef RP_IRQ_ROUTES

#if defined(PIC_MODE)

#define ACPI_DEV_IRQ(dev_, pin_, pin_name_) \
	Package() { ## dev_ ## ffff, pin_, \_SB.PCI0.LPCB.LNK ## pin_name_, 0 }

#define RP_IRQ_ROUTES(prefix_, func_, a_, b_, c_, d_) \
Name(prefix_ ## func_ ## P, Package() \
{ \
	ACPI_DEV_IRQ(0x0000, 0, a_), \
	ACPI_DEV_IRQ(0x0000, 1, b_), \
	ACPI_DEV_IRQ(0x0000, 2, c_), \
	ACPI_DEV_IRQ(0x0000, 3, d_), \
})

/* define as blank so ROOTPORT_METHODS only gets inserted once */
#define ROOTPORT_METHODS(prefix_, dev_)

#else /* defined(PIC_MODE) */

#define ACPI_DEV_IRQ(dev_, pin_, pin_name_) \
	Package() { ## dev_ ## ffff, pin_, 0, PIRQ ## pin_name_ ## _APIC_IRQ }

#define RP_IRQ_ROUTES(prefix_, func_, a_, b_, c_, d_) \
Name(prefix_ ## func_ ## A, Package() \
{ \
	ACPI_DEV_IRQ(0x0000, 0, a_), \
	ACPI_DEV_IRQ(0x0000, 1, b_), \
	ACPI_DEV_IRQ(0x0000, 2, c_), \
	ACPI_DEV_IRQ(0x0000, 3, d_), \
})

#define ROOTPORT_METHODS(prefix_, dev_) \
	RP_METHOD(prefix_, dev_, 0) \
	RP_METHOD(prefix_, dev_, 1) \
	RP_METHOD(prefix_, dev_, 2) \
	RP_METHOD(prefix_, dev_, 3) \
	RP_METHOD(prefix_, dev_, 4) \
	RP_METHOD(prefix_, dev_, 5) \
	RP_METHOD(prefix_, dev_, 6) \
	RP_METHOD(prefix_, dev_, 7)

#endif /* defined(PIC_MODE) */

#define PCI_DEV_PIRQ_ROUTE(dev_, a_, b_, c_, d_) \
	ACPI_DEV_IRQ(dev_, 0, a_), \
	ACPI_DEV_IRQ(dev_, 1, b_), \
	ACPI_DEV_IRQ(dev_, 2, c_), \
	ACPI_DEV_IRQ(dev_, 3, d_), \

#define PCIE_BRIDGE_DEV(prefix_, dev_, a_, b_, c_, d_) \
	ROOTPORT_IRQ_ROUTES(prefix_, a_, b_, c_, d_) \
	ROOTPORT_METHODS(prefix_, dev_)

#define ROOTPORT_IRQ_ROUTES(prefix_, a_, b_, c_, d_) \
	RP_IRQ_ROUTES(prefix_, 0, a_, b_, c_, d_) \
	RP_IRQ_ROUTES(prefix_, 1, b_, c_, d_, a_) \
	RP_IRQ_ROUTES(prefix_, 2, c_, d_, a_, b_) \
	RP_IRQ_ROUTES(prefix_, 3, d_, a_, b_, c_) \
	RP_IRQ_ROUTES(prefix_, 4, a_, b_, c_, d_) \
	RP_IRQ_ROUTES(prefix_, 5, b_, c_, d_, a_) \
	RP_IRQ_ROUTES(prefix_, 6, c_, d_, a_, b_) \
	RP_IRQ_ROUTES(prefix_, 7, d_, a_, b_, c_)

#define RP_METHOD(prefix_, dev_, func_)\
Device(prefix_ ## 0 ## func_) \
{ \
	Name(_ADR, dev_ ## 000 ## func_) \
	Name(_PRW, Package() { \
		0, 0 \
	}) \
	Method(_PRT,0) { \
		If(PICM) { \
			Return (prefix_ ## func_ ## A) \
		 } Else { \
			Return (prefix_ ## func_ ## P) \
		 } \
	} \
}

/* Empty PIRQ_PIC definition. */
#define PIRQ_PIC(pirq_, pic_irq_)

/* Include the mainboard irq route definition */
#include "irqroute.h"
