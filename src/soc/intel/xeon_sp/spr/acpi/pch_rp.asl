/* SPDX-License-Identifier: GPL-2.0-or-later */

Device (RP01)
{
	Name (LTRZ, 0x00)
	Name (LMSL, 0x00)
	Name (LNSL, 0x00)
	Method (_ADR, 0, NotSerialized)
	{
		Return (0x00080000)
	}

	Name (SLOT, 0x01)

	OperationRegion (PXCS, PCI_Config, 0x00, 0x0900)
	Field (PXCS, AnyAcc, NoLock, Preserve)
	{
		VDID,   32,
		Offset (0x60),
		Offset (0x62),
		PSPX,   1
	}

	Field (PXCS, AnyAcc, NoLock, WriteAsZeros)
	{
		Offset (0xCC),
		Offset (0xCE),
		PMSX,   1
	}

	Method (HPME, 0, Serialized)
	{
		If (VDID != 0xFFFFFFFF && PMSX == 0x01)
		{
			Notify (PXSX, 0x02) // Device Wake
			PMSX = 0x01
			PSPX = 0x01
		}
	}

	Device (PXSX)
	{
		Name (_ADR, 0x00)
	}
}

Device (RP02)
{
	Name (LTRZ, 0x00)
	Name (LMSL, 0x00)
	Name (LNSL, 0x00)
	Method (_ADR, 0, NotSerialized)
	{
		Return (0x00090000)
	}

	Name (SLOT, 0x02)

	OperationRegion (PXCS, PCI_Config, 0x00, 0x0900)
	Field (PXCS, AnyAcc, NoLock, Preserve)
	{
		VDID,   32,
		Offset (0x60),
		Offset (0x62),
		PSPX,   1
	}

	Field (PXCS, AnyAcc, NoLock, WriteAsZeros)
	{
		Offset (0xCC),
		Offset (0xCE),
		PMSX,   1
	}

	Method (HPME, 0, Serialized)
	{
		If (VDID != 0xFFFFFFFF && PMSX == 0x01)
		{
			Notify (PXSX, 0x02) // Device Wake
			PMSX = 0x01
			PSPX = 0x01
		}
	}

	Device (PXSX)
	{
		Name (_ADR, 0x00)
	}
}

Device (RP03)
{
	Name (LTRZ, 0x00)
	Name (LMSL, 0x00)
	Name (LNSL, 0x00)
	Method (_ADR, 0, NotSerialized)
	{
		Return (0x000A0000)
	}

	Name (SLOT, 0x03)

	OperationRegion (PXCS, PCI_Config, 0x00, 0x0900)
	Field (PXCS, AnyAcc, NoLock, Preserve)
	{
		VDID,   32,
		Offset (0x60),
		Offset (0x62),
		PSPX,   1
	}

	Field (PXCS, AnyAcc, NoLock, WriteAsZeros)
	{
		Offset (0xCC),
		Offset (0xCE),
		PMSX,   1
	}

	Method (HPME, 0, Serialized)
	{
		If (VDID != 0xFFFFFFFF && PMSX == 0x01)
		{
			Notify (PXSX, 0x02) // Device Wake
			PMSX = 0x01
			PSPX = 0x01
		}
	}

	Device (PXSX)
	{
		Name (_ADR, 0x00)
	}
}

Device (RP04)
{
	Name (LTRZ, 0x00)
	Name (LMSL, 0x00)
	Name (LNSL, 0x00)
	Method (_ADR, 0, NotSerialized)
	{
		Return (0x000B0000)
	}

	Name (SLOT, 0x04)

	OperationRegion (PXCS, PCI_Config, 0x00, 0x0900)
	Field (PXCS, AnyAcc, NoLock, Preserve)
	{
		VDID,   32,
		Offset (0x60),
		Offset (0x62),
		PSPX,   1
	}

	Field (PXCS, AnyAcc, NoLock, WriteAsZeros)
	{
		Offset (0xCC),
		Offset (0xCE),
		PMSX,   1
	}

	Method (HPME, 0, Serialized)
	{
		If (VDID != 0xFFFFFFFF && PMSX == 0x01)
		{
			Notify (PXSX, 0x02) // Device Wake
			PMSX = 0x01
			PSPX = 0x01
		}
	}

	Device (PXSX)
	{
		Name (_ADR, 0x00)
	}
}

