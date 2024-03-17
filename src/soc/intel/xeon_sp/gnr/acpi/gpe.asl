/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/iomap.h>

Scope (\_SB)
{
	Scope (\_GPE)
	{
		OperationRegion (PMIO, SystemIO, ACPI_BASE_ADDRESS, 0xFF)
		Field (PMIO, ByteAcc, NoLock, Preserve) {
			Offset(0x34),   /* 0x34, SMI/SCI STS*/
				,  9,
			SGCS, 1,        /* SWGPE STS BIT */

			Offset(0x40),   /* 0x40, SMI/SCI_EN*/
				,  17,
			SGPC, 1,        /* SWGPE CTRL BIT */

			Offset(0x6C),   /* 0x6C, General Purpose Event 0 Status [127:96] */
				,  2,
			SGPS,  1,       /* SWGPE STATUS */

			Offset(0x7C),   /* 0x7C, General Purpose Event 0 Enable [127:96] */
				,  2,
			SGPE,  1        /* SWGPE ENABLE */
		}
		Method (_L62, 0, NotSerialized)
		{
			DBGO("\\_GPE\\_L62\n")
			SGPC = 0 // clear SWGPE control
			SGPS = 1 // clear SWGPE Status
		}
	}
}
