/* SPDX-License-Identifier: GPL-2.0-only */

/* Intel PCH PCIe support */

Method (IRQM, 1, Serialized) {

	/* Interrupt Map INTA->INTA, INTB->INTB, INTC->INTC, INTD->INTD */
	Name (IQAA, Package () {
		Package () { 0x0000ffff, 0, 0, 16 },
		Package () { 0x0000ffff, 1, 0, 17 },
		Package () { 0x0000ffff, 2, 0, 18 },
		Package () { 0x0000ffff, 3, 0, 19 } })
	Name (IQAP, Package () {
		Package () { 0x0000ffff, 0, 0, 11 },
		Package () { 0x0000ffff, 1, 0, 10 },
		Package () { 0x0000ffff, 2, 0, 11 },
		Package () { 0x0000ffff, 3, 0, 11 } })

	/* Interrupt Map INTA->INTB, INTB->INTC, INTC->INTD, INTD->INTA */
	Name (IQBA, Package () {
		Package () { 0x0000ffff, 0, 0, 17 },
		Package () { 0x0000ffff, 1, 0, 18 },
		Package () { 0x0000ffff, 2, 0, 19 },
		Package () { 0x0000ffff, 3, 0, 16 } })
	Name (IQBP, Package () {
		Package () { 0x0000ffff, 0, 0, 10 },
		Package () { 0x0000ffff, 1, 0, 11 },
		Package () { 0x0000ffff, 2, 0, 11 },
		Package () { 0x0000ffff, 3, 0, 11 } })

	/* Interrupt Map INTA->INTC, INTB->INTD, INTC->INTA, INTD->INTB */
	Name (IQCA, Package () {
		Package () { 0x0000ffff, 0, 0, 18 },
		Package () { 0x0000ffff, 1, 0, 19 },
		Package () { 0x0000ffff, 2, 0, 16 },
		Package () { 0x0000ffff, 3, 0, 17 } })
	Name (IQCP, Package () {
		Package () { 0x0000ffff, 0, 0, 11 },
		Package () { 0x0000ffff, 1, 0, 11 },
		Package () { 0x0000ffff, 2, 0, 11 },
		Package () { 0x0000ffff, 3, 0, 10 } })

	/* Interrupt Map INTA->INTD, INTB->INTA, INTC->INTB, INTD->INTC */
	Name (IQDA, Package () {
		Package () { 0x0000ffff, 0, 0, 19 },
		Package () { 0x0000ffff, 1, 0, 16 },
		Package () { 0x0000ffff, 2, 0, 17 },
		Package () { 0x0000ffff, 3, 0, 18 } })
	Name (IQDP, Package () {
		Package () { 0x0000ffff, 0, 0, 11 },
		Package () { 0x0000ffff, 1, 0, 11 },
		Package () { 0x0000ffff, 2, 0, 10 },
		Package () { 0x0000ffff, 3, 0, 11 } })

	Switch (ToInteger (Arg0))
	{
		Case (Package () { 1, 5, 9, 13
#if CONFIG(SOC_INTEL_TIGERLAKE_PCH_H)
		, 17, 21
#endif
		}) {
			If (PICM) {
				Return (IQAA)
			} Else {
				Return (IQAP)
			}
		}

		Case (Package () { 2, 6, 10, 14
#if CONFIG(SOC_INTEL_TIGERLAKE_PCH_H)
		, 18, 22
#endif
		}) {
			If (PICM) {
				Return (IQBA)
			} Else {
				Return (IQBP)
			}
		}

		Case (Package () { 3, 7, 11, 15
#if CONFIG(SOC_INTEL_TIGERLAKE_PCH_H)
		, 19, 23
#endif
		}) {
			If (PICM) {
				Return (IQCA)
			} Else {
				Return (IQCP)
			}
		}

		Case (Package () { 4, 8, 12, 16
#if CONFIG(SOC_INTEL_TIGERLAKE_PCH_H)
		, 20, 24
#endif
		}) {
			If (PICM) {
				Return (IQDA)
			} Else {
				Return (IQDP)
			}
		}

		Default {
			If (PICM) {
				Return (IQDA)
			} Else {
				Return (IQDP)
			}
		}
	}
}