Device (RP05)
{
	Name (LTRZ, 0x00)
	Name (LMSL, 0x00)
	Name (LNSL, 0x00)
	Method (_ADR, 0, NotSerialized)
	{
		Return (0x000C0000)
	}

	Name (SLOT, 0x05)

	OperationRegion (PXCS, PCI_Config, 0x00, 0x0900)
	Field (PXCS, AnyAcc, NoLock, Preserve)
	{
		VDID,   32,
		Offset (0x60),
		Offset (0x62),
		PSPX,   1
	}

	Field (PXCS, AnyAcc, NoLock, WriteAsZeros)
	{
		Offset (0xCC),
		Offset (0xCE),
		PMSX,   1
	}

	Method (HPME, 0, Serialized)
	{
		If (VDID != 0xFFFFFFFF && PMSX == 0x01)
		{
			Notify (PXSX, 0x02) // Device Wake
			PMSX = 0x01
			PSPX = 0x01
		}
	}

	Device (PXSX)
	{
		Name (_ADR, 0x00)
	}
}

Device (RP06)
{
	Name (LTRZ, 0x00)
	Name (LMSL, 0x00)
	Name (LNSL, 0x00)
	Method (_ADR, 0, NotSerialized)
	{
		Return (0x000D0000)
	}

	Name (SLOT, 0x06)

	OperationRegion (PXCS, PCI_Config, 0x00, 0x0900)
	Field (PXCS, AnyAcc, NoLock, Preserve)
	{
		VDID,   32,
		Offset (0x60),
		Offset (0x62),
		PSPX,   1
	}

	Field (PXCS, AnyAcc, NoLock, WriteAsZeros)
	{
		Offset (0xCC),
		Offset (0xCE),
		PMSX,   1
	}

	Method (HPME, 0, Serialized)
	{
		If (VDID != 0xFFFFFFFF && PMSX == 0x01)
		{
			Notify (PXSX, 0x02) // Device Wake
			PMSX = 0x01
			PSPX = 0x01
		}
	}

	Device (PXSX)
	{
		Name (_ADR, 0x00)
	}
}

Device (RP07)
{
	Name (LTRZ, 0x00)
	Name (LMSL, 0x00)
	Name (LNSL, 0x00)
	Method (_ADR, 0, NotSerialized)
	{
		Return (0x000E0000)
	}

	Name (SLOT, 0x07)

	OperationRegion (PXCS, PCI_Config, 0x00, 0x0900)
	Field (PXCS, AnyAcc, NoLock, Preserve)
	{
		VDID,   32,
		Offset (0x60),
		Offset (0x62),
		PSPX,   1
	}

	Field (PXCS, AnyAcc, NoLock, WriteAsZeros)
	{
		Offset (0xCC),
		Offset (0xCE),
		PMSX,   1
	}

	Method (HPME, 0, Serialized)
	{
		If (VDID != 0xFFFFFFFF && PMSX == 0x01)
		{
			Notify (PXSX, 0x02) // Device Wake
			PMSX = 0x01
			PSPX = 0x01
		}
	}

	Device (PXSX)
	{
		Name (_ADR, 0x00)
	}
}

Device (RP08)
{
	Name (LTRZ, 0x00)
	Name (LMSL, 0x00)
	Name (LNSL, 0x00)
	Method (_ADR, 0, NotSerialized)
	{
		Return (0x000F0000)
	}

	Name (SLOT, 0x08)

	OperationRegion (PXCS, PCI_Config, 0x00, 0x0900)
	Field (PXCS, AnyAcc, NoLock, Preserve)
	{
		VDID,   32,
		Offset (0x60),
		Offset (0x62),
		PSPX,   1
	}

	Field (PXCS, AnyAcc, NoLock, WriteAsZeros)
	{
		Offset (0xCC),
		Offset (0xCE),
		PMSX,   1
	}

	Method (HPME, 0, Serialized)
	{
		If (VDID != 0xFFFFFFFF && PMSX == 0x01)
		{
			Notify (PXSX, 0x02) // Device Wake
			PMSX = 0x01
			PSPX = 0x01
		}
	}

	Device (PXSX)
	{
		Name (_ADR, 0x00)
	}
}

