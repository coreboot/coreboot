/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "gpu_defines.h"

Scope (\_SB.PCI0.PEG0)
{
	#include "peg.asl"

	Device (PEGP)
	{
		Name (_ADR, 0x0)
		OperationRegion (PCIC, PCI_Config, 0x00, 0x100)
		Field (PCIC, DWordAcc, NoLock, Preserve)
		{
			NVID,	16,
			NDID,	16,
			CMDR,	8,
			VGAR,	2008,			/* VGA Registers */
		}

		#include "utility.asl"
		#include "power.asl"
		#include "nvop.asl"
		#include "nvjt.asl"
		#include "nbci.asl"

		Method (_DSM, 4, Serialized)
		{
			If (Arg0 == ToUUID (UUID_NVOP))
			{
				If (ToInteger(Arg1) >= REVISION_MIN_NVOP)
				{
					Return (NVOP (Arg2, Arg3))
				}
			}
			ElseIf (Arg0 == ToUUID (UUID_NVJT))
			{
				If (ToInteger (Arg1) >= REVISION_MIN_NVJT)
				{
					Return (NVJT (Arg2, Arg3))
				}
			}
			ElseIf (Arg0 == ToUUID (UUID_NBCI))
			{
				If (ToInteger (Arg1) >= REVISION_MIN_NBCI)
				{
					Return (NBCI (Arg2, Arg3))
				}
			}

			Return (NV_ERROR_UNSUPPORTED)
		}
	}
}
