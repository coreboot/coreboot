/* SPDX-License-Identifier: GPL-2.0-only */

#define PCI_HOST_BRIDGE_OSC_UUID     "33db4d5b-1ff7-401c-9657-7441c03dd766"
#define CXL_HOST_BRIDGE_OSC_UUID     "68f2d50b-c469-4d8a-bd3d-941a103fd3fc"

#define OSC_RET_FAILURE              0x02
#define OSC_RET_UNRECOGNIZED_UUID    0x04
#define OSC_RET_UNRECOGNIZED_REV     0x08
#define OSC_RET_CAPABILITIES_MASKED  0x10

#define OSC_QUERY_SUPPORT_SET        0x01

#define ASL_UUID_UNHANDLED           0x00
#define ASL_UUID_HANDLED             0x01

Scope (\_SB)
{
	/*
	 * \_SB.POSC - OSC handler for PCIe _OSC calls
	 *
	 * Reference:
	 * 6.2.11 in https://uefi.org/htmlspecs/ACPI_Spec_6_4_html/06_Device_Configuration/Device_Configuration.html
	 *
	 * Note:
	 * This call negotiate with OS on fixed firmware capabilities. It doesn't support to runtime
	 * change firmware settings.
	 *
	 * Arguments: (7)
	 * Arg0 - Map to _OSC Arg0. A Buffer containing a UUID
	 * Arg1 - Map to _OSC Arg1. An Integer containing a Revision ID of the buffer format
	 * Arg2 - Map to _OSC Arg2. An Integer containing a count of entries in Arg3
	 * Arg3 - Map to _OSC Arg3. A Buffer containing a list of DWORD capabilities
	 * Arg4 - GrantedPCIeFeatures
	 * Arg5 - IsCxlDomain
	 * Arg6 - GrantedCxlFeatures
	 *
	 * _OSC ASL Return Value:
	 * A Buffer containing a list of capabilities
	 *
	 * Local Variables Assignment:
	 * Local0 - Not used
	 * Local1 - Not used
	 * Local2 - Not used
	 * Local3 - Not used
	 * Local4 - Not used
	 * Local5 - Not used
	 * Local6 - Record whether the UUID is handled
	 * Local7 - Backs up the input value of Arg3
	 *
	 * Field Definitions:
	 * Name - Width	Source	Offset	Description
	 * --------------------------------
	 * RETE - DWord	Arg3	0x00	Returned errors
	 * SUPP - Dword	Arg3	0x04	PCIe Features that OS supported
	 * CTRL - Dword	Arg3	0x08	PCIe Features that firmware grant control to OS
	 * OTRL - Dword	Local7	0x08	PCIe Features that OS requests for control
	 * SUPC - Dword	Arg3	0x0C	CXL Features that OS supported
	 * CTRC - Dword	Arg3	0x10	CXL Features that firmware grant control to OS
	 * OTRC - Dword	Local7	0x10	CXL Features that OS requests for control
	*/

	Method (POSC, 7, NotSerialized)
	{

#define OscArg0              Arg0
#define OscArg1              Arg1
#define OscArg2              Arg2
#define OscArg3              Arg3
#define GrantedPCIeFeatures  Arg4
#define IsCxlDomain          Arg5
#define GrantedCxlFeatures   Arg6

		Local7 = OscArg3
		CreateDWordField (OscArg3, Zero, RETE)
		RETE = 0x0
		Local6 = ASL_UUID_UNHANDLED

		If ((OscArg1 != 0x01))
		{
			RETE = OSC_RET_UNRECOGNIZED_REV
			Return (OscArg3)
		}

		If ((OscArg2 < 0x03))
		{
			RETE = OSC_RET_FAILURE
			Return (OscArg3)
		}

		If ((OscArg0 == ToUUID (PCI_HOST_BRIDGE_OSC_UUID)) ||
			((IsCxlDomain != 0x00) &&
			(OscArg0 == ToUUID (CXL_HOST_BRIDGE_OSC_UUID))))
		{
			CreateDWordField (OscArg3, 0x04, SUPP)
			CreateDWordField (OscArg3, 0x08, CTRL)
			CreateDWordField (Local7, 0x08, OTRL)

			CTRL &= GrantedPCIeFeatures

			/* TODO: further suppress CTRL bits per SUPP caps */

			If ((CTRL != OTRL))
			{
				RETE = OSC_RET_CAPABILITIES_MASKED
			}

			Local6 = ASL_UUID_HANDLED
		}

		If ((IsCxlDomain != 0x00) &&
			(OscArg0 == ToUUID (CXL_HOST_BRIDGE_OSC_UUID)))
		{
			CreateDWordField (OscArg3, 0x0C, SUPC)
			CreateDWordField (OscArg3, 0x10, CTRC)
			CreateDWordField (Local7, 0x10, OTRC)

			CTRC &= GrantedCxlFeatures

			/* TODO: further suppress CTRC bits per SUPC caps */

			If ((CTRC != OTRC))
			{
				RETE = OSC_RET_CAPABILITIES_MASKED
			}

			Local6 = ASL_UUID_HANDLED
		}

		If ((Local6 == ASL_UUID_UNHANDLED))
		{
			RETE = OSC_RET_UNRECOGNIZED_UUID
		}

		Return (OscArg3)
	}
}