Device (RP09)
{
	Name (LTRZ, 0x00)
	Name (LMSL, 0x00)
	Name (LNSL, 0x00)
	Method (_ADR, 0, NotSerialized)
	{
		Return (0x00100000)
	}

	Name (SLOT, 0x09)

	OperationRegion (PXCS, PCI_Config, 0x00, 0x0900)
	Field (PXCS, AnyAcc, NoLock, Preserve)
	{
		VDID,   32,
		Offset (0x60),
		Offset (0x62),
		PSPX,   1
	}

	Field (PXCS, AnyAcc, NoLock, WriteAsZeros)
	{
		Offset (0xCC),
		Offset (0xCE),
		PMSX,   1
	}

	Method (HPME, 0, Serialized)
	{
		If (VDID != 0xFFFFFFFF && PMSX == 0x01)
		{
			Notify (PXSX, 0x02) // Device Wake
			PMSX = 0x01
			PSPX = 0x01
		}
	}

	Device (PXSX)
	{
		Name (_ADR, 0x00)
	}
}

Device (RP10)
{
	Name (LTRZ, 0x00)
	Name (LMSL, 0x00)
	Name (LNSL, 0x00)
	Method (_ADR, 0, NotSerialized)
	{
		Return (0x00110000)
	}

	Name (SLOT, 0x0A)

	OperationRegion (PXCS, PCI_Config, 0x00, 0x0900)
	Field (PXCS, AnyAcc, NoLock, Preserve)
	{
		VDID,   32,
		Offset (0x60),
		Offset (0x62),
		PSPX,   1
	}

	Field (PXCS, AnyAcc, NoLock, WriteAsZeros)
	{
		Offset (0xCC),
		Offset (0xCE),
		PMSX,   1
	}

	Method (HPME, 0, Serialized)
	{
		If (VDID != 0xFFFFFFFF && PMSX == 0x01)
		{
			Notify (PXSX, 0x02) // Device Wake
			PMSX = 0x01
			PSPX = 0x01
		}
	}

	Device (PXSX)
	{
		Name (_ADR, 0x00)
	}
}

Device (RP11)
{
	Name (LTRZ, 0x00)
	Name (LMSL, 0x00)
	Name (LNSL, 0x00)
	Method (_ADR, 0, NotSerialized)
	{
		Return (0x00120000)
	}

	Name (SLOT, 0x0B)

	OperationRegion (PXCS, PCI_Config, 0x00, 0x0900)
	Field (PXCS, AnyAcc, NoLock, Preserve)
	{
		VDID,   32,
		Offset (0x60),
		Offset (0x62),
		PSPX,   1
	}

	Field (PXCS, AnyAcc, NoLock, WriteAsZeros)
	{
		Offset (0xCC),
		Offset (0xCE),
		PMSX,   1
	}

	Method (HPME, 0, Serialized)
	{
		If (VDID != 0xFFFFFFFF && PMSX == 0x01)
		{
			Notify (PXSX, 0x02) // Device Wake
			PMSX = 0x01
			PSPX = 0x01
		}
	}

	Device (PXSX)
	{
		Name (_ADR, 0x00)
	}
}

Device (RP12)
{
	Name (LTRZ, 0x00)
	Name (LMSL, 0x00)
	Name (LNSL, 0x00)
	Method (_ADR, 0, NotSerialized)
	{
		Return (0x00130000)
	}

	Name (SLOT, 0x0C)

	OperationRegion (PXCS, PCI_Config, 0x00, 0x0900)
	Field (PXCS, AnyAcc, NoLock, Preserve)
	{
		VDID,   32,
		Offset (0x60),
		Offset (0x62),
		PSPX,   1
	}

	Field (PXCS, AnyAcc, NoLock, WriteAsZeros)
	{
		Offset (0xCC),
		Offset (0xCE),
		PMSX,   1
	}

	Method (HPME, 0, Serialized)
	{
		If (VDID != 0xFFFFFFFF && PMSX == 0x01)
		{
			Notify (PXSX, 0x02) // Device Wake
			PMSX = 0x01
			PSPX = 0x01
		}
	}

	Device (PXSX)
	{
		Name (_ADR, 0x00)
	}
}

