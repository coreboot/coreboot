/* SPDX-License-Identifier: GPL-2.0-only */

#define MAKE_IIO_DEV(id,rt)						\
	Device (PC##id)							\
	{								\
		Name (_HID, EisaId ("PNP0A08") /* PCI Express Bus */)	\
		Name (_CID, EisaId ("PNP0A03") /* PCI Bus */)		\
		Name (_UID, 0x##id)					\
		Method (_PRT, 0, NotSerialized)				\
		{							\
			If (PICM)					\
			{						\
				Return (\_SB_.AR##rt)			\
			}						\
			Return (\_SB_.PR##rt)				\
		}							\
		External(\_SB.RT##id)					\
		Method (_CRS, 0, NotSerialized)				\
		{							\
			Return (\_SB.RT##id)				\
		}							\
		Name (SUPP, 0x00)					\
		Name (CTRL, 0x00)					\
		Name (_PXM, 0x00)  /* _PXM: Device Proximity */		\
		Method (_OSC, 4, NotSerialized)				\
		{							\
			CreateDWordField (Arg3, 0x00, CDW1)		\
			If ((Arg0 == ToUUID ("33db4d5b-1ff7-401c-9657-7441c03dd766") /* PCI Host Bridge Device */))  \
			{							\
				CreateDWordField (Arg3, 0x04, CDW2)		\
				If ((Arg2 > 0x02))				\
				{						\
					CreateDWordField (Arg3, 0x08, CDW3)	\
				}						\
				SUPP = CDW2					\
				CTRL = CDW3					\
				If ((AHPE || ((SUPP & 0x16) != 0x16)))		\
				{						\
					CTRL &= 0x1E				\
					Sleep (0x03E8)				\
				}						\
				/* Never allow SHPC (no SHPC controller in system) */ \
				CTRL &= 0x1D					\
				/* Disable Native PCIe AER handling from OS */	\
				CTRL &= 0x17					\
				If ((Arg1 != One)) /* unknown revision */	\
				{						\
					CDW1 |= 0x08				\
				}						\
				If ((CDW3 != CTRL)) /* capabilities bits were masked */ \
				{						\
					CDW1 |= 0x10				\
				}						\
				CDW3 = CTRL					\
				Return (Arg3)					\
			}							\
			Else							\
			{							\
				/* indicate unrecognized UUID */		\
				CDW1 |= 0x04					\
				DBG0 = 0xEE					\
				Return (Arg3)					\
			}							\
		}								\
	}

MAKE_IIO_DEV(00, 00)
MAKE_IIO_DEV(01, 10)
MAKE_IIO_DEV(02, 20)
MAKE_IIO_DEV(03, 28)

#if (CONFIG_MAX_SOCKET > 1)
MAKE_IIO_DEV(06, 40)
MAKE_IIO_DEV(07, 50)
MAKE_IIO_DEV(08, 60)
MAKE_IIO_DEV(09, 68)
#endif