Device (PEG0)
{
	Name (_ADR, 0x00060000)

	OperationRegion (RPCS, PCI_Config, 0x4c, 4)
	Field (RPCS, AnyAcc, NoLock, Preserve)
	{
		, 24,
		RPPN, 8,	/* Root Port Number */
	}

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

#if CONFIG(SOC_INTEL_TIGERLAKE_PCH_H)
Device (PEG1)
{
	Name (_ADR, 0x00010000)

	OperationRegion (RPCS, PCI_Config, 0x4c, 4)
	Field (RPCS, AnyAcc, NoLock, Preserve)
	{
		, 24,
		RPPN, 8,	/* Root Port Number */
	}

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (PEG2)
{
	Name (_ADR, 0x00010001)

	OperationRegion (RPCS, PCI_Config, 0x4c, 4)
	Field (RPCS, AnyAcc, NoLock, Preserve)
	{
		, 24,
		RPPN, 8,	/* Root Port Number */
	}

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (PEG3)
{
	Name (_ADR, 0x00010002)

	OperationRegion (RPCS, PCI_Config, 0x4c, 4)
	Field (RPCS, AnyAcc, NoLock, Preserve)
	{
		, 24,
		RPPN, 8,	/* Root Port Number */
	}

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}
#endif

Device (RP01)
{
	Name (_ADR, 0x001C0000)

	OperationRegion (RPCS, PCI_Config, 0x4c, 4)
	Field (RPCS, AnyAcc, NoLock, Preserve)
	{
		, 24,
		RPPN, 8,	/* Root Port Number */
	}

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP02)
{
	Name (_ADR, 0x001C0001)

	OperationRegion (RPCS, PCI_Config, 0x4c, 4)
	Field (RPCS, AnyAcc, NoLock, Preserve)
	{
		, 24,
		RPPN, 8,	/* Root Port Number */
	}

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP03)
{
	Name (_ADR, 0x001C0002)

	OperationRegion (RPCS, PCI_Config, 0x4c, 4)
	Field (RPCS, AnyAcc, NoLock, Preserve)
	{
		, 24,
		RPPN, 8,	/* Root Port Number */
	}

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP04)
{
	Name (_ADR, 0x001C0003)

	OperationRegion (RPCS, PCI_Config, 0x4c, 4)
	Field (RPCS, AnyAcc, NoLock, Preserve)
	{
		, 24,
		RPPN, 8,	/* Root Port Number */
	}

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP05)
{
	Name (_ADR, 0x001C0004)

	OperationRegion (RPCS, PCI_Config, 0x4c, 4)
	Field (RPCS, AnyAcc, NoLock, Preserve)
	{
		, 24,
		RPPN, 8,	/* Root Port Number */
	}

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP06)
{
	Name (_ADR, 0x001C0005)

	OperationRegion (RPCS, PCI_Config, 0x4c, 4)
	Field (RPCS, AnyAcc, NoLock, Preserve)
	{
		, 24,
		RPPN, 8,	/* Root Port Number */
	}

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP07)
{
	Name (_ADR, 0x001C0006)

	OperationRegion (RPCS, PCI_Config, 0x4c, 4)
	Field (RPCS, AnyAcc, NoLock, Preserve)
	{
		, 24,
		RPPN, 8,	/* Root Port Number */
	}

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP08)
{
	Name (_ADR, 0x001C0007)

	OperationRegion (RPCS, PCI_Config, 0x4c, 4)
	Field (RPCS, AnyAcc, NoLock, Preserve)
	{
		, 24,
		RPPN, 8,	/* Root Port Number */
	}

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP09)
{
	Name (_ADR, 0x001D0000)

	OperationRegion (RPCS, PCI_Config, 0x4c, 4)
	Field (RPCS, AnyAcc, NoLock, Preserve)
	{
		, 24,
		RPPN, 8,	/* Root Port Number */
	}

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP10)
{
	Name (_ADR, 0x001D0001)

	OperationRegion (RPCS, PCI_Config, 0x4c, 4)
	Field (RPCS, AnyAcc, NoLock, Preserve)
	{
		, 24,
		RPPN, 8,	/* Root Port Number */
	}

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP11)
{
	Name (_ADR, 0x001D0002)

	OperationRegion (RPCS, PCI_Config, 0x4c, 4)
	Field (RPCS, AnyAcc, NoLock, Preserve)
	{
		, 24,
		RPPN, 8,	/* Root Port Number */
	}

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP12)
{
	Name (_ADR, 0x001D0003)

	OperationRegion (RPCS, PCI_Config, 0x4c, 4)
	Field (RPCS, AnyAcc, NoLock, Preserve)
	{
		, 24,
		RPPN, 8,	/* Root Port Number */
	}

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

#if CONFIG(SOC_INTEL_TIGERLAKE_PCH_H)
Device (RP17)
{
	Name (_ADR, 0x001B0000)

	OperationRegion (RPCS, PCI_Config, 0x4c, 4)
	Field (RPCS, AnyAcc, NoLock, Preserve)
	{
		, 24,
		RPPN, 8,	/* Root Port Number */
	}

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP18)
{
	Name (_ADR, 0x001B0001)

	OperationRegion (RPCS, PCI_Config, 0x4c, 4)
	Field (RPCS, AnyAcc, NoLock, Preserve)
	{
		, 24,
		RPPN, 8,	/* Root Port Number */
	}

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP19)
{
	Name (_ADR, 0x001B0002)

	OperationRegion (RPCS, PCI_Config, 0x4c, 4)
	Field (RPCS, AnyAcc, NoLock, Preserve)
	{
		, 24,
		RPPN, 8,	/* Root Port Number */
	}

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP20)
{
	Name (_ADR, 0x001B0003)

	OperationRegion (RPCS, PCI_Config, 0x4c, 4)
	Field (RPCS, AnyAcc, NoLock, Preserve)
	{
		, 24,
		RPPN, 8,	/* Root Port Number */
	}

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP21)
{
	Name (_ADR, 0x001B0004)

	OperationRegion (RPCS, PCI_Config, 0x4c, 4)
	Field (RPCS, AnyAcc, NoLock, Preserve)
	{
		, 24,
		RPPN, 8,	/* Root Port Number */
	}

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP22)
{
	Name (_ADR, 0x001B0005)

	OperationRegion (RPCS, PCI_Config, 0x4c, 4)
	Field (RPCS, AnyAcc, NoLock, Preserve)
	{
		, 24,
		RPPN, 8,	/* Root Port Number */
	}

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP23)
{
	Name (_ADR, 0x001B0006)

	OperationRegion (RPCS, PCI_Config, 0x4c, 4)
	Field (RPCS, AnyAcc, NoLock, Preserve)
	{
		, 24,
		RPPN, 8,	/* Root Port Number */
	}

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}

Device (RP24)
{
	Name (_ADR, 0x001B0007)

	OperationRegion (RPCS, PCI_Config, 0x4c, 4)
	Field (RPCS, AnyAcc, NoLock, Preserve)
	{
		, 24,
		RPPN, 8,	/* Root Port Number */
	}

	Method (_PRT)
	{
		Return (IRQM (RPPN))
	}
}
#endif /* CONFIG(SOC_INTEL_TIGERLAKE_PCH_H) */
