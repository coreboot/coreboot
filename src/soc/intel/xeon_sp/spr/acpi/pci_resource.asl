/* SPDX-License-Identifier: GPL-2.0-or-later */

#define _IIO_DEVICE_NAME(str, skt, stk)     str##skt##stk
#define IIO_DEVICE_NAME(str, skt, stk)      _IIO_DEVICE_NAME(str, skt, stk)
#define IIO_RESOURCE_NAME(res, str, skt, stk)      res##str##skt##stk

#define STR(s)  #s
#define _IIO_DEVICE_UID(str, skt, stk)      STR(str##skt##stk)
#define IIO_DEVICE_UID(str, skt, stk)       _IIO_DEVICE_UID(str, skt, stk)

Device (IIO_DEVICE_NAME(DEVPREFIX, SOCKET, STACK))
{
	Name (_HID, EisaId ("PNP0A08") /* PCI Express Bus */)
	Name (_CID, EisaId ("PNP0A03") /* PCI Bus */)
	Name (_UID, IIO_DEVICE_UID(DEVPREFIX, SOCKET, STACK))
	External (\_SB.IIO_DEVICE_NAME(STPREFIX, SOCKET, STACK))
	Method (_STA, 0, NotSerialized)
	{
		Return (\_SB.IIO_DEVICE_NAME(STPREFIX, SOCKET, STACK))
	}
	Method (_PRT, 0, NotSerialized)
	{
		Return (\_SB.PRTID)
	}
	External (\_SB.IIO_DEVICE_NAME(RESPREFIX, SOCKET, STACK))
	Method (_CRS, 0, NotSerialized)
	{
		Return (\_SB.IIO_DEVICE_NAME(RESPREFIX, SOCKET, STACK))
	}
	Name (SUPP, 0x00)
	Name (CTRL, 0x00)
	Name (_PXM, SOCKET)  /* _PXM: Device Proximity */
	Method (_OSC, 4, NotSerialized)
	{
		CreateDWordField (Arg3, 0x00, CDW1)
		If (Arg0 == ToUUID ("33db4d5b-1ff7-401c-9657-7441c03dd766") /* PCI Host Bridge Device */)
		{
			If (Arg2 < 0x03) /* Number of DWORDs in Arg3 must be at least 3 */
			{
				CDW1 |= 0x02 /* Unknown failure */
				Return (Arg3)
			}
			CreateDWordField (Arg3, 0x04, CDW2)
			CreateDWordField (Arg3, 0x08, CDW3)

			SUPP = CDW2
			CTRL = CDW3
			If (SUPP & 0x16 != 0x16)
			{
				CTRL &= 0x1E
			}
			/* Never allow SHPC (no SHPC controller in system) */
			CTRL &= 0x1D
			/* Disable Native PCIe AER handling from OS */
			CTRL &= 0x17

			If (Arg1 != 1) /* unknown revision */
			{
				CDW1 |= 0x08
			}
			If (CDW3 != CTRL) /* capabilities bits were masked */
			{
				CDW1 |= 0x10
			}
			CDW3 = CTRL
			Return (Arg3)
		}
		Else
		{
			/* indicate unrecognized UUID */
			CDW1 |= 0x04
			IO80 = 0xEE
			Return (Arg3)
		}
	}
}
