/* SPDX-License-Identifier: GPL-2.0-or-later */

#define NVOP_FUNC_SUPPORT		0x00
#define NVOP_FUNC_GET_OBJ_BY_TYPE	0x10
#define NVOP_FUNC_OPTIMUS_CAPS		0x1a
#define NVOP_FUNC_OPTIMUS_STATUS	0x1b

Method (NVOP, 2, Serialized)
{
	Switch (ToInteger (Arg0))
	{
		Case (NVOP_FUNC_SUPPORT)
		{
			Return (ITOB (
				(1 << NVOP_FUNC_SUPPORT) |
				(1 << NVOP_FUNC_OPTIMUS_CAPS) |
				(1 << NVOP_FUNC_OPTIMUS_STATUS)))
		}
		Case (NVOP_FUNC_OPTIMUS_CAPS)
		{
			CreateField(Arg1, 0, 1, FLUP)	/* Flag Update */
			CreateField(Arg1, 1, 1, CSOT)	/* Change configuration Space Owner Target */
			CreateField(Arg1, 2, 1, CSOW)	/* Change configuration Space Owner Write */
			CreateField(Arg1, 24, 2, NPCE)	/* New Power Control Enable setting */

			/* Change Optimus power control capabilities */
			If (ToInteger (FLUP) != 0 && ToInteger (NPCE) != 0)
			{
				OPCS = NPCE
			}

			/* Change PCI configuration space save/restore owner */
			If (ToInteger (CSOW) == 1)
			{
				PCIO = CSOT
			}

			/* Current GPU Control Status */
			If (PSTA () == 1)
			{
				Local0 = 3
			}
			Else
			{
				Local0 = 0
			}

			Return (ITOB (
				(1 << 0) |	/* Optimus Enabled */
				(Local0 << 3) |	/* Current GPU Control Status */
				(0 << 6) |	/* Shared Discrete GPU Hotplug Capabilities */
				(0 << 7) |	/* MUXed DDC/Aux Capabilities */
				(PCIO << 8) |	/* PCIe Configuration Space Owner */
				(1 << 24) |	/* Platform Optimus Power Capabilities */
				(0 << 27)))	/* Optimus HD Audio Codec Capabilities */
		}
		Case (NVOP_FUNC_OPTIMUS_STATUS)
		{
			Return (ITOB (
				(0 << 0) |	/* Optimus Audio Codec Control */
				(0 << 2) |	/* Request GPU Power State */
				(0 << 4) |	/* Evaluate Requested GPU Power State */
				(0 << 5) |	/* Request Optimus Adapter Policy */
				(0 << 7)))	/* Evaluate Requested Optimus Adapter Selection */
		}
	}

	Return (NV_ERROR_UNSUPPORTED)
}
