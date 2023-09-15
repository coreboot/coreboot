/* SPDX-License-Identifier: GPL-2.0-or-later */

#define GPS_FUNC_SUPPORT	0
#define GPS_FUNC_GETCALLBACKS	0x13
#define GPS_FUNC_PSHARESTATUS	0x20
#define GPS_FUNC_PSHAREPARAMS	0x2a
#define GPS_FUNC_REQUESTDXSTATE	0x12

#define QUERY_GET_STATUS		0
#define QUERY_GET_SUPPORTED_FIELDS	1
#define QUERY_GET_CURRENT_LIMITS	2

#define EC_D_NOTIFY_MASK	0x7

/* GPS return Package */
Name (GPSP, Buffer (0x28) {0x0})
CreateDWordField (GPSP, 0, RETN)
CreateDWordField (GPSP, 4, VRV1)
CreateDWordField (GPSP, 8, TGPU)

/* GETCALLBACKS return value
   [0]: Callback for post-mode set
   [1]: Callback for pre-mode set
   [2]: Callback for post power state transition */
Name (GPSR, Buffer (4) { 0x4, 0x0, 0x0, 0x0 })

Method (GPS, 2, Serialized)
{
	Switch (ToInteger (Arg0))
	{
		Case (GPS_FUNC_SUPPORT)
		{
			Return (LTOB(
				(1 << GPS_FUNC_SUPPORT) |
				(1 << GPS_FUNC_GETCALLBACKS) |
				(1 << GPS_FUNC_PSHARESTATUS) |
				(1 << GPS_FUNC_PSHAREPARAMS) |
				(1 << GPS_FUNC_REQUESTDXSTATE)))
		}
		Case (GPS_FUNC_GETCALLBACKS)
		{
			CreateDWordField (Arg1, 0, QURY)

			/* Driver querying for which callbacks the ACPI code
			   wants callbacks for. */
			If (QURY == 0)
			{
				Return (GPSR)
			}

			If (QURY & 0x4)
			{
				Printf("GPS: Kernel driver callback post power state transition")
				Return (GPSR)
			}
		}
		Case (GPS_FUNC_PSHARESTATUS)
		{
			Return (ITOB(
				(0 << 0) |	/* GPS_FUNC_PSHAREPARAMS should not be called
				                   again after initialization, i.e., its return
						   value is not dynamically updated. */
				(1 << 0) |	/* System uses both a GPIO and ACPI notification
						   codes to set multiple GPU power limits. */
				(0 << 21)))	/* EDPpeak limit is default */
		}
		Case (GPS_FUNC_PSHAREPARAMS)
		{
			CreateField (Arg1, 0, 4, QUTY)	 /* Query type */

			/* Version of return value */
			VRV1 = 0x10000
			Switch (ToInteger (QUTY))
			{
				Case (QUERY_GET_STATUS)
				{
					Return (GPSP)
				}
				Case (QUERY_GET_SUPPORTED_FIELDS)
				{
					/* Only GPU temperature is supported */
					RETN = 0x100 | ToInteger (QUTY)
					Return (GPSP)
				}
				Case (QUERY_GET_CURRENT_LIMITS)
				{
					/* Request a limit of '0' for the GPU temperature,
					   meaning there is no limit. */
					RETN = 0x102
					TGPU = 0
					Return (GPSP)
				}
			}
		}
		Case (GPS_FUNC_REQUESTDXSTATE)
		{
			Local0 = ToInteger(\_SB.PCI0.LPCB.EC0.GPUD) & EC_D_NOTIFY_MASK
			\_SB.PCI0.PEG0.PEGP.DNOT (Local0, 1)
			Return (NV_ERROR_SUCCESS)
		}
	}

	Return (NV_ERROR_UNSUPPORTED)
}
