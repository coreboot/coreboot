/* SPDX-License-Identifier: GPL-2.0-only */

/* Routing is in System Bus scope */
Name (PR0, Package()
{
	/* NB devices */
	/* Bus 0, Dev 0 - F15 Host Controller */

	/* Bus 0, Dev 1, Func 0 - PCI Bridge for Internal Graphics(IGP) */
	/* Bus 0, Dev 1, Func 1 - HDMI Audio Controller */
	Package() { 0x0001FFFF, 0, INTG, 0 },
	Package() { 0x0001FFFF, 1, INTH, 0 },
	Package() { 0x0001FFFF, 2, INTE, 0 },
	Package() { 0x0001FFFF, 3, INTF, 0 },

	/* Bus 0, Dev 2 Func 0,1,2,3,4,5 - PCIe Bridges */
	Package() { 0x0002FFFF, 0, INTH, 0 },
	Package() { 0x0002FFFF, 1, INTA, 0 },
	Package() { 0x0002FFFF, 2, INTB, 0 },
	Package() { 0x0002FFFF, 3, INTC, 0 },

	/* FCH devices */
	/* Bus 0, Dev 20 - F0:SMBus/ACPI;F3:LPC;F7:SD */
	Package() { 0x0014FFFF, 0, INTA, 0 },
	Package() { 0x0014FFFF, 1, INTB, 0 },
	Package() { 0x0014FFFF, 2, INTC, 0 },
	Package() { 0x0014FFFF, 3, INTD, 0 },

	/* Bus 0, Dev 18 Func 0 - USB: EHCI */
	Package() { 0x0012FFFF, 0, INTC, 0 },

	/* Bus 0, Dev 10 Func 0 - USB: xHCI */
	Package() { 0x0010FFFF, 0, INTC, 0 },

	/* Bus 0, Dev 17 - SATA controller */
	Package() { 0x0011FFFF, 0, INTD, 0 },

})

Name (APR0, Package()
{
	/* NB devices in APIC mode */
	/* Bus 0, Dev 0 - F15 Host Controller */

	/* Bus 0, Dev 1 - PCI Bridge for Internal Graphics(IGP) */
	/* IOAPIC2BASE + (group * 4) == 24 + (1 * 4), CDAB swizzle */
	Package() { 0x0001FFFF, 0, 0, 30 },
	Package() { 0x0001FFFF, 1, 0, 31 },
	Package() { 0x0001FFFF, 1, 0, 28 },
	Package() { 0x0001FFFF, 1, 0, 29 },

	/* Bus 0, Dev 2 - PCIe Bridges  */
	/* IOAPIC2BASE + 23 */
	Package() { 0x0002FFFF, 0, 0, 47 },
	Package() { 0x0002FFFF, 1, 0, 48 },
	Package() { 0x0002FFFF, 2, 0, 49 },
	Package() { 0x0002FFFF, 3, 0, 50 },

	/* SB devices in APIC mode */
	/* Bus 0, Dev 20 - F0:SMBus/ACPI;F3:LPC;F7:SD */
	Package() { 0x0014FFFF, 0, 0, 16 },
	Package() { 0x0014FFFF, 1, 0, 17 },
	Package() { 0x0014FFFF, 2, 0, 18 },
	Package() { 0x0014FFFF, 3, 0, 19 },

	/* Bus 0, Dev 18 Func 0 - USB: EHCI */
	Package() { 0x0012FFFF, 0, 0, 18 },

	/* Bus 0, Dev 10 Func 0 - USB: xHCI */
	Package() { 0x0010FFFF, 0, 0, 18},

	/* Bus 0, Dev 17 - SATA controller */
	Package() { 0x0011FFFF, 0, 0, 19 },
})

/* GPP 0 */
Name (PS4, Package()
{
	Package() { 0x0000FFFF, 0, INTA, 0 },
	Package() { 0x0000FFFF, 1, INTB, 0 },
	Package() { 0x0000FFFF, 2, INTC, 0 },
	Package() { 0x0000FFFF, 3, INTD, 0 },
})
Name (APS4, Package()
{	/* IOAPIC2BASE + (group * 4) == 24 + (0 * 4), no swizzle */
	/* PCIe slot - Hooked to PCIe slot 4 */
	Package() { 0x0000FFFF, 0, 0, 24 },
	Package() { 0x0000FFFF, 1, 0, 25 },
	Package() { 0x0000FFFF, 2, 0, 26 },
	Package() { 0x0000FFFF, 3, 0, 27 },
})

/* GPP 1 */
Name (PS5, Package()
{
	Package() { 0x0000FFFF, 0, INTA, 0 },
	Package() { 0x0000FFFF, 1, INTB, 0 },
	Package() { 0x0000FFFF, 2, INTC, 0 },
	Package() { 0x0000FFFF, 3, INTD, 0 },
})
Name (APS5, Package()
{	/* IOAPIC2BASE + (group * 4) == 24 + (2 * 4), no swizzle */
	Package() { 0x0000FFFF, 0, 0, 32 },
	Package() { 0x0000FFFF, 1, 0, 33 },
	Package() { 0x0000FFFF, 2, 0, 34 },
	Package() { 0x0000FFFF, 3, 0, 35 },
})

/* GPP 2 */
Name (PS6, Package()
{
	Package() { 0x0000FFFF, 0, INTA, 0 },
	Package() { 0x0000FFFF, 1, INTB, 0 },
	Package() { 0x0000FFFF, 2, INTC, 0 },
	Package() { 0x0000FFFF, 3, INTD, 0 },
})
Name (APS6, Package()
{	/* IOAPIC2BASE + (group * 4) == 24 + (4 * 4), no swizzle */
	Package() { 0x0000FFFF, 0, 0, 40 },
	Package() { 0x0000FFFF, 1, 0, 41 },
	Package() { 0x0000FFFF, 2, 0, 42 },
	Package() { 0x0000FFFF, 3, 0, 43 },
})

/* GPP 3 */
Name (PS7, Package()
{
	Package() { 0x0000FFFF, 0, INTA, 0 },
	Package() { 0x0000FFFF, 1, INTB, 0 },
	Package() { 0x0000FFFF, 2, INTC, 0 },
	Package() { 0x0000FFFF, 3, INTD, 0 },
})
Name (APS7, Package()
{	/* IOAPIC2BASE + (group * 4) == 24 + (6 * 4), no swizzle */
	Package() { 0x0000FFFF, 0, 0, 48 },
	Package() { 0x0000FFFF, 1, 0, 49 },
	Package() { 0x0000FFFF, 2, 0, 50 },
	Package() { 0x0000FFFF, 3, 0, 51 },
})

/* GPP 4 */
Name(PS8, Package(){
	Package(){0x0000FFFF, 0, INTA, 0 },
	Package(){0x0000FFFF, 1, INTB, 0 },
	Package(){0x0000FFFF, 2, INTC, 0 },
	Package(){0x0000FFFF, 3, INTD, 0 },
})
Name (APS8, Package()
{	/* IOAPIC2BASE + (group * 4) == 24 + (0 * 4), DABC swizzle */
	Package() { 0x0000FFFF, 0, 0, 27 },
	Package() { 0x0000FFFF, 1, 0, 24 },
	Package() { 0x0000FFFF, 2, 0, 25 },
	Package() { 0x0000FFFF, 3, 0, 26 },
})
