/* SPDX-License-Identifier: GPL-2.0-only */

/* Describe the Northbridge devices */

/* PCI Routing Table */
Name(PR0, Package(){
	/* Bus 0, Dev 0x00 - F2: IOMMU */
	Package() { 0x0000FFFF, 0, INTA, 0 },
	Package() { 0x0000FFFF, 0, INTB, 0 },
	Package() { 0x0000FFFF, 0, INTC, 0 },
	Package() { 0x0000FFFF, 0, INTD, 0 },

	/* Bus 0, Dev 0x14 - F[0:SMBus 3:LPC] */
	Package() { 0x0014FFFF, 0, INTA, 0 },
	Package() { 0x0014FFFF, 1, INTB, 0 },
	Package() { 0x0014FFFF, 2, INTC, 0 },
	Package() { 0x0014FFFF, 3, INTD, 0 },
})

Method(_PRT,0, NotSerialized)
{
	Return(PR0)
}