Device (RP13)
{
	Name (LTRZ, 0x00)
	Name (LMSL, 0x00)
	Name (LNSL, 0x00)
	Method (_ADR, 0, NotSerialized)
	{
		Return (0x001A0000)
	}

	Name (SLOT, 0x0D)

	OperationRegion (PXCS, PCI_Config, 0x00, 0x0900)
	Field (PXCS, AnyAcc, NoLock, Preserve)
	{
		VDID,   32,
		Offset (0x60),
		Offset (0x62),
		PSPX,   1
	}

	Field (PXCS, AnyAcc, NoLock, WriteAsZeros)
	{
		Offset (0xCC),
		Offset (0xCE),
		PMSX,   1
	}

	Method (HPME, 0, Serialized)
	{
		If (VDID != 0xFFFFFFFF && PMSX == 0x01)
		{
			Notify (PXSX, 0x02) // Device Wake
			PMSX = 0x01
			PSPX = 0x01
		}
	}

	Device (PXSX)
	{
		Name (_ADR, 0x00)
	}
}

Device (RP14)
{
	Name (LTRZ, 0x00)
	Name (LMSL, 0x00)
	Name (LNSL, 0x00)
	Method (_ADR, 0, NotSerialized)
	{
		Return (0x001B0000)
	}

	Name (SLOT, 0x0E)

	OperationRegion (PXCS, PCI_Config, 0x00, 0x0900)
	Field (PXCS, AnyAcc, NoLock, Preserve)
	{
		VDID,   32,
		Offset (0x60),
		Offset (0x62),
		PSPX,   1
	}

	Field (PXCS, AnyAcc, NoLock, WriteAsZeros)
	{
		Offset (0xCC),
		Offset (0xCE),
		PMSX,   1
	}

	Method (HPME, 0, Serialized)
	{
		If (VDID != 0xFFFFFFFF && PMSX == 0x01)
		{
			Notify (PXSX, 0x02) // Device Wake
			PMSX = 0x01
			PSPX = 0x01
		}
	}

	Device (PXSX)
	{
		Name (_ADR, 0x00)
	}
}

Device (RP15)
{
	Name (LTRZ, 0x00)
	Name (LMSL, 0x00)
	Name (LNSL, 0x00)
	Method (_ADR, 0, NotSerialized)
	{
		Return (0x001C0000)
	}

	Name (SLOT, 0x0F)

	OperationRegion (PXCS, PCI_Config, 0x00, 0x0900)
	Field (PXCS, AnyAcc, NoLock, Preserve)
	{
		VDID,   32,
		Offset (0x60),
		Offset (0x62),
		PSPX,   1
	}

	Field (PXCS, AnyAcc, NoLock, WriteAsZeros)
	{
		Offset (0xCC),
		Offset (0xCE),
		PMSX,   1
	}

	Method (HPME, 0, Serialized)
	{
		If (VDID != 0xFFFFFFFF && PMSX == 0x01)
		{
			Notify (PXSX, 0x02) // Device Wake
			PMSX = 0x01
			PSPX = 0x01
		}
	}

	Device (PXSX)
	{
		Name (_ADR, 0x00)
	}
}

Device (RP16)
{
	Name (LTRZ, 0x00)
	Name (LMSL, 0x00)
	Name (LNSL, 0x00)
	Method (_ADR, 0, NotSerialized)
	{
		Return (0x001C0000)
	}

	Name (SLOT, 0x10)

	OperationRegion (PXCS, PCI_Config, 0x00, 0x0900)
	Field (PXCS, AnyAcc, NoLock, Preserve)
	{
		VDID,   32,
		Offset (0x60),
		Offset (0x62),
		PSPX,   1
	}

	Field (PXCS, AnyAcc, NoLock, WriteAsZeros)
	{
		Offset (0xCC),
		Offset (0xCE),
		PMSX,   1
	}

	Method (HPME, 0, Serialized)
	{
		If (VDID != 0xFFFFFFFF && PMSX == 0x01)
		{
			Notify (PXSX, 0x02) // Device Wake
			PMSX = 0x01
			PSPX = 0x01
		}
	}

	Device (PXSX)
	{
		Name (_ADR, 0x00)
	}
}
