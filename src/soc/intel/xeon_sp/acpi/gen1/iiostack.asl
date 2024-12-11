/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/intel/xeon_sp/acpi/iiostack.asl>
#include <soc/acpi.h>

#define MAKE_IIO_DEV(id,rt,pxm)						\
	Device (PC##id)							\
	{								\
		Name (_HID, EisaId ("PNP0A08") /* PCI Express Bus */)	\
		Name (_CID, EisaId ("PNP0A03") /* PCI Bus */)		\
		Name (_UID, 0x##id)					\
		Method (_STA, 0, NotSerialized)				\
		{							\
			If (CondRefOf (_CRS))				\
			{						\
				Return (0xf)				\
			}						\
			Else						\
			{						\
				Return (0)				\
			}						\
		}							\
		Method (_PRT, 0, NotSerialized)				\
		{							\
			If (PICM)					\
			{						\
				Return (\_SB_.AR##rt)			\
			}						\
			Return (\_SB_.PR##rt)				\
		}							\
		Name (_PXM, pxm)  /* _PXM: Device Proximity */		\
		Method (_OSC, 4, NotSerialized)				\
		{							\
			Return (\_SB.POSC(Arg0, Arg1, Arg2, Arg3,	\
				(PCIE_CAP_STRUCTURE_CONTROL|		\
				PCIE_PME_CONTROL|			\
				PCIE_NATIVE_HOTPLUG_CONTROL), 0 , 0))	\
		}							\
	}

// Keep in sync with iio_domain_set_acpi_name()!
MAKE_IIO_DEV(00, 00, 0)
MAKE_IIO_DEV(01, 10, 0)
MAKE_IIO_DEV(02, 20, 0)
MAKE_IIO_DEV(03, 28, 0)

#if (CONFIG_MAX_SOCKET > 1)
MAKE_IIO_DEV(20, 40, 1)
MAKE_IIO_DEV(21, 50, 1)
MAKE_IIO_DEV(22, 60, 1)
MAKE_IIO_DEV(23, 68, 1)
#endif
