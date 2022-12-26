/* SPDX-License-Identifier: GPL-2.0-only */

/* Note: Only need HID on Primary Bus */
External (TOM1)
External (TOM2)
Name(_HID, EISAID("PNP0A08"))	/* PCI Express Root Bridge */
Name(_CID, EISAID("PNP0A03"))	/* PCI Root Bridge */

/* Describe the Northbridge devices */

Method(_BBN, 0, NotSerialized)	/* Bus number = 0 */
{
	Return(0)
}

Method(_STA, 0, NotSerialized)
{
	Return(0x0B)	/* Status is visible */
}

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
